#include "Textures.h"

using namespace Reymenta;

Textures::Textures(ParameterBagRef aParameterBag, ShadersRef aShadersRef)
{
	mParameterBag = aParameterBag;
	mShaders = aShadersRef;
	// instanciate the logger class
	log = Logger::create("TexturesLog.txt");
	log->logTimedString("Textures constructor");

	//createPreviewFbo();//mFboWidth/4 or 16
	// mix fbo at index 0
	FrameBuffa mixFbo;
	mixFbo.fbo = gl::Fbo(mParameterBag->mFboWidth, mParameterBag->mFboHeight);
	sprintf_s(mixFbo.name, "mix");
	mixFbo.isFlipV = false;
	mixFbo.isFlipH = false;
	mFbos.push_back(mixFbo);

	// preview fbo at index 5
	//mFbos.push_back(gl::Fbo(mParameterBag->mFboWidth, mParameterBag->mFboHeight));//640x480
	for (size_t m = mFbos.size(); m < mParameterBag->MAX; m++) {
		//fbo
		FrameBuffa fb;
		fb.fbo = gl::Fbo(mParameterBag->mFboWidth, mParameterBag->mFboHeight);
		sprintf_s(fb.name, "fbo%d", m);
		fb.isFlipV = false;
		fb.isFlipH = false;
		mFbos.push_back(fb);
		//thumb fbo
		FrameBuffa tfb;
		tfb.fbo = gl::Fbo(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight);
		sprintf_s(tfb.name, "thumbfbo%d", m);
		tfb.isFlipV = false;
		tfb.isFlipH = false;
		mThumbFbos.push_back(tfb);
		//mFbos.push_back(gl::Fbo(mParameterBag->mFboWidth, mParameterBag->mFboHeight));
		//mThumbFbos.push_back(gl::Fbo(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight));
	}
	currentShadaThumbIndex = 0;
	// audio fbo at index 6
	sprintf_s(mFbos[mParameterBag->mAudioFboIndex].name, "audio");// = gl::Fbo(mParameterBag->mFboWidth, mParameterBag->mFboHeight);
	mFbos[mParameterBag->mAudioFboIndex].isFlipV = true;//.getTexture(0).setFlipped(true);

	//const char* fboNames[] = { "mix", "", "", "", "", "", "", "", "", "", "audio", "", "1", "2", "3", "4" };
	sprintf_s(mFbos[1].name, "left");
	sprintf_s(mFbos[2].name, "right");
	sprintf_s(mFbos[3].name, "warp1");
	sprintf_s(mFbos[4].name, "warp2");
	sprintf_s(mFbos[5].name, "preview");
	sprintf_s(mFbos[6].name, "abp");
	sprintf_s(mFbos[7].name, "live");
	sprintf_s(mFbos[8].name, "sphere");
	sprintf_s(mFbos[9].name, "mesh");


	for (int i = 0; i < 1024; ++i) dTexture[i] = (unsigned char)(Rand::randUint() & 0xFF);
	// store it as a 512x2 texture in the first texture
	Texta audioTex;
	sprintf_s(audioTex.name, "Audio");
	audioTex.sequenceIndex = 0;
	audioTex.isSequence = false;
	textas.push_back(audioTex);
	sTextures.push_back(gl::Texture(dTexture, GL_LUMINANCE, 512, 2));

	fs::path localFile;
	for (int j = 0; j < mParameterBag->MAX - 1; j++)
	{
		fileName = toString(j) + ".jpg";
		localFile = getAssetPath("") / mParameterBag->mAssetsPath / fileName;
		if (fs::exists(localFile))
		{
			gl::Texture img(loadImage(localFile));// TODO , gl::Texture::Format().magFilter(GL_LINEAR).minFilter(GL_LINEAR).loadTopDown());
			//img.setFlipped();
			sTextures.push_back(img);
		}
		else
		{
			gl::Texture img(loadImage(loadAsset("reymenta.jpg")));
			sTextures.push_back(img);
		}
		Texta tex;
		sprintf_s(tex.name, "tex%d", j);
		tex.sequenceIndex = 0;
		tex.isSequence = false;
		textas.push_back(tex);

	}
	sprintf_s(textas[1].name, "img1");
	sprintf_s(textas[2].name, "img2");
	sprintf_s(textas[3].name, "img3");
	sprintf_s(textas[4].name, "4pvwFbo");
	sprintf_s(textas[5].name, "5mixFbo");
	sprintf_s(textas[6].name, "6leftFbo");
	sprintf_s(textas[7].name, "7rightFbo");
	sprintf_s(textas[8].name, "8warp1Fbo");
	sprintf_s(textas[9].name, "9warp2Fbo");
	sprintf_s(textas[10].name, "10spout");
	sprintf_s(textas[11].name, "11LiveFbo");

	// image sequence
	playheadFrameInc = 1;
	// init with one sequence
	sequence seq;
	seq.filePath = "none";
	sprintf_s(seq.folder, "none");
	seq.index = 0;
	seq.loadingFilesComplete = true;
	seq.loadingPaused = true;
	seq.framesLoaded = 0;
	seq.currentLoadedFrame = 0;
	seq.nextIndexFrameToTry = 0;
	seq.playing = false;
	seq.speed = 1;
	seq.ext = "png";
	seq.prefix = "none";
	seq.nextIndexFrameToTry = 0;
	seq.playheadPosition = 0;
	sequences.push_back(seq);
}
char* Textures::getTextureName(int index) {
	return textas[index].name;
}
void Textures::setTextureName(int index, char* name) {
	sprintf_s(textas[index].name, name);

}
char* Textures::getFboName(int index) {
	return mFbos[index].name;
}

/*void Textures::createWarpInput()
{
WarpInput newWarpInput;
newWarpInput.leftIndex = 0;
newWarpInput.leftMode = 0;
newWarpInput.rightIndex = 0;
newWarpInput.rightMode = 0;
newWarpInput.controlValues[18] = 0.5;
newWarpInput.hasTexture = false;
warpInputs.push_back(newWarpInput);
// init mixTextures
mMixesFbos.push_back(gl::Fbo(mParameterBag->mFboWidth, mParameterBag->mFboHeight));
}*/

void Textures::setAudioTexture(unsigned char *signal)
{
	sTextures[0] = gl::Texture(signal, GL_LUMINANCE, 512, 2);
}
void Textures::setTexture(int index, string fileName)
{
	if (index > mParameterBag->MAX - 1) index = mParameterBag->MAX - 1;
	if (index > 0)
	{
		try
		{
			string pathToAssetFile = (getAssetPath("") / fileName).string();

			if (!fs::exists(pathToAssetFile))
			{
				log->logTimedString("asset file not found: " + fileName);
			}
			else
			{
				log->logTimedString("asset found file: " + fileName);
				if (index < sTextures.size())
				{
					sTextures[index] = gl::Texture(loadImage(loadAsset(fileName)));
				}
				else
				{
					sTextures.push_back(gl::Texture(loadImage(loadAsset(fileName))));
				}
				log->logTimedString("asset loaded: " + fileName);
			}
		}
		catch (...)
		{
			log->logTimedString("Load asset error: " + fileName);
		}
	}
}
void Textures::flipTexture(int index)
{
	sTextures[index].setFlipped(!sTextures[index].isFlipped());
	sprintf(buf, "flipTexture index %d after: %d", index, sTextures[index].isFlipped());
	mParameterBag->mMsg = buf;
	mParameterBag->newMsg = true;
}
void Textures::flipFboV(int index)
{
	mFbos[index].isFlipV = !mFbos[index].isFlipV;
}
void Textures::flipFboH(int index)
{
	mFbos[index].isFlipH = !mFbos[index].isFlipH;
}
/*void Textures::setCurrentFboIndex(int aFbo)
{
if (aFbo < mFbos.size()) mParameterBag->mCurrentFboIndex = aFbo;
}
ci::gl::Texture Textures::getFboThumb(int index)
{
if (index > mThumbFbos.size() - 1) index = mThumbFbos.size() - 1;
return mThumbFbos[index].getTexture();
}
*/
ci::gl::Texture Textures::getTexture(int index)
{
	if (index > sTextures.size() - 1) index = sTextures.size() - 1;
	return sTextures[index];
}
ci::gl::Texture Textures::getWarpTexture(int index)
{
	if (index > mParameterBag->MAX - 1) index = mParameterBag->MAX - 1;
	return mParameterBag->mWarpFbos[index].fbo.getTexture();
}
void Textures::setTexture(int index, ci::gl::Texture texture)
{
	if (index < sTextures.size())
	{
		sTextures[index] = texture;
	}
}
ci::gl::Texture Textures::getFboTexture(int index)
{
	if (index > mFbos.size() - 1) index = mFbos.size() - 1;
	return mFbos[index].fbo.getTexture();
}
GLuint Textures::getFboTextureId(int index)
{
	if (index > mFbos.size() - 1) index = mFbos.size() - 1;
	return  mFbos[index].fbo.getTexture().getId();
}

ci::gl::Fbo Textures::getFbo(int index)
{
	// fbo
	return mFbos[index].fbo;
}
GLuint Textures::getShaderThumbTextureId(int index)
{
	if (index > mThumbFbos.size() - 1) index = mThumbFbos.size() - 1;

	return  mThumbFbos[index].fbo.getTexture().getId();
}
void Textures::loadImageFile(int index, string aFile)
{
	try
	{
		// try loading image file
		if (index > 0) sTextures[index] = gl::Texture(loadImage(aFile));
		mParameterBag->isUIDirty = true;
	}
	catch (...)
	{
		log->logTimedString("Error loading image: " + aFile);
	}
}

void Textures::loadMovieFile(int index, string aFile)
{
	try
	{
		mMovieIndex = index;
		// try loading video file
		mMovie = qtime::MovieGl::create(aFile);
		mMovie->setLoop();
		mMovie->play();
	}
	catch (...)
	{
		log->logTimedString("Error loading video: " + aFile);
		mMovie->reset();

	}
}

void Textures::update()
{
	if (mMovie) {
		if (mMovie->hasVisuals()) {
			if (mMovie->isPlaying()) {
				// binding crashes later on:  sTextures[mMovieIndex] = mMovie->getTexture();
			}
		}
	}
	if (sequences.size() > 0) {
		for (int i = 0; i < sequences.size(); i++) {
			if (sequences[i].playing) updateSequence(i);
			if (!sequences[i].loadingFilesComplete) loadNextImageFromDisk(i);
		}

	}

}
void Textures::renderWarpFbos()
{
	/*TODO	mShaders->getWarpShader();
				if (mWarp.textureMode == 0)
				{
				// 0 for input texture
				getSenderTexture(mWarp.textureIndex)->bind(0);
				}
				else
				{
				// 1 for shader
				getFboTexture(mWarp.textureIndex)->bind(0);
				}
				mShaders->getWarpShader()->uniform("iAlpha", mParameterBag->controlValues[4]);
				gl::draw(mMesh);
				*/
}
void Textures::renderShadaThumbFbo()
{
	// start profiling
	auto start = Clock::now();
	mThumbFbos[currentShadaThumbIndex].fbo.bindFramebuffer();
	gl::setViewport(mThumbFbos[currentShadaThumbIndex].fbo.getBounds());

	// clear the FBO
	gl::clear(Color(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
	gl::setMatricesWindow(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight);

	aShader = mShaders->getShader(currentShadaThumbIndex).shader;
	if (mShaders->getShader(currentShadaThumbIndex).active)
	{
		aShader->bind();
		aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
		aShader->uniform("iResolution", Vec3f(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight, 1.0));
		aShader->uniform("iChannelResolution", mParameterBag->iChannelResolution, 4);
		aShader->uniform("iMouse", Vec4f(mParameterBag->mRenderPosXY.x, mParameterBag->mRenderPosXY.y, mParameterBag->iMouse.z, mParameterBag->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
		aShader->uniform("iZoom", mParameterBag->iZoomLeft);
		aShader->uniform("iChannel0", mParameterBag->iChannels[0]);
		aShader->uniform("iChannel1", mParameterBag->iChannels[1]);
		aShader->uniform("iAudio0", 0);
		aShader->uniform("iFreq0", mParameterBag->iFreqs[0]);
		aShader->uniform("iFreq1", mParameterBag->iFreqs[1]);
		aShader->uniform("iFreq2", mParameterBag->iFreqs[2]);
		aShader->uniform("iFreq3", mParameterBag->iFreqs[3]);
		aShader->uniform("iChannelTime", mParameterBag->iChannelTime, 4);
		aShader->uniform("iColor", Vec3f(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3]));
		aShader->uniform("iBackgroundColor", Vec3f(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
		aShader->uniform("iSteps", (int)mParameterBag->controlValues[20]);
		aShader->uniform("iRatio", mParameterBag->controlValues[11]);
		aShader->uniform("width", 1);
		aShader->uniform("height", 1);
		aShader->uniform("iRenderXY", mParameterBag->mRenderXY);
		aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
		aShader->uniform("iBlendmode", mParameterBag->iBlendMode);
		aShader->uniform("iRotationSpeed", mParameterBag->controlValues[19]);
		aShader->uniform("iCrossfade", mParameterBag->controlValues[21]);
		aShader->uniform("iPixelate", mParameterBag->controlValues[15]);
		aShader->uniform("iExposure", mParameterBag->controlValues[14]);
		aShader->uniform("iDeltaTime", mParameterBag->iDeltaTime);
		aShader->uniform("iFade", (int)mParameterBag->iFade);
		aShader->uniform("iToggle", (int)mParameterBag->controlValues[46]);
		aShader->uniform("iLight", (int)mParameterBag->iLight);
		aShader->uniform("iLightAuto", (int)mParameterBag->iLightAuto);
		aShader->uniform("iGreyScale", (int)mParameterBag->iGreyScale);
		aShader->uniform("iTransition", mParameterBag->iTransition);
		aShader->uniform("iAnim", mParameterBag->iAnim.value());
		aShader->uniform("iRepeat", (int)mParameterBag->iRepeat);
		aShader->uniform("iVignette", (int)mParameterBag->controlValues[47]);
		aShader->uniform("iInvert", (int)mParameterBag->controlValues[48]);
		aShader->uniform("iDebug", (int)mParameterBag->iDebug);
		aShader->uniform("iShowFps", (int)mParameterBag->iShowFps);
		aShader->uniform("iFps", mParameterBag->iFps);
		aShader->uniform("iTempoTime", mParameterBag->iTempoTime);
		aShader->uniform("iGlitch", (int)mParameterBag->controlValues[45]);
		aShader->uniform("iBeat", mParameterBag->iBeat);
		aShader->uniform("iSeed", mParameterBag->iSeed);
		aShader->uniform("iFlipH", mFbos[mParameterBag->mMixFboIndex].isFlipH);
		aShader->uniform("iFlipV", mFbos[mParameterBag->mMixFboIndex].isFlipV);

		for (size_t m = 0; m < 2; m++)
		{
			getTexture(m).bind(m);
		}
		gl::drawSolidRect(Rectf(0, 0, mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight));
		// stop drawing into the FBO
		mThumbFbos[currentShadaThumbIndex].fbo.unbindFramebuffer();

		for (size_t m = 0; m < 2; m++)
		{
			getTexture(m).unbind();
		}

		aShader->unbind();
	}
	auto end = Clock::now();
	//auto msdur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	//int milli = msdur.count();
	auto nsdur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	int micro = nsdur.count();
	mShaders->setShaderMicroSeconds(currentShadaThumbIndex, micro);

	// increment shada thumb index
	currentShadaThumbIndex++;
	// mThumbFbos must equal mFragmentShaders size
	if (mThumbFbos.size() < mShaders->getCount()) {
		FrameBuffa tfb;
		tfb.fbo = gl::Fbo(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight);
		sprintf_s(tfb.name, "new");
		tfb.isFlipV = false;
		tfb.isFlipH = false;
		mThumbFbos.push_back(tfb);
	}
	if (currentShadaThumbIndex > mShaders->getCount() - 1)
	{
		currentShadaThumbIndex = 0;
	}
}

void Textures::draw()
{
	// start profiling
	auto start = Clock::now();
	mParameterBag->iSeed = randFloat();
	//renderWarpFbos();
	if (mParameterBag->mRenderThumbs) renderShadaThumbFbo();
#pragma region left
	/**********************************************
	* library FBOs
	* start of mLibraryFbos[mParameterBag->mLeftFboIndex]
	*/

	mFbos[mParameterBag->mLeftFboIndex].fbo.bindFramebuffer();
	gl::setViewport(mFbos[mParameterBag->mLeftFboIndex].fbo.getBounds());

	// clear the FBO
	gl::clear(Color(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
	gl::setMatricesWindow(mParameterBag->mFboWidth, mParameterBag->mFboHeight);

	aShader = mShaders->getShader(mParameterBag->mLeftFragIndex).shader;
	aShader->bind();
	aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
	//aShader->uniform("iResolution", Vec3f(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 1.0));
	aShader->uniform("iResolution", Vec3f(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 1.0));
	aShader->uniform("iChannelResolution", mParameterBag->iChannelResolution, 4);
	aShader->uniform("iMouse", Vec4f(mParameterBag->mRenderPosXY.x, mParameterBag->mRenderPosXY.y, mParameterBag->iMouse.z, mParameterBag->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	aShader->uniform("iZoom", mParameterBag->iZoomLeft);
	aShader->uniform("iChannel0", mParameterBag->iChannels[0]);
	aShader->uniform("iChannel1", mParameterBag->iChannels[1]);
	aShader->uniform("iChannel2", mParameterBag->iChannels[2]);
	aShader->uniform("iChannel3", mParameterBag->iChannels[3]);
	aShader->uniform("iChannel4", mParameterBag->iChannels[4]);
	aShader->uniform("iChannel5", mParameterBag->iChannels[5]);
	aShader->uniform("iChannel6", mParameterBag->iChannels[6]);
	aShader->uniform("iChannel7", mParameterBag->iChannels[7]);
	aShader->uniform("iAudio0", 0);
	aShader->uniform("iFreq0", mParameterBag->iFreqs[0]);
	aShader->uniform("iFreq1", mParameterBag->iFreqs[1]);
	aShader->uniform("iFreq2", mParameterBag->iFreqs[2]);
	aShader->uniform("iFreq3", mParameterBag->iFreqs[3]);
	aShader->uniform("iChannelTime", mParameterBag->iChannelTime, 4);
	aShader->uniform("iColor", Vec3f(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3]));
	aShader->uniform("iBackgroundColor", Vec3f(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
	aShader->uniform("iSteps", (int)mParameterBag->controlValues[20]);
	aShader->uniform("iRatio", mParameterBag->controlValues[11]);
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mParameterBag->mLeftRenderXY);
	aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
	aShader->uniform("iBlendmode", mParameterBag->iBlendMode);
	aShader->uniform("iRotationSpeed", mParameterBag->controlValues[19]);
	aShader->uniform("iCrossfade", mParameterBag->controlValues[21]);
	aShader->uniform("iPixelate", mParameterBag->controlValues[15]);
	aShader->uniform("iExposure", mParameterBag->controlValues[14]);
	aShader->uniform("iDeltaTime", mParameterBag->iDeltaTime);
	aShader->uniform("iFade", (int)mParameterBag->iFade);
	aShader->uniform("iToggle", (int)mParameterBag->controlValues[46]);
	aShader->uniform("iLight", (int)mParameterBag->iLight);
	aShader->uniform("iLightAuto", (int)mParameterBag->iLightAuto);
	aShader->uniform("iGreyScale", (int)mParameterBag->iGreyScale);
	aShader->uniform("iTransition", mParameterBag->iTransition);
	aShader->uniform("iAnim", mParameterBag->iAnim.value());
	aShader->uniform("iRepeat", (int)mParameterBag->iRepeat);
	aShader->uniform("iVignette", (int)mParameterBag->controlValues[47]);
	aShader->uniform("iInvert", (int)mParameterBag->controlValues[48]);
	aShader->uniform("iDebug", (int)mParameterBag->iDebug);
	aShader->uniform("iShowFps", (int)mParameterBag->iShowFps);
	aShader->uniform("iFps", mParameterBag->iFps);
	aShader->uniform("iTempoTime", mParameterBag->iTempoTime);
	aShader->uniform("iGlitch", (int)mParameterBag->controlValues[45]);
	aShader->uniform("iBeat", mParameterBag->iBeat);
	aShader->uniform("iSeed", mParameterBag->iSeed);
	aShader->uniform("iFlipH", mFbos[mParameterBag->mLeftFboIndex].isFlipH);
	aShader->uniform("iFlipV", mFbos[mParameterBag->mLeftFboIndex].isFlipV);

	for (size_t m = 0; m < mParameterBag->MAX; m++)
	{
		getTexture(m).bind(m);
	}
	gl::drawSolidRect(Rectf(0, 0, mParameterBag->mFboWidth, mParameterBag->mFboHeight));
	// stop drawing into the FBO
	mFbos[mParameterBag->mLeftFboIndex].fbo.unbindFramebuffer();

	for (size_t m = 0; m < mParameterBag->MAX; m++)
	{
		getTexture(m).unbind();
	}

	aShader->unbind();
	sTextures[6] = mFbos[mParameterBag->mLeftFboIndex].fbo.getTexture();
	/*
	* end of mLibraryFbos[mParameterBag->mLeftFboIndex]
	***********************************************/
#pragma endregion left
#pragma region right
	/***********************************************
	* start of mLibraryFbos[mParameterBag->mRightFboIndex]
	*/

	mFbos[mParameterBag->mRightFboIndex].fbo.bindFramebuffer();
	gl::setViewport(mFbos[mParameterBag->mRightFboIndex].fbo.getBounds());

	// clear the FBO
	gl::clear(Color(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
	gl::setMatricesWindow(mParameterBag->mFboWidth, mParameterBag->mFboHeight);

	aShader = mShaders->getShader(mParameterBag->mRightFragIndex).shader;
	aShader->bind();
	aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
	aShader->uniform("iResolution", Vec3f(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 1.0));
	aShader->uniform("iChannelResolution", mParameterBag->iChannelResolution, 4);
	aShader->uniform("iMouse", Vec4f(mParameterBag->mRenderPosXY.x, mParameterBag->mRenderPosXY.y, mParameterBag->iMouse.z, mParameterBag->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	aShader->uniform("iZoom", mParameterBag->iZoomRight);
	aShader->uniform("iChannel0", mParameterBag->iChannels[0]);
	aShader->uniform("iChannel1", mParameterBag->iChannels[1]);
	aShader->uniform("iChannel2", mParameterBag->iChannels[2]);
	aShader->uniform("iChannel3", mParameterBag->iChannels[3]);
	aShader->uniform("iChannel4", mParameterBag->iChannels[4]);
	aShader->uniform("iChannel5", mParameterBag->iChannels[5]);
	aShader->uniform("iChannel6", mParameterBag->iChannels[6]);
	aShader->uniform("iChannel7", mParameterBag->iChannels[7]);
	aShader->uniform("iAudio0", 0);
	aShader->uniform("iFreq0", mParameterBag->iFreqs[0]);
	aShader->uniform("iFreq1", mParameterBag->iFreqs[1]);
	aShader->uniform("iFreq2", mParameterBag->iFreqs[2]);
	aShader->uniform("iFreq3", mParameterBag->iFreqs[3]);
	aShader->uniform("iChannelTime", mParameterBag->iChannelTime, 4);
	aShader->uniform("iColor", Vec3f(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3]));// mParameterBag->iColor);
	aShader->uniform("iBackgroundColor", Vec3f(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));// mParameterBag->iBackgroundColor);
	aShader->uniform("iSteps", (int)mParameterBag->controlValues[20]);
	aShader->uniform("iRatio", mParameterBag->controlValues[11]);//check if needed: +1;//mParameterBag->iRatio); 
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mParameterBag->mRightRenderXY);
	aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
	aShader->uniform("iBlendmode", mParameterBag->iBlendMode);
	aShader->uniform("iRotationSpeed", mParameterBag->controlValues[19]);
	aShader->uniform("iCrossfade", mParameterBag->controlValues[21]);
	aShader->uniform("iPixelate", mParameterBag->controlValues[15]);
	aShader->uniform("iExposure", mParameterBag->controlValues[14]);
	aShader->uniform("iDeltaTime", mParameterBag->iDeltaTime);
	aShader->uniform("iFade", (int)mParameterBag->iFade);
	aShader->uniform("iToggle", (int)mParameterBag->controlValues[46]);
	aShader->uniform("iLight", (int)mParameterBag->iLight);
	aShader->uniform("iLightAuto", (int)mParameterBag->iLightAuto);
	aShader->uniform("iGreyScale", (int)mParameterBag->iGreyScale);
	aShader->uniform("iTransition", mParameterBag->iTransition);
	aShader->uniform("iAnim", mParameterBag->iAnim.value());
	aShader->uniform("iRepeat", (int)mParameterBag->iRepeat);
	aShader->uniform("iVignette", (int)mParameterBag->controlValues[47]);
	aShader->uniform("iInvert", (int)mParameterBag->controlValues[48]);
	aShader->uniform("iDebug", (int)mParameterBag->iDebug);
	aShader->uniform("iShowFps", (int)mParameterBag->iShowFps);
	aShader->uniform("iFps", mParameterBag->iFps);
	aShader->uniform("iTempoTime", mParameterBag->iTempoTime);
	aShader->uniform("iGlitch", (int)mParameterBag->controlValues[45]);
	aShader->uniform("iBeat", mParameterBag->iBeat);
	aShader->uniform("iSeed", mParameterBag->iSeed);
	aShader->uniform("iFlipH", mFbos[mParameterBag->mRightFboIndex].isFlipH);
	aShader->uniform("iFlipV", mFbos[mParameterBag->mRightFboIndex].isFlipV);

	for (size_t m = 0; m < mParameterBag->MAX; m++)
	{
		getTexture(m).bind(m);
	}
	gl::drawSolidRect(Rectf(0, 0, mParameterBag->mFboWidth, mParameterBag->mFboHeight));
	// stop drawing into the FBO
	mFbos[mParameterBag->mRightFboIndex].fbo.unbindFramebuffer();

	for (size_t m = 0; m < mParameterBag->MAX; m++)
	{
		getTexture(m).unbind();
	}

	aShader->unbind();
	sTextures[7] = mFbos[mParameterBag->mRightFboIndex].fbo.getTexture();
	/*
	* end of mLibraryFbos[mParameterBag->mRightFboLibraryIndex]
	***********************************************/
#pragma endregion right
#pragma region warp
	if (mParameterBag->mMode == mParameterBag->MODE_WARP)
	{
		/***********************************************
		* start of mFbos[mParameterBag->mWarp1FboIndex]
		*/

		mFbos[mParameterBag->mWarp1FboIndex].fbo.bindFramebuffer();
		gl::setViewport(mFbos[mParameterBag->mWarp1FboIndex].fbo.getBounds());

		// clear the FBO
		gl::clear(Color(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
		gl::setMatricesWindow(mParameterBag->mFboWidth, mParameterBag->mFboHeight);

		aShader = mShaders->getShader(mParameterBag->mWarp1FragIndex).shader;
		aShader->bind();
		aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
		aShader->uniform("iResolution", Vec3f(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 1.0));
		aShader->uniform("iChannelResolution", mParameterBag->iChannelResolution, 4);
		aShader->uniform("iMouse", Vec4f(mParameterBag->mRenderPosXY.x, mParameterBag->mRenderPosXY.y, mParameterBag->iMouse.z, mParameterBag->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
		aShader->uniform("iZoom", mParameterBag->iZoomRight);
		aShader->uniform("iChannel0", mParameterBag->iChannels[0]);
		aShader->uniform("iChannel1", mParameterBag->iChannels[1]);
		aShader->uniform("iChannel2", mParameterBag->iChannels[2]);
		aShader->uniform("iChannel3", mParameterBag->iChannels[3]);
		aShader->uniform("iChannel4", mParameterBag->iChannels[4]);
		aShader->uniform("iChannel5", mParameterBag->iChannels[5]);
		aShader->uniform("iChannel6", mParameterBag->iChannels[6]);
		aShader->uniform("iChannel7", mParameterBag->iChannels[7]);
		aShader->uniform("iAudio0", 0);
		aShader->uniform("iFreq0", mParameterBag->iFreqs[0]);
		aShader->uniform("iFreq1", mParameterBag->iFreqs[1]);
		aShader->uniform("iFreq2", mParameterBag->iFreqs[2]);
		aShader->uniform("iFreq3", mParameterBag->iFreqs[3]);
		aShader->uniform("iChannelTime", mParameterBag->iChannelTime, 4);
		aShader->uniform("iColor", Vec3f(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3]));// mParameterBag->iColor);
		aShader->uniform("iBackgroundColor", Vec3f(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));// mParameterBag->iBackgroundColor);
		aShader->uniform("iSteps", (int)mParameterBag->controlValues[20]);
		aShader->uniform("iRatio", mParameterBag->controlValues[11]);//check if needed: +1;//mParameterBag->iRatio); 
		aShader->uniform("width", 1);
		aShader->uniform("height", 1);
		aShader->uniform("iRenderXY", mParameterBag->mWarp1RenderXY);
		aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
		aShader->uniform("iBlendmode", mParameterBag->iBlendMode);
		aShader->uniform("iRotationSpeed", mParameterBag->controlValues[19]);
		aShader->uniform("iCrossfade", mParameterBag->controlValues[21]);
		aShader->uniform("iPixelate", mParameterBag->controlValues[15]);
		aShader->uniform("iExposure", mParameterBag->controlValues[14]);
		aShader->uniform("iDeltaTime", mParameterBag->iDeltaTime);
		aShader->uniform("iFade", (int)mParameterBag->iFade);
		aShader->uniform("iToggle", (int)mParameterBag->controlValues[46]);
		aShader->uniform("iLight", (int)mParameterBag->iLight);
		aShader->uniform("iLightAuto", (int)mParameterBag->iLightAuto);
		aShader->uniform("iGreyScale", (int)mParameterBag->iGreyScale);
		aShader->uniform("iTransition", mParameterBag->iTransition);
		aShader->uniform("iAnim", mParameterBag->iAnim.value());
		aShader->uniform("iRepeat", (int)mParameterBag->iRepeat);
		aShader->uniform("iVignette", (int)mParameterBag->controlValues[47]);
		aShader->uniform("iInvert", (int)mParameterBag->controlValues[48]);
		aShader->uniform("iDebug", (int)mParameterBag->iDebug);
		aShader->uniform("iShowFps", (int)mParameterBag->iShowFps);
		aShader->uniform("iFps", mParameterBag->iFps);
		aShader->uniform("iTempoTime", mParameterBag->iTempoTime);
		aShader->uniform("iGlitch", (int)mParameterBag->controlValues[45]);
		aShader->uniform("iBeat", mParameterBag->iBeat);
		aShader->uniform("iSeed", mParameterBag->iSeed);
		aShader->uniform("iFlipH", mFbos[mParameterBag->mWarp1FboIndex].isFlipH);
		aShader->uniform("iFlipV", mFbos[mParameterBag->mWarp1FboIndex].isFlipV);

		for (size_t m = 0; m < mParameterBag->MAX; m++)
		{
			getTexture(m).bind(m);
		}
		gl::drawSolidRect(Rectf(0, 0, mParameterBag->mFboWidth, mParameterBag->mFboHeight));
		// stop drawing into the FBO
		mFbos[mParameterBag->mWarp1FboIndex].fbo.unbindFramebuffer();

		for (size_t m = 0; m < mParameterBag->MAX; m++)
		{
			getTexture(m).unbind();
		}

		aShader->unbind();
		sTextures[8] = mFbos[mParameterBag->mWarp1FboIndex].fbo.getTexture();

		/*
		* end of mFbos[mParameterBag->mWarp1FboIndex]
		***********************************************/

		/***********************************************
		* start of mFbos[mParameterBag->mWarp2FboIndex]
		*/

		mFbos[mParameterBag->mWarp2FboIndex].fbo.bindFramebuffer();
		gl::setViewport(mFbos[mParameterBag->mWarp2FboIndex].fbo.getBounds());

		// clear the FBO
		gl::clear(Color(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
		gl::setMatricesWindow(mParameterBag->mFboWidth, mParameterBag->mFboHeight);

		aShader = mShaders->getShader(mParameterBag->mWarp2FragIndex).shader;
		aShader->bind();
		aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
		aShader->uniform("iResolution", Vec3f(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 1.0));
		aShader->uniform("iChannelResolution", mParameterBag->iChannelResolution, 4);
		aShader->uniform("iMouse", Vec4f(mParameterBag->mRenderPosXY.x, mParameterBag->mRenderPosXY.y, mParameterBag->iMouse.z, mParameterBag->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
		aShader->uniform("iZoom", mParameterBag->iZoomRight);
		aShader->uniform("iChannel0", mParameterBag->iChannels[0]);
		aShader->uniform("iChannel1", mParameterBag->iChannels[1]);
		aShader->uniform("iChannel2", mParameterBag->iChannels[2]);
		aShader->uniform("iChannel3", mParameterBag->iChannels[3]);
		aShader->uniform("iChannel4", mParameterBag->iChannels[4]);
		aShader->uniform("iChannel5", mParameterBag->iChannels[5]);
		aShader->uniform("iChannel6", mParameterBag->iChannels[6]);
		aShader->uniform("iChannel7", mParameterBag->iChannels[7]);
		aShader->uniform("iAudio0", 0);
		aShader->uniform("iFreq0", mParameterBag->iFreqs[0]);
		aShader->uniform("iFreq1", mParameterBag->iFreqs[1]);
		aShader->uniform("iFreq2", mParameterBag->iFreqs[2]);
		aShader->uniform("iFreq3", mParameterBag->iFreqs[3]);
		aShader->uniform("iChannelTime", mParameterBag->iChannelTime, 4);
		aShader->uniform("iColor", Vec3f(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3]));// mParameterBag->iColor);
		aShader->uniform("iBackgroundColor", Vec3f(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));// mParameterBag->iBackgroundColor);
		aShader->uniform("iSteps", (int)mParameterBag->controlValues[20]);
		aShader->uniform("iRatio", mParameterBag->controlValues[11]);//check if needed: +1;//mParameterBag->iRatio); 
		aShader->uniform("width", 1);
		aShader->uniform("height", 1);
		aShader->uniform("iRenderXY", mParameterBag->mWarp2RenderXY);
		aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
		aShader->uniform("iBlendmode", mParameterBag->iBlendMode);
		aShader->uniform("iRotationSpeed", mParameterBag->controlValues[19]);
		aShader->uniform("iCrossfade", mParameterBag->controlValues[21]);
		aShader->uniform("iPixelate", mParameterBag->controlValues[15]);
		aShader->uniform("iExposure", mParameterBag->controlValues[14]);
		aShader->uniform("iDeltaTime", mParameterBag->iDeltaTime);
		aShader->uniform("iFade", (int)mParameterBag->iFade);
		aShader->uniform("iToggle", (int)mParameterBag->controlValues[46]);
		aShader->uniform("iLight", (int)mParameterBag->iLight);
		aShader->uniform("iLightAuto", (int)mParameterBag->iLightAuto);
		aShader->uniform("iGreyScale", (int)mParameterBag->iGreyScale);
		aShader->uniform("iTransition", mParameterBag->iTransition);
		aShader->uniform("iAnim", mParameterBag->iAnim.value());
		aShader->uniform("iRepeat", (int)mParameterBag->iRepeat);
		aShader->uniform("iVignette", (int)mParameterBag->controlValues[47]);
		aShader->uniform("iInvert", (int)mParameterBag->controlValues[48]);
		aShader->uniform("iDebug", (int)mParameterBag->iDebug);
		aShader->uniform("iShowFps", (int)mParameterBag->iShowFps);
		aShader->uniform("iFps", mParameterBag->iFps);
		aShader->uniform("iTempoTime", mParameterBag->iTempoTime);
		aShader->uniform("iGlitch", (int)mParameterBag->controlValues[45]);
		aShader->uniform("iBeat", mParameterBag->iBeat);
		aShader->uniform("iSeed", mParameterBag->iSeed);
		aShader->uniform("iFlipH", mFbos[mParameterBag->mWarp2FboIndex].isFlipH);
		aShader->uniform("iFlipV", mFbos[mParameterBag->mWarp2FboIndex].isFlipV);

		for (size_t m = 0; m < mParameterBag->MAX; m++)
		{
			getTexture(m).bind(m);
		}
		gl::drawSolidRect(Rectf(0, 0, mParameterBag->mFboWidth, mParameterBag->mFboHeight));
		// stop drawing into the FBO
		mFbos[mParameterBag->mWarp2FboIndex].fbo.unbindFramebuffer();

		for (size_t m = 0; m < mParameterBag->MAX; m++)
		{
			getTexture(m).unbind();
		}

		aShader->unbind();
		sTextures[9] = mFbos[mParameterBag->mWarp2FboIndex].fbo.getTexture();

		/*
		* end of mFbos[mParameterBag->mWarp2FboIndex]
		***********************************************/
	}
#pragma endregion warp
#pragma region preview
	if (mParameterBag->mPreviewEnabled)
	{
		/***********************************************
		* start of mLibraryFbos[mParameterBag->mCurrentPreviewFboIndex]
		*/
		mFbos[mParameterBag->mCurrentPreviewFboIndex].fbo.bindFramebuffer();
		gl::setViewport(mFbos[mParameterBag->mCurrentPreviewFboIndex].fbo.getBounds());

		// clear the FBO
		gl::clear(Color(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
		gl::setMatricesWindow(mParameterBag->mFboWidth, mParameterBag->mFboHeight);

		aShader = mShaders->getShader(mParameterBag->mPreviewFragIndex).shader;
		aShader->bind();
		aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
		aShader->uniform("iResolution", Vec3f(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 1.0));
		//aShader->uniform("iResolution", Vec3f(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 1.0));
		aShader->uniform("iChannelResolution", mParameterBag->iChannelResolution, 4);
		aShader->uniform("iMouse", Vec4f(mParameterBag->mRenderPosXY.x, mParameterBag->mRenderPosXY.y, mParameterBag->iMouse.z, mParameterBag->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
		aShader->uniform("iZoom", mParameterBag->controlValues[22]);
		aShader->uniform("iChannel0", mParameterBag->iChannels[0]);
		aShader->uniform("iChannel1", mParameterBag->iChannels[1]);
		aShader->uniform("iChannel2", mParameterBag->iChannels[2]);
		aShader->uniform("iChannel3", mParameterBag->iChannels[3]);
		aShader->uniform("iChannel4", mParameterBag->iChannels[4]);
		aShader->uniform("iChannel5", mParameterBag->iChannels[5]);
		aShader->uniform("iChannel6", mParameterBag->iChannels[6]);
		aShader->uniform("iChannel7", mParameterBag->iChannels[7]);
		aShader->uniform("iAudio0", 0);
		aShader->uniform("iFreq0", mParameterBag->iFreqs[0]);
		aShader->uniform("iFreq1", mParameterBag->iFreqs[1]);
		aShader->uniform("iFreq2", mParameterBag->iFreqs[2]);
		aShader->uniform("iFreq3", mParameterBag->iFreqs[3]);
		aShader->uniform("iChannelTime", mParameterBag->iChannelTime, 4);
		aShader->uniform("iColor", Vec3f(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3]));// mParameterBag->iColor);
		aShader->uniform("iBackgroundColor", Vec3f(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));// mParameterBag->iBackgroundColor);
		aShader->uniform("iSteps", (int)mParameterBag->controlValues[20]);
		aShader->uniform("iRatio", mParameterBag->controlValues[11]);
		aShader->uniform("width", 1);
		aShader->uniform("height", 1);
		aShader->uniform("iRenderXY", mParameterBag->mPreviewFragXY);
		aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
		aShader->uniform("iBlendmode", mParameterBag->iBlendMode);
		aShader->uniform("iRotationSpeed", mParameterBag->controlValues[19]);
		aShader->uniform("iCrossfade", mParameterBag->controlValues[21]);
		aShader->uniform("iPixelate", mParameterBag->controlValues[15]);
		aShader->uniform("iExposure", mParameterBag->controlValues[14]);
		aShader->uniform("iDeltaTime", mParameterBag->iDeltaTime);
		aShader->uniform("iFade", (int)mParameterBag->iFade);
		aShader->uniform("iToggle", (int)mParameterBag->controlValues[46]);
		aShader->uniform("iLight", (int)mParameterBag->iLight);
		aShader->uniform("iLightAuto", (int)mParameterBag->iLightAuto);
		aShader->uniform("iGreyScale", (int)mParameterBag->iGreyScale);
		aShader->uniform("iTransition", mParameterBag->iTransition);
		aShader->uniform("iAnim", mParameterBag->iAnim.value());
		aShader->uniform("iRepeat", (int)mParameterBag->iRepeat);
		aShader->uniform("iVignette", (int)mParameterBag->controlValues[47]);
		aShader->uniform("iInvert", (int)mParameterBag->controlValues[48]);
		aShader->uniform("iDebug", (int)mParameterBag->iDebug);
		aShader->uniform("iShowFps", (int)mParameterBag->iShowFps);
		aShader->uniform("iFps", mParameterBag->iFps);
		aShader->uniform("iTempoTime", mParameterBag->iTempoTime);
		aShader->uniform("iGlitch", (int)mParameterBag->controlValues[45]);
		aShader->uniform("iBeat", mParameterBag->iBeat);
		aShader->uniform("iSeed", mParameterBag->iSeed);
		aShader->uniform("iFlipH", mFbos[mParameterBag->mCurrentPreviewFboIndex].isFlipH);
		aShader->uniform("iFlipV", mFbos[mParameterBag->mCurrentPreviewFboIndex].isFlipV);

		for (size_t m = 0; m < mParameterBag->MAX; m++)
		{
			getTexture(m).bind(m);
		}
		gl::drawSolidRect(Rectf(0, 0, mParameterBag->mFboWidth, mParameterBag->mFboHeight));
		// stop drawing into the FBO
		mFbos[mParameterBag->mCurrentPreviewFboIndex].fbo.unbindFramebuffer();

		for (size_t m = 0; m < mParameterBag->MAX; m++)
		{
			getTexture(m).unbind();
		}

		aShader->unbind();

		sTextures[4] = mFbos[mParameterBag->mCurrentPreviewFboIndex].fbo.getTexture();
		/*
		* end of mLibraryFbos[mParameterBag->mCurrentPreviewFboIndex]
		***********************************************/
	}

#pragma endregion preview
#pragma region livecode
	if (mParameterBag->mLiveCode)
	{
		/***********************************************
		* start of mLibraryFbos[mParameterBag->mLiveFboIndex]
		*/
		mFbos[mParameterBag->mLiveFboIndex].fbo.bindFramebuffer();
		gl::setViewport(mFbos[mParameterBag->mLiveFboIndex].fbo.getBounds());

		// clear the FBO
		gl::clear(Color(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
		gl::setMatricesWindow(mParameterBag->mFboWidth, mParameterBag->mFboHeight);

		aShader = mShaders->getLiveShader();
		aShader->bind();
		aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
		aShader->uniform("iResolution", Vec3f(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 1.0));
		aShader->uniform("iChannelResolution", mParameterBag->iChannelResolution, 4);
		aShader->uniform("iMouse", Vec4f(mParameterBag->mRenderPosXY.x, mParameterBag->mRenderPosXY.y, mParameterBag->iMouse.z, mParameterBag->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
		aShader->uniform("iZoom", mParameterBag->controlValues[22]);
		aShader->uniform("iChannel0", mParameterBag->iChannels[0]);
		aShader->uniform("iChannel1", mParameterBag->iChannels[1]);
		aShader->uniform("iChannel2", mParameterBag->iChannels[2]);
		aShader->uniform("iChannel3", mParameterBag->iChannels[3]);
		aShader->uniform("iChannel4", mParameterBag->iChannels[4]);
		aShader->uniform("iChannel5", mParameterBag->iChannels[5]);
		aShader->uniform("iChannel6", mParameterBag->iChannels[6]);
		aShader->uniform("iChannel7", mParameterBag->iChannels[7]);
		aShader->uniform("iAudio0", 0);
		aShader->uniform("iFreq0", mParameterBag->iFreqs[0]);
		aShader->uniform("iFreq1", mParameterBag->iFreqs[1]);
		aShader->uniform("iFreq2", mParameterBag->iFreqs[2]);
		aShader->uniform("iFreq3", mParameterBag->iFreqs[3]);
		aShader->uniform("iChannelTime", mParameterBag->iChannelTime, 4);
		aShader->uniform("iColor", Vec3f(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3]));// mParameterBag->iColor);
		aShader->uniform("iBackgroundColor", Vec3f(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));// mParameterBag->iBackgroundColor);
		aShader->uniform("iSteps", (int)mParameterBag->controlValues[20]);
		aShader->uniform("iRatio", mParameterBag->controlValues[11]);
		aShader->uniform("width", 1);
		aShader->uniform("height", 1);
		aShader->uniform("iRenderXY", mParameterBag->mPreviewFragXY);
		aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
		aShader->uniform("iBlendmode", mParameterBag->iBlendMode);
		aShader->uniform("iRotationSpeed", mParameterBag->controlValues[19]);
		aShader->uniform("iCrossfade", mParameterBag->controlValues[21]);
		aShader->uniform("iPixelate", mParameterBag->controlValues[15]);
		aShader->uniform("iExposure", mParameterBag->controlValues[14]);
		aShader->uniform("iDeltaTime", mParameterBag->iDeltaTime);
		aShader->uniform("iFade", (int)mParameterBag->iFade);
		aShader->uniform("iToggle", (int)mParameterBag->controlValues[46]);
		aShader->uniform("iLight", (int)mParameterBag->iLight);
		aShader->uniform("iLightAuto", (int)mParameterBag->iLightAuto);
		aShader->uniform("iGreyScale", (int)mParameterBag->iGreyScale);
		aShader->uniform("iTransition", mParameterBag->iTransition);
		aShader->uniform("iAnim", mParameterBag->iAnim.value());
		aShader->uniform("iRepeat", (int)mParameterBag->iRepeat);
		aShader->uniform("iVignette", (int)mParameterBag->controlValues[47]);
		aShader->uniform("iInvert", (int)mParameterBag->controlValues[48]);
		aShader->uniform("iDebug", (int)mParameterBag->iDebug);
		aShader->uniform("iShowFps", (int)mParameterBag->iShowFps);
		aShader->uniform("iFps", mParameterBag->iFps);
		aShader->uniform("iTempoTime", mParameterBag->iTempoTime);
		aShader->uniform("iGlitch", (int)mParameterBag->controlValues[45]);
		aShader->uniform("iBeat", mParameterBag->iBeat);
		aShader->uniform("iSeed", mParameterBag->iSeed);
		aShader->uniform("iFlipH", mFbos[mParameterBag->mLiveFboIndex].isFlipH);
		aShader->uniform("iFlipV", mFbos[mParameterBag->mLiveFboIndex].isFlipV);

		for (size_t m = 0; m < mParameterBag->MAX; m++)
		{
			getTexture(m).bind(m);
		}
		gl::drawSolidRect(Rectf(0, 0, mParameterBag->mFboWidth, mParameterBag->mFboHeight));
		// stop drawing into the FBO
		mFbos[mParameterBag->mLiveFboIndex].fbo.unbindFramebuffer();

		for (size_t m = 0; m < mParameterBag->MAX; m++)
		{
			getTexture(m).unbind();
		}

		aShader->unbind();

		sTextures[11] = mFbos[mParameterBag->mLiveFboIndex].fbo.getTexture();
		/*
		* end of mFbos[mParameterBag->mLiveFboIndex]
		***********************************************/
	}

#pragma endregion livecode
#pragma region mix
	/***********************************************
	* mix 2 FBOs begin
	* first render the 2 frags to fbos (done before)
	* then use them as textures for the mix shader
	*/

	// draw using the mix shader
	mFbos[mParameterBag->mMixFboIndex].fbo.bindFramebuffer();

	gl::setViewport(mFbos[mParameterBag->mMixFboIndex].fbo.getBounds());

	// clear the FBO
	gl::clear();
	gl::setMatricesWindow(mParameterBag->mFboWidth, mParameterBag->mFboHeight);

	aShader = mShaders->getMixShader();
	aShader->bind();
	aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
	//20140703 aShader->uniform("iResolution", Vec3f(mParameterBag->mRenderResoXY.x, mParameterBag->mRenderResoXY.y, 1.0));
	aShader->uniform("iResolution", Vec3f(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 1.0));
	aShader->uniform("iChannelResolution", mParameterBag->iChannelResolution, 4);
	aShader->uniform("iMouse", Vec4f(mParameterBag->mRenderPosXY.x, mParameterBag->mRenderPosXY.y, mParameterBag->iMouse.z, mParameterBag->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	aShader->uniform("iChannel0", 0);
	aShader->uniform("iChannel1", 1);
	aShader->uniform("iAudio0", 0);
	aShader->uniform("iFreq0", mParameterBag->iFreqs[0]);
	aShader->uniform("iFreq1", mParameterBag->iFreqs[1]);
	aShader->uniform("iFreq2", mParameterBag->iFreqs[2]);
	aShader->uniform("iFreq3", mParameterBag->iFreqs[3]);
	aShader->uniform("iChannelTime", mParameterBag->iChannelTime, 4);
	aShader->uniform("iColor", Vec3f(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3]));// mParameterBag->iColor);
	aShader->uniform("iBackgroundColor", Vec3f(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));// mParameterBag->iBackgroundColor);
	aShader->uniform("iSteps", (int)mParameterBag->controlValues[20]);
	aShader->uniform("iRatio", mParameterBag->controlValues[11]);//check if needed: +1;//mParameterBag->iRatio); 
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mParameterBag->mRenderXY);
	aShader->uniform("iZoom", mParameterBag->controlValues[22]);
	aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
	aShader->uniform("iBlendmode", mParameterBag->iBlendMode);
	aShader->uniform("iChromatic", mParameterBag->controlValues[10]);
	aShader->uniform("iRotationSpeed", mParameterBag->controlValues[19]);
	aShader->uniform("iCrossfade", mParameterBag->controlValues[18]);
	aShader->uniform("iPixelate", mParameterBag->controlValues[15]);
	aShader->uniform("iExposure", mParameterBag->controlValues[14]);
	aShader->uniform("iDeltaTime", mParameterBag->iDeltaTime);
	aShader->uniform("iFade", (int)mParameterBag->iFade);
	aShader->uniform("iToggle", (int)mParameterBag->controlValues[46]);
	aShader->uniform("iLight", (int)mParameterBag->iLight);
	aShader->uniform("iLightAuto", (int)mParameterBag->iLightAuto);
	aShader->uniform("iGreyScale", (int)mParameterBag->iGreyScale);
	aShader->uniform("iTransition", mParameterBag->iTransition);
	aShader->uniform("iAnim", mParameterBag->iAnim.value());
	aShader->uniform("iRepeat", (int)mParameterBag->iRepeat);
	aShader->uniform("iVignette", (int)mParameterBag->controlValues[47]);
	aShader->uniform("iInvert", (int)mParameterBag->controlValues[48]);
	aShader->uniform("iDebug", (int)mParameterBag->iDebug);
	aShader->uniform("iShowFps", (int)mParameterBag->iShowFps);
	aShader->uniform("iFps", mParameterBag->iFps);
	aShader->uniform("iTempoTime", mParameterBag->iTempoTime);
	aShader->uniform("iGlitch", (int)mParameterBag->controlValues[45]);
	aShader->uniform("iTrixels", mParameterBag->controlValues[16]);
	aShader->uniform("iGridSize", mParameterBag->controlValues[17]);
	aShader->uniform("iBeat", mParameterBag->iBeat);
	aShader->uniform("iSeed", mParameterBag->iSeed);
	aShader->uniform("iRedMultiplier", mParameterBag->iRedMultiplier);
	aShader->uniform("iGreenMultiplier", mParameterBag->iGreenMultiplier);
	aShader->uniform("iBlueMultiplier", mParameterBag->iBlueMultiplier);
	aShader->uniform("iFlipH", mFbos[mParameterBag->mMixFboIndex].isFlipH);
	aShader->uniform("iFlipV", mFbos[mParameterBag->mMixFboIndex].isFlipV);
	aShader->uniform("iParam1", mParameterBag->iParam1);
	aShader->uniform("iParam2", mParameterBag->iParam2);
	aShader->uniform("iXorY", mParameterBag->iXorY);
	aShader->uniform("iBadTv", mParameterBag->iBadTv);

	sTextures[6].bind(0);
	sTextures[7].bind(1);
	gl::drawSolidRect(Rectf(0, 0, mParameterBag->mFboWidth, mParameterBag->mFboHeight));
	// stop drawing into the FBO
	mFbos[mParameterBag->mMixFboIndex].fbo.unbindFramebuffer();
	sTextures[6].unbind();
	sTextures[7].unbind();

	aShader->unbind();
	sTextures[5] = mFbos[mParameterBag->mMixFboIndex].fbo.getTexture();

	//}
	/***********************************************
	* mix 2 FBOs end
	*/
#pragma endregion mix

	auto end = Clock::now();
	auto msdur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	//std::cout << msdur.count() << "ms, " << nsdur.count() << "µs" << std::endl;
	sprintf_s(previewTime, "%2d", msdur.count());

}


Textures::~Textures()
{
	log->logTimedString("Textures destructor");
	sTextures.clear();
	//sequenceTextures.clear();
}

// sequence

//Begins playback of sequence
void Textures::playSequence(int textureIndex)
{
	sequences[textas[textureIndex].sequenceIndex].playing = true;
}
// Pauses playback
void Textures::pauseSequence(int textureIndex)
{

	sequences[textas[textureIndex].sequenceIndex].playing = false;
}

// Stops playback and resets the playhead to zero
void Textures::stopSequence(int textureIndex)
{
	sequences[textas[textureIndex].sequenceIndex].playing = false;
	sequences[textas[textureIndex].sequenceIndex].playheadPosition = 0;
}

int Textures::getMaxFrames(int textureIndex) {

	return sequences[textas[textureIndex].sequenceIndex].framesLoaded;
}
int Textures::getPlayheadPosition(int textureIndex) {

	return sequences[textas[textureIndex].sequenceIndex].playheadPosition;
}
// Seek to a new position in the sequence
void Textures::setPlayheadPosition(int textureIndex, int position) {

	sequences[textas[textureIndex].sequenceIndex].playheadPosition = max(0, min(position, (int)sequences[textas[textureIndex].sequenceIndex].sequenceTextures.size() - 1));
}

void Textures::reverseSequence(int textureIndex) {

	sequences[textas[textureIndex].sequenceIndex].speed *= -1;

}
float Textures::getSpeed(int textureIndex) {

	return sequences[textas[textureIndex].sequenceIndex].speed;
}
void Textures::setSpeed(int textureIndex, float speed) {

	sequences[textas[textureIndex].sequenceIndex].speed = speed;
}
bool Textures::isLoadingFromDisk(int textureIndex) {
	return sequences[textas[textureIndex].sequenceIndex].loadingFilesComplete;
}

void Textures::toggleLoadingFromDisk(int textureIndex) {

	sequences[textas[textureIndex].sequenceIndex].loadingPaused = !sequences[textas[textureIndex].sequenceIndex].loadingPaused;
}
/**
*  -- Loads all files contained in the supplied folder and creates Textures from them
*/
void Textures::createFromDir(string filePath, int index)
{
	fs::path p(filePath);
	sequence seq;
	seq.filePath = filePath;
	seq.index = index;
	seq.loadingFilesComplete = false;
	seq.loadingPaused = false;
	seq.framesLoaded = 0;
	seq.currentLoadedFrame = 0;
	seq.nextIndexFrameToTry = 0;
	seq.playing = false;
	seq.speed = 1;
	seq.numberOfDigits = 4;
	sprintf_s(seq.folder, "");
	// find the folder name for display in the ui
	if (filePath.find_last_of("\\") != std::string::npos) {
		int slashIndex = filePath.find_last_of("\\") + 1;
		string folder = filePath.substr(slashIndex);
		sprintf_s(seq.folder, "%s", folder.c_str());
	}
	bool noValidFile = true; // if no valid files in the folder, we keep existing vector
	bool firstIndexFound = false;
	int i = 0;
	// loading 2000 files takes a while, I load only 2
	for (fs::directory_iterator it(p); it != fs::directory_iterator(); ++it)
	{
		if (fs::is_regular_file(*it))
		{
			string fileName = it->path().filename().string();
			if (fileName.find_last_of(".") != std::string::npos) {
				int dotIndex = fileName.find_last_of(".");
				seq.ext = fileName.substr(dotIndex + 1);
				// get the prefix for the image sequence
				// the files are named from p0000.jpg to p2253.jpg for instance
				// sometimes only 3 digits : l000 this time
				// find the first digit
				int firstDigit = fileName.find_first_of("0123456789");
				// if valid image sequence (contains a digit)
				if (firstDigit > -1) {
					seq.numberOfDigits = dotIndex - firstDigit;
					int prefixIndex = fileName.find_last_of(".") - seq.numberOfDigits;//-4 or -3
					seq.prefix = fileName.substr(0, prefixIndex);
					if (!firstIndexFound) {
						firstIndexFound = true;
						seq.nextIndexFrameToTry = std::stoi(fileName.substr(prefixIndex, dotIndex));
					}

				}
			}
			// only if proper image sequence
			if (firstIndexFound) {
				if (seq.ext == "png" || seq.ext == "jpg")
				{
					if (noValidFile)
					{
						// we found a valid file
						noValidFile = false;
						seq.sequenceTextures.clear();
						// reset playhead to the start
						seq.playheadPosition = 0;
						sequences.push_back(seq);
						textas[seq.index].sequenceIndex = sequences.size() - 1;
						sprintf_s(textas[seq.index].name, "%s", seq.folder);
						textas[seq.index].isSequence = true;
						loadNextImageFromDisk(sequences.size() - 1);
						seq.playing = true;
					}
				}
			}
		}
	}

}
bool Textures::isSequence(int textureIndex) {
	return textas[textureIndex].isSequence;
}

ci::gl::Texture Textures::getCurrentSequenceTexture(int sequenceIndex) {
	if (sequenceIndex > sequences.size()) {
		sequenceIndex = 0;
	}
	if (sequences[sequenceIndex].playheadPosition > sequences[sequenceIndex].framesLoaded) {
		//error
	}
	return sequences[sequenceIndex].sequenceTextures[sequences[sequenceIndex].playheadPosition];
}
void Textures::stopLoading() {
	for (int i = 0; i < sequences.size(); i++)
	{
		sequences[i].loadingPaused = true;
	}
}
void Textures::loadNextImageFromDisk(int currentSeq) {
	if (!sequences[currentSeq].loadingPaused) {

		if (!sequences[currentSeq].loadingFilesComplete) {
			// thank you Omar!
			char restOfFileName[32];
			if (sequences[currentSeq].numberOfDigits == 4) {
				sprintf(restOfFileName, "%04d", sequences[currentSeq].nextIndexFrameToTry);

			}
			else {
				sprintf(restOfFileName, "%03d", sequences[currentSeq].nextIndexFrameToTry);

			}

			fs::path fileToLoad = sequences[currentSeq].filePath + sequences[currentSeq].prefix + restOfFileName + "." + sequences[currentSeq].ext;
			if (fs::exists(fileToLoad)) {
				sequences[currentSeq].sequenceTextures.push_back(ci::gl::Texture(loadImage(fileToLoad)));
				sequences[currentSeq].currentLoadedFrame = sequences[currentSeq].framesLoaded;
				sequences[currentSeq].framesLoaded++;
				mParameterBag->mMsg = fileToLoad.string() + " loaded";
			}
			else {
				mParameterBag->mMsg = fileToLoad.string() + " does not exist";
				if (sequences[currentSeq].framesLoaded > 0) {
					// if frames have been loaded we hit the last file of the image sequence at this point
					mParameterBag->mMsg = "last image loaded";
					sequences[currentSeq].loadingFilesComplete = true;
				}
			}
			mParameterBag->newMsg = true;
			// increment counter for next filename
			sequences[currentSeq].nextIndexFrameToTry++;
			if (sequences[currentSeq].nextIndexFrameToTry > 9999) sequences[currentSeq].loadingFilesComplete = true;

		}
	}
}


// Loads all of the images in the supplied list of file paths
void Textures::createFromPathList(vector<string> paths)
{
	/*sequenceTextures.clear();
	for (int i = 0; i < paths.size(); ++i)
	{
	sequenceTextures.push_back(ci::gl::Texture(loadImage(paths[i])));
	}
	totalFrames = seq.sequenceTextures.size();*/
}

void Textures::createFromTextureList(vector<ci::gl::Texture> textureList)
{
	/*sequenceTextures.clear();
	sequenceTextures = textureList;
	totalFrames = sequenceTextures.size();*/
}

//timeline().apply( &mAlpha, 1.0f, 2.0f ).finishFn( [&]{ textureSequence.update(); mAlpha= 1.0f; } );
void Textures::updateSequence(int sequenceIndex)
{
	if (sequences[sequenceIndex].sequenceTextures.size() > 0) {
		// Call on each frame to update the playhead
		if (sequences[sequenceIndex].playing)
		{
			int newPosition = sequences[sequenceIndex].playheadPosition + (playheadFrameInc * sequences[sequenceIndex].speed);
			if (newPosition < 0) newPosition = sequences[sequenceIndex].sequenceTextures.size() - 1;
			if (newPosition > sequences[sequenceIndex].sequenceTextures.size() - 1) newPosition = 0;
			sequences[sequenceIndex].playheadPosition = max(0, min(newPosition, (int)sequences[sequenceIndex].sequenceTextures.size() - 1));

		}
		sTextures[sequences[sequenceIndex].index] = getCurrentSequenceTexture(sequenceIndex);
	}

}
void Textures::setSenderTextureSize(int index, int width, int height)
{
	sTextures[10] = gl::Texture(width, height);
}

int Textures::createSpoutTexture(char name[256], unsigned int width, unsigned int height)
{
	// replace spout image at index 10
	log->logTimedString("createSpoutTexture, replace: " + toString(name));

	memcpy(&spoutSenderName[0], name, strlen(name) + 1);
	sTextures[10] = gl::Texture(width, height);
	return 10;
}