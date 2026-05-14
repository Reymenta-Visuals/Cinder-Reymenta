//
//  VDsession.cpp
//

#include "VDSession.h"

using namespace videodromm;

VDSession::VDSession(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix)
{
	CI_LOG_V("VDSession ctor");
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDUniforms = aVDUniforms;
	// Mix
	mVDMix = aVDMix;
	// Params
	mVDParams = VDParams::create();

	// Animation
	// TODO: needed? mVDAnimation->tapTempo();
	// fbo
	gl::Fbo::Format format;
	//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing

	fboFmt.setColorTextureFormat(fmt);
	mWarpsFbo = gl::Fbo::create(mVDParams->getFboWidth(), mVDParams->getFboHeight(), format.depthTexture());
	mPostFbo = gl::Fbo::create(mVDParams->getFboWidth(), mVDParams->getFboHeight(), format.depthTexture());
	mFxFbo = gl::Fbo::create(mVDParams->getFboWidth(), mVDParams->getFboHeight(), format.depthTexture());
	// 20210103 mGlslPost = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("passthrough.vs")).fragment(loadAsset("post.glsl")));
	mGlslPost = gl::GlslProg::create(gl::GlslProg::Format().vertex(mVDParams->getDefaultVertexString()).fragment(loadAsset("post.glsl")));
	mGlslFx = gl::GlslProg::create(gl::GlslProg::Format().vertex(mVDParams->getDefaultVertexString()).fragment(loadAsset("fx.glsl")));
	mWarpTexture = ci::gl::Texture::create(mVDParams->getFboWidth(), mVDParams->getFboHeight(), ci::gl::Texture::Format().loadTopDown());
	// adjust the content size of the warps

	// TODO 20200305 if (getFboRenderedTexture(0)) Warp::setSize(mWarpList, getFboRenderedTexture(0)->getSize());
	Warp::setSize(mWarpList, ivec2(mVDParams->getFboWidth(), mVDParams->getFboHeight())); //
	// initialize warps
	mSettings = getAssetPath("") / mVDMix->getAssetsPath() / "warps.xml";
	if (fs::exists(mSettings)) {
		// load warp settings from file if one exists
		mWarpList = Warp::readSettings(loadFile(mSettings));
	}
	else {
		// otherwise create a warp from scratch
		mWarpList.push_back(WarpPerspectiveBilinear::create());
	}
	loadFolder(mVDMix->getAssetsPath());

	// Modes
	mModesList[VDDisplayMode::FBO0] = "Fbo0";
	mModesList[VDDisplayMode::FBO1] = "Fbo1";
	mModesList[VDDisplayMode::FBO2] = "Fbo2";
	mModesList[VDDisplayMode::FBO3] = "Fbo3";
	mModesList[VDDisplayMode::FBO4] = "Fbo4";
	mModesList[VDDisplayMode::FBO5] = "Fbo5";
	mModesList[VDDisplayMode::FX] = "Fx";
	mModesList[VDDisplayMode::POST] = "Post";
	mModesList[VDDisplayMode::MIXETTE] = "Mixette";
	mModesList[VDDisplayMode::WARP] = "Warp";

	// reset no matter what, so we don't miss anything
	cmd = -1;
	mFreqWSSend = false;
	reset();

	mCurrentBlend = 0;

}
//void VDSession::loadFromJsonFile(const fs::path& jsonFile) {
//	
//}
void VDSession::setupHttpClient() {

}

bool VDSession::loadFolder(const string& aFolder) {
	unsigned int f = 0;
	bool found = true;
	mVDSettings->setMsg(aFolder);
	if (aFolder != mVDMix->getAssetsPath()) {
		// find mix.json
		std::string mixFileName = "mix.json";
		fs::path mixFile = getAssetPath("") / aFolder / mixFileName;
		if (fs::exists(mixFile)) {
			//mVDMix->clearFboShaderList();
			
		}
	}

	// find fbo...json
	//while (found) {
	//	std::string jsonFileName = "fbo" + toString(f) + ".json";

	//	fs::path jsonFile = getAssetPath("") / aFolder / jsonFileName;
	//	if (fs::exists(jsonFile)) {
	//		//loadFromJsonFile(jsonFile)->createShader()->createUniforms()->compile()->createFboWhenSuccess()->addToFboList();
	//		JsonTree json(loadFile(jsonFile));
	//		fboFromJson(json, f, aFolder);
	//		f++;
	//	}
	//	else {
	//		found = false;
	//	}
	//} //while

	return !found;
}
void VDSession::toggleUI() {
	mShowUI = !mShowUI;
};
bool VDSession::showUI() {
	return mShowUI;
};

std::string VDSession::getModeName(unsigned int aMode) {
	if (aMode > mModesList.size() - 1) aMode = mModesList.size() - 1;
	return mModesList[aMode];
}
unsigned int VDSession::getModesCount() {
	return mModesList.size();
};
void VDSession::update() {

	// fps calculated in main app
	mVDAnimation->update();

	mVDMix->getMixetteTexture(0);
	
	renderWarpsToFbo();
	renderPostToFbo();
	if (mVDUniforms->getUniformValue(mVDUniforms->IDISPLAYMODE) == VDDisplayMode::FX || getElapsedFrames() % 100 == 0) renderFxToFbo();
}
void VDSession::renderPostToFbo()
{
	{
		gl::ScopedFramebuffer fbScp(mPostFbo);
		// clear out the FBO with black
		gl::clear(Color::black());
		//gl::clear(ColorA(0.4f, 0.8f, 0.0f, 0.3f));

		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mPostFbo->getSize());

		// texture binding must be before ScopedGlslProg
		//mWarpsFbo->getColorTexture()
		mWarpTexture->bind(40);
		gl::ScopedGlslProg prog(mGlslPost);

		// not used yet mGlslPost->uniform("TIME", getUniformValue(mVDUniforms->ITIME) - mVDSettings->iStart);;
		mGlslPost->uniform("iResolution", vec3(mVDParams->getFboWidth(), mVDParams->getFboHeight(), 1.0));
		mGlslPost->uniform("iChannel0", 40); // texture 0
		// tmp 20210102
		float iz = mVDUniforms->getUniformValue(mVDUniforms->IZOOM);
		mGlslPost->uniform("iTime", mVDUniforms->getUniformValue(mVDUniforms->ITIME));
		mGlslPost->uniform("iTempoTime", mVDUniforms->getUniformValue(mVDUniforms->ITEMPOTIME));
		mGlslPost->uniform("iRatio", mVDUniforms->getUniformValue(mVDUniforms->IRATIO));
		mGlslPost->uniform("iSobel", mVDUniforms->getUniformValue(mVDUniforms->ISOBEL));
		mGlslPost->uniform("iExposure", mVDUniforms->getUniformValue(mVDUniforms->IEXPOSURE));
		mGlslPost->uniform("iTrixels", mVDUniforms->getUniformValue(mVDUniforms->ITRIXELS)); // trixels if > 0.
		mGlslPost->uniform("iPixelate", mVDUniforms->getUniformValue(mVDUniforms->IPIXELATE)); // pixelate if < 1.
		mGlslPost->uniform("iZoom", mVDUniforms->getUniformValue(mVDUniforms->IZOOM));
		mGlslPost->uniform("iGlitch", mVDUniforms->getUniformValue(mVDUniforms->IGLITCH));
		mGlslPost->uniform("iChromatic", mVDUniforms->getUniformValue(mVDUniforms->ICHROMATIC));
		mGlslPost->uniform("iFlipV", mVDUniforms->getUniformValue(mVDUniforms->IFLIPPOSTV));
		mGlslPost->uniform("iFlipH", mVDUniforms->getUniformValue(mVDUniforms->IFLIPPOSTH));
		mGlslPost->uniform("iInvert", mVDUniforms->getUniformValue(mVDUniforms->IINVERT));
		mGlslPost->uniform("iToggle", mVDUniforms->getUniformValue(mVDUniforms->ITOGGLE));
		mGlslPost->uniform("iGreyScale", mVDUniforms->getUniformValue(mVDUniforms->IGREYSCALE));
		mGlslPost->uniform("iVignette", mVDUniforms->getUniformValue(mVDUniforms->IVIGNETTE));
		mGlslPost->uniform("iRedMultiplier", mVDUniforms->getUniformValue(mVDUniforms->IFRX));
		mGlslPost->uniform("iGreenMultiplier", mVDUniforms->getUniformValue(mVDUniforms->IFGX));
		mGlslPost->uniform("iBlueMultiplier", mVDUniforms->getUniformValue(mVDUniforms->IFBX));
		mGlslPost->uniform("iRenderXYX", mVDUniforms->getUniformValue(mVDUniforms->IRENDERXYX));
		mGlslPost->uniform("iRenderXYY", mVDUniforms->getUniformValue(mVDUniforms->IRENDERXYY));
		mGlslPost->uniform("iMouse", mVDUniforms->getVec4UniformValueByName("iMouse"));
		gl::drawSolidRect(Rectf(0, 0, mVDParams->getFboWidth(), mVDParams->getFboHeight()));
	}
}
void VDSession::renderFxToFbo()
{
	{
		gl::ScopedFramebuffer fbScp(mFxFbo);
		// clear out the FBO with black
		//gl::clear(Color::black());
		gl::clear(ColorA(0.4f, 0.8f, 0.0f, 0.3f));

		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mFxFbo->getSize());

		// texture binding must be before ScopedGlslProg
		//mWarpsFbo->getColorTexture()
		mWarpTexture->bind(41);
		gl::ScopedGlslProg prog(mGlslFx);

		mGlslFx->uniform("iResolution", vec3(mVDParams->getFboWidth(), mVDParams->getFboHeight(), 1.0));
		mGlslFx->uniform("iChannel0", 41); // texture 0	
		mGlslFx->uniform("iChannel1", 41); // texture audio
		mGlslFx->uniform("iTime", mVDUniforms->getUniformValue(mVDUniforms->ITIME));
		mGlslFx->uniform("iGreyScale", mVDUniforms->getUniformValue(mVDUniforms->IGREYSCALE));
		mGlslFx->uniform("iGlitch", mVDUniforms->getUniformValue(mVDUniforms->IGLITCH));
		mGlslFx->uniform("iFreq0", mVDUniforms->getUniformValue(mVDUniforms->IFREQ0));
		mGlslFx->uniform("iMouse", mVDUniforms->getVec4UniformValueByName("iMouse"));


		gl::drawSolidRect(Rectf(0, 0, mVDParams->getFboWidth(), mVDParams->getFboHeight()));
	}
}
void VDSession::renderWarpsToFbo()
{
	{
		gl::ScopedFramebuffer fbScp(mWarpsFbo);
		// clear out the FBO with black
		gl::clear(Color::black());
		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mWarpsFbo->getSize());
		// iterate over the warps and draw their content
		int i = 0;
		int a = 0;

		for (auto& warp : mWarpList) {
			a = warp->getAFboIndex();
			if (a < 0) a = 0; // TODO 20200228 a could be negative if warps3.xml > warps01.json
			i = math<int>::min(a, getFboShaderListSize() - 1);
			warp->draw(mVDMix->getRenderedMixetteTexture(0));
		}
		mWarpTexture = mWarpsFbo->getColorTexture();
	}
}


void VDSession::resetSomeParams() {
	// parameters not exposed in json file

}

void VDSession::reset()
{
	// parameters exposed in json file
	resetSomeParams();
}

void VDSession::blendRenderEnable(bool render) {
	mVDAnimation->blendRenderEnable(render);
}

void VDSession::fileDrop(FileDropEvent event) {
	std::string ext = "";
	//string fileName = "";

	unsigned int index = (unsigned int)((event.getX() - mVDParams->getUILargeW()) / (mVDParams->getUILargePreviewW() + mVDParams->getUIMargin()));
	//int y = (int)(event.getY());
	//if (index < 2 || y < mVDSettings->uiYPosRow3 || y > mVDSettings->uiYPosRow3 + mVDSettings->uiPreviewH) index = 0;
	ci::fs::path mPath = event.getFile(event.getNumFiles() - 1);
	std::string absolutePath = mPath.string();
	// use the last of the dropped files
	int dotIndex = absolutePath.find_last_of(".");
	int slashIndex = absolutePath.find_last_of("\\");

	if (dotIndex != std::string::npos && dotIndex > slashIndex) {
		ext = absolutePath.substr(dotIndex + 1);
		//fileName = absolutePath.substr(slashIndex + 1, dotIndex - slashIndex - 1);
		//if (ext == "json") {
		//	JsonTree json(loadFile(absolutePath));
		//	fboFromJson(json);
		//}else

		 if (ext == "glsl" || ext == "frag" || ext == "fs") {
			loadFragmentShader(absolutePath, index);
		}
		else if (ext == "png" || ext == "jpg") {
			if (index < 1) index = 1;
			// 20211227 useless? if (index > 3) index = 3;
			loadImageFile(absolutePath, index);
		}
		else if (ext == "mp4" ) {
			if (index < 1) index = 1;
			loadVideoFile(absolutePath, index);
		}
		else if (ext == "wav" || ext == "mp3") {
			loadAudioFile(absolutePath);
		}
		/*
		else if (ext == "mov") {
			loadMovie(absolutePath, index);
		}

		else if (ext == "") {
			// 20211108 never called?
			// try loading image sequence from dir
			//if (!loadImageSequence(absolutePath, index)) {
				// try to load a folder of shaders
				//loadShaderFolder(absolutePath);
			//}
		}*/
	}
	else {
		// folder was dropped
		std::string folderName = absolutePath.substr(slashIndex + 1);
		loadFolder(folderName);
		// try loading image sequence from dir
		// if (!loadImageSequence(absolutePath, index)) {
			// try to load a folder of shaders
			//loadShaderFolder(absolutePath);
		//}
	}
}

/*bool VDSession::loadImageSequence(const string& aFolder, unsigned int aTextureIndex) {
	return mVDMix->loadImageSequence(aFolder, aTextureIndex);
}*/
void VDSession::loadAudioFile(const string& aFile) {
	//mTextureList[0]->loadFromFullPath(aFile);
}
#pragma region events
bool VDSession::handleMouseMove(MouseEvent& event)
{
	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseMove(mWarpList, event)) {
		// let your application perform its mouseMove handling here
		handled = false;
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseDown(MouseEvent& event)
{
	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDown(mWarpList, event)) {
		// let your application perform its mouseMove handling here event.getPos()
		handled = mVDMix->handleMouseDown(event);
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseDrag(MouseEvent& event)
{

	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDrag(mWarpList, event)) {
		// let your application perform its mouseMove handling here
		handled = false;
		mVDMix->handleMouseDrag(event);
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseUp(MouseEvent& event)
{
	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseUp(mWarpList, event)) {
		// let your application perform its mouseMove handling here
		handled = false;
	}

	event.setHandled(handled);
	return event.isHandled();
}


bool VDSession::handleKeyDown(KeyEvent& event)
{
	bool handled = true;
	float newValue;
#if defined( CINDER_COCOA )
	bool isModDown = event.isMetaDown();
#else // windows
	bool isModDown = event.isControlDown();
#endif
	bool isShiftDown = event.isShiftDown();
	bool isAltDown = event.isAltDown();
	CI_LOG_V("session keydown: " + toString(event.getCode()) + " ctrl: " + toString(isModDown) + " shift: " + toString(isShiftDown) + " alt: " + toString(isAltDown));
	// pass this key event to the warp editor first
	if (!Warp::handleKeyDown(mWarpList, event)) {

		switch (event.getCode()) {
		case KeyEvent::KEY_s:
			// Spout UI
			mVDMix->selectSenderPanel();
			break;
		case KeyEvent::KEY_w:
			CI_LOG_V("oscConnect");
			if (isModDown) {
				//oscConnect();
			}
			else {
				// handled in main app
				//handled = false;
				// toggle warp edit mode
				Warp::enableEditMode(!Warp::isEditModeEnabled());
			}
			break;
		case KeyEvent::KEY_UP:
			// imgseq next
			//incrementSequencePosition();
			break;
		case KeyEvent::KEY_DOWN:
			// imgseq next
			//decrementSequencePosition();
			break;
		case KeyEvent::KEY_v:
			//if (isModDown) fboFlipV(0);// TODO other indexes mVDSettings->mFlipV = !mVDSettings->mFlipV; useless?
			break;
		case KeyEvent::KEY_h:
			if (isModDown) {
				//fboFlipH(0);// TODO other indexes mVDSettings->mFlipH = !mVDSettings->mFlipH; useless?
			}
			else {
				// ui visibility
				toggleUI();
			}
			break;
		case KeyEvent::KEY_d:
			/*if (isAltDown) {
				setSpeed(0, getSpeed(0) - 0.01f);
			}
			else {
				setSpeed(0, getSpeed(0) + 0.01f);
			}*/
			break;
		default:
			CI_LOG_V("session keydown: " + toString(event.getCode()));
			handled = false;
			break;
		}
	}
	CI_LOG_V((handled ? "session keydown handled " : "session keydown not handled "));
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleKeyUp(KeyEvent& event) {
	bool handled = true;

	// pass this key event to the warp editor first
	if (!Warp::handleKeyUp(mWarpList, event)) {
		if (!mVDAnimation->handleKeyUp(event)) {
			// Animation did not handle the key, so handle it here
			switch (event.getCode()) {

			default:
				CI_LOG_V("session keyup: " + toString(event.getCode()));
				handled = false;
				break;
			}
		}
	}
	CI_LOG_V((handled ? "session keyup handled " : "session keyup not handled "));
	event.setHandled(handled);
	return event.isHandled();
}
#pragma endregion events
// fbos
#pragma region fbos

#pragma endregion fbos
// shaders

ci::gl::TextureRef VDSession::getFboRenderedTexture(unsigned int aFboIndex) {
	return mVDMix->getFboRenderedTexture(aFboIndex);
}
ci::gl::TextureRef VDSession::getFboTexture(unsigned int aFboIndex) {
	return mVDMix->getFboTexture(aFboIndex);
}

ci::gl::TextureRef VDSession::getFboShaderTexture(unsigned int aFboShaderIndex) {
	return mVDMix->getFboRenderedTexture(aFboShaderIndex); //20201229 or getTexture?
};

std::vector<ci::gl::GlslProg::Uniform> VDSession::getFboShaderUniforms(unsigned int aFboShaderIndex) {
	return mVDMix->getFboShaderUniforms(aFboShaderIndex);
}

float VDSession::getUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex) {
	return mVDMix->getUniformValueByLocation(aFboShaderIndex, aLocationIndex);
};
void VDSession::setUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex, float aValue) {
	//mFboShaderList[aFboShaderIndex]->setUniformValueByLocation(aLocationIndex, aValue);
	mVDMix->setUniformValueByLocation(aFboShaderIndex, aLocationIndex, aValue);
};
ci::gl::TextureRef VDSession::getMixetteTexture(unsigned int aFboIndex) {
	return mVDMix->getMixetteTexture(aFboIndex);
}
ci::gl::TextureRef VDSession::getRenderedMixetteTexture(unsigned int aFboIndex) {
	return mVDMix->getRenderedMixetteTexture(aFboIndex);
}
ci::gl::TextureRef VDSession::getPostFboTexture() {
	return mPostFbo->getColorTexture();
};
ci::gl::TextureRef VDSession::getFxFboTexture() {
	return mFxFbo->getColorTexture();
};
ci::gl::TextureRef VDSession::getWarpFboTexture() {
	return mWarpsFbo->getColorTexture();
};
ci::gl::TextureRef VDSession::getRenderedWarpFboTexture() {
	return mWarpTexture;
};
void VDSession::resize() {
	// tell the fbos our window has been resized, so they properly scale up or down
	Warp::handleResize(mWarpList);
	Warp::setSize(mWarpList, ivec2(mVDParams->getFboWidth(), mVDParams->getFboHeight()));
}
unsigned int VDSession::getWarpCount() { return mWarpList.size(); };
std::string	 VDSession::getWarpName(unsigned int aWarpIndex) { return mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->getName(); };// or trycatch
int VDSession::getWarpWidth(unsigned int aWarpIndex) { return mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->getWidth(); };
int VDSession::getWarpHeight(unsigned int aWarpIndex) { return mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->getHeight(); };
void VDSession::setWarpWidth(unsigned int aWarpIndex, int aWidth) {
	mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->setWidth(aWidth);
	mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->resize();
};
void VDSession::setWarpHeight(unsigned int aWarpIndex, int aHeight) {
	Warp::handleResize(mWarpList);
	Warp::setSize(mWarpList, ivec2(mVDParams->getFboWidth(), aHeight));
	mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->setHeight(aHeight);

};
unsigned int VDSession::getWarpAFboIndex(unsigned int aWarpIndex) { return mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->getAFboIndex(); };
unsigned int VDSession::getWarpBFboIndex(unsigned int aWarpIndex) { return mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->getBFboIndex(); };
void VDSession::setWarpAFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex) {
	if (aWarpIndex < mWarpList.size() && aWarpFboIndex < mVDMix->getFboShaderListSize()) {
		mWarpList[aWarpIndex]->setAFboIndex(aWarpFboIndex);
	}
}
void VDSession::setWarpBFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex) {
	if (aWarpIndex < mWarpList.size() && aWarpFboIndex < mVDMix->getFboShaderListSize()) {
		mWarpList[aWarpIndex]->setBFboIndex(aWarpFboIndex);
	}
}

void VDSession::createWarp() {
	auto warp = WarpBilinear::create();
	warp->setName("New");
	warp->setAFboIndex(0);
	warp->setBFboIndex(0);
	warp->setAShaderIndex(0);
	warp->setBShaderIndex(0);
	warp->setAShaderFilename("inputImage.fs");
	warp->setBShaderFilename("inputImage.fs");
	warp->setATextureFilename("audio");
	warp->setBTextureFilename("audio");
	mWarpList.push_back(WarpBilinear::create());
}
std::string VDSession::getFboShaderName(unsigned int aFboIndex) {
	return mVDMix->getFboShaderName(aFboIndex);
}
std::string VDSession::getFboTextureName(unsigned int aFboIndex) {
	return mVDMix->getFboTextureName(aFboIndex);
}
void VDSession::saveWarps() {
	/*int i = 0;
	for (auto &warp : mWarpList) {
		//
		warp->setAShaderFilename(getFboShaderName(warp->getAFboIndex()));
		warp->setATextureFilename(getFboTextureName(warp->getAFboIndex()));
		JsonTree		json;
		string jsonFileName = "warp" + toString(i) + ".json";
		fs::path jsonFile = getAssetPath("") / mVDSettings->mAssetsPath / jsonFileName;
		// write file
		json.pushBack(warp->toJson());
		json.write(jsonFile);
		i++;
	}
	// save warp settings*/
	Warp::writeSettings(mWarpList, writeFile(mSettings));
}
void VDSession::setAnim(unsigned int aCtrl, unsigned int aAnim) {
	mVDAnimation->setAnim(aCtrl, aAnim);
}
unsigned int VDSession::getAnim(unsigned int aCtrl) {
	return mVDAnimation->getAnim(aCtrl);
}

// control values
void VDSession::toggleValue(unsigned int aCtrl) {
	float val = mVDUniforms->getUniformValue(aCtrl);
	if (val > 0.0f) {
		mVDUniforms->setUniformValue(aCtrl, 0.0f);
	}
	else {
		mVDUniforms->setUniformValue(aCtrl, 1.0f);
	}
};

float VDSession::getMinUniformValue(unsigned int aIndex) {
	return mVDUniforms->getMinUniformValue(aIndex);
};
float VDSession::getMaxUniformValue(unsigned int aIndex) {
	return mVDUniforms->getMaxUniformValue(aIndex);
};
float VDSession::getDefaultUniformValue(unsigned int aIndex) {
	return mVDUniforms->getDefaultUniformValue(aIndex);
}
int VDSession::getSampler2DUniformValueByName(const std::string& aName) {
	return mVDUniforms->getSampler2DUniformValueByName(aName);
};
float VDSession::getUniformValueByName(const std::string& aCtrlName) {
	return mVDUniforms->getUniformValueByName(aCtrlName);
};
