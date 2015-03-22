#include "Textures.h"

using namespace Reymenta;

Textures::Textures(ParameterBagRef aParameterBag, ShadersRef aShadersRef)
{
	mParameterBag = aParameterBag;
	mShaders = aShadersRef;
	// instanciate the logger class
	log = Logger::create("TexturesLog.txt");
	log->logTimedString("Textures constructor");

	// preview fbo at index 0
	mFbos.push_back(gl::Fbo(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight));//640x480
	//mFbos[0].getTexture(0).setFlipped(true);
	//createPreviewFbo();//mFboWidth/4 or 16
	// mix fbo at index 1
	mFbos.push_back(gl::Fbo(mParameterBag->mFboWidth, mParameterBag->mFboHeight));
	mFbos[1].getTexture(0).setFlipped(true);

	for (size_t m = mFbos.size(); m < 9; m++)
	{
		mFbos.push_back(gl::Fbo(mParameterBag->mFboWidth, mParameterBag->mFboHeight));
		//mFbos[mFbos.size() - 1].getTexture(0).setFlipped(true);
	}
	for (size_t m = 0; m < mShaders->getCount(); m++)
	{
		mThumbFbos.push_back(gl::Fbo(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight));
	}
	currentShadaThumbIndex = 0;
	//mFbos[mParameterBag->mMeshFboIndex].getTexture(0).setFlipped(false);
	// audio fbo at index 3
	mFbos[mParameterBag->mAudioFboIndex] = gl::Fbo(mParameterBag->mFboWidth, mParameterBag->mFboHeight);
	mFbos[mParameterBag->mAudioFboIndex].getTexture(0).setFlipped(true);
	for (int i = 0; i < 1024; ++i) dTexture[i] = (unsigned char)(Rand::randUint() & 0xFF);
	// store it as a 512x2 texture in the first texture
	sTextures.push_back(gl::Texture(dTexture, GL_LUMINANCE, 512, 2));

	fs::path localFile;
	for (int j = 0; j < mTexturesCount - 1; j++)
	{
		fileName = toString(j) + ".jpg";
		localFile = getAssetPath("") / fileName;
		if (fs::exists(localFile))
		{
			gl::Texture img(loadImage(loadAsset(fileName)));// TODO , gl::Texture::Format().magFilter(GL_LINEAR).minFilter(GL_LINEAR).loadTopDown());
			//img.setFlipped();
			sTextures.push_back(img);
		}
		else
		{
			gl::Texture img(loadImage(loadAsset("reymenta.jpg")));
			sTextures.push_back(img);
		}
	}

}

void Textures::createWarpInput()
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
}

void Textures::setAudioTexture(unsigned char *signal)
{
	sTextures[0] = gl::Texture(signal, GL_LUMINANCE, 512, 2);
}
void Textures::setTexture(int index, string fileName)
{
	if (index > mTexturesCount - 1) index = mTexturesCount - 1;
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
}
void Textures::flipFbo(int index)
{
	mFbos[index].getTexture(0).setFlipped(!mFbos[index].getTexture(0).isFlipped());
}
void Textures::flipMixFbo(bool flip)
{
	mFbos[mParameterBag->mMixFboIndex].getTexture(0).setFlipped(flip);
}
void Textures::flipMixFboH(bool flip)
{
	mParameterBag->mFlipH = flip;
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
	return mFbos[index].getTexture();
}
GLuint Textures::getFboTextureId(int index)
{
	if (index > mFbos.size() - 1) index = mFbos.size() - 1;
	return  mFbos[index].getTexture().getId();
}

ci::gl::Fbo Textures::getFbo(int index)
{
	// fbo
	return mFbos[index];
}
GLuint Textures::getShaderThumbTextureId(int index)
{
	if (index > mThumbFbos.size() - 1) index = mThumbFbos.size() - 1;
	return  mThumbFbos[index].getTexture().getId();
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
		log->logTimedString("Error loading image:" + aFile);
	}
}


void Textures::update()
{

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
	mThumbFbos[currentShadaThumbIndex].bindFramebuffer();
	gl::setViewport(mThumbFbos[currentShadaThumbIndex].getBounds());

	// clear the FBO
	gl::clear(Color(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
	gl::setMatricesWindow(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight, mParameterBag->mOriginUpperLeft);

	aShader = mShaders->getShader(currentShadaThumbIndex);
	aShader->bind();
	aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
	//aShader->uniform("iResolution", Vec3f(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight, 1.0));
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
	aShader->uniform("iSteps", (int)mParameterBag->controlValues[16]);
	aShader->uniform("iRatio", mParameterBag->controlValues[11]);
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mParameterBag->mLeftRenderXY);
	aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
	aShader->uniform("iBlendmode", (int)mParameterBag->controlValues[20]);
	aShader->uniform("iRotationSpeed", mParameterBag->controlValues[19]);
	aShader->uniform("iCrossfade", mParameterBag->controlValues[17]);
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

	for (size_t m = 0; m < mTexturesCount; m++)
	{
		getTexture(m).bind(m);
	}
	gl::drawSolidRect(Rectf(0, 0, mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight));
	// stop drawing into the FBO
	mThumbFbos[currentShadaThumbIndex].unbindFramebuffer();

	for (size_t m = 0; m < mTexturesCount; m++)
	{
		getTexture(m).unbind();
	}

	aShader->unbind();
	// increment shada thumb index
	currentShadaThumbIndex++;
	if (currentShadaThumbIndex > mShaders->getCount()-1) currentShadaThumbIndex = 0;
}

void Textures::draw()
{
	//renderWarpFbos();
	renderShadaThumbFbo();
	/**********************************************
	* library FBOs
	*/
	// start of mLibraryFbos[mParameterBag->mLeftFboIndex]
	mFbos[mParameterBag->mLeftFboIndex].bindFramebuffer();
	gl::setViewport(mFbos[mParameterBag->mLeftFboIndex].getBounds());

	// clear the FBO
	gl::clear(Color(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
	gl::setMatricesWindow(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight, mParameterBag->mOriginUpperLeft);

	aShader = mShaders->getShader(mParameterBag->mLeftFragIndex);
	aShader->bind();
	aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
	//aShader->uniform("iResolution", Vec3f(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight, 1.0));
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
	aShader->uniform("iSteps", (int)mParameterBag->controlValues[16]);
	aShader->uniform("iRatio", mParameterBag->controlValues[11]);
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mParameterBag->mLeftRenderXY);
	aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
	aShader->uniform("iBlendmode", (int)mParameterBag->controlValues[20]);
	aShader->uniform("iRotationSpeed", mParameterBag->controlValues[19]);
	aShader->uniform("iCrossfade", mParameterBag->controlValues[17]);
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

	for (size_t m = 0; m < mTexturesCount; m++)
	{
		getTexture(m).bind(m);
	}
	gl::drawSolidRect(Rectf(0, 0, mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight));
	// stop drawing into the FBO
	mFbos[mParameterBag->mLeftFboIndex].unbindFramebuffer();

	for (size_t m = 0; m < mTexturesCount; m++)
	{
		getTexture(m).unbind();
	}

	aShader->unbind();
	sTextures[6] = mFbos[mParameterBag->mLeftFboIndex].getTexture();
	// end of mLibraryFbos[mParameterBag->mLeftFboIndex]

	// start of mLibraryFbos[mParameterBag->mRightFboIndex]
	mFbos[mParameterBag->mRightFboIndex].bindFramebuffer();
	gl::setViewport(mFbos[mParameterBag->mRightFboIndex].getBounds());

	// clear the FBO
	gl::clear(Color(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
	gl::setMatricesWindow(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight, mParameterBag->mOriginUpperLeft);

	aShader = mShaders->getShader(mParameterBag->mRightFragIndex);
	aShader->bind();
	aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
	//aShader->uniform("iResolution", Vec3f(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight, 1.0));
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
	aShader->uniform("iSteps", (int)mParameterBag->controlValues[16]);
	aShader->uniform("iRatio", mParameterBag->controlValues[11]);//check if needed: +1;//mParameterBag->iRatio); 
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mParameterBag->mRightRenderXY);
	aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
	aShader->uniform("iBlendmode", (int)mParameterBag->controlValues[20]);
	aShader->uniform("iRotationSpeed", mParameterBag->controlValues[19]);
	aShader->uniform("iCrossfade", mParameterBag->controlValues[17]);
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

	for (size_t m = 0; m < mTexturesCount; m++)
	{
		getTexture(m).bind(m);
	}
	gl::drawSolidRect(Rectf(0, 0, mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight));
	// stop drawing into the FBO
	mFbos[mParameterBag->mRightFboIndex].unbindFramebuffer();

	for (size_t m = 0; m < mTexturesCount; m++)
	{
		getTexture(m).unbind();
	}

	aShader->unbind();
	sTextures[7] = mFbos[mParameterBag->mRightFboIndex].getTexture();

	// end of mLibraryFbos[mParameterBag->mRightFboLibraryIndex]

	// start of mLibraryFbos[mParameterBag->mCurrentPreviewFboIndex]
	mFbos[mParameterBag->mCurrentPreviewFboIndex].bindFramebuffer();
	gl::setViewport(mFbos[mParameterBag->mCurrentPreviewFboIndex].getBounds());

	// clear the FBO
	gl::clear(Color(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
	gl::setMatricesWindow(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight, mParameterBag->mOriginUpperLeft);

	aShader = mShaders->getShader(mParameterBag->mPreviewFragIndex);
	aShader->bind();
	aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
	aShader->uniform("iResolution", Vec3f(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight, 1.0));
	//aShader->uniform("iResolution", Vec3f(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 1.0));
	aShader->uniform("iChannelResolution", mParameterBag->iChannelResolution, 4);
	aShader->uniform("iMouse", Vec4f(mParameterBag->mRenderPosXY.x, mParameterBag->mRenderPosXY.y, mParameterBag->iMouse.z, mParameterBag->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	aShader->uniform("iZoom", mParameterBag->controlValues[13]);
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
	aShader->uniform("iSteps", (int)mParameterBag->controlValues[16]);
	aShader->uniform("iRatio", mParameterBag->controlValues[11]);
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mParameterBag->mPreviewFragXY);
	aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
	aShader->uniform("iBlendmode", (int)mParameterBag->controlValues[20]);
	aShader->uniform("iRotationSpeed", mParameterBag->controlValues[19]);
	aShader->uniform("iCrossfade", mParameterBag->controlValues[17]);
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

	for (size_t m = 0; m < mTexturesCount; m++)
	{
		getTexture(m).bind(m);
	}
	gl::drawSolidRect(Rectf(0, 0, mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight));
	// stop drawing into the FBO
	mFbos[mParameterBag->mCurrentPreviewFboIndex].unbindFramebuffer();

	for (size_t m = 0; m < mTexturesCount; m++)
	{
		getTexture(m).unbind();
	}

	aShader->unbind();
	sTextures[4] = mFbos[mParameterBag->mCurrentPreviewFboIndex].getTexture();
	// draw thumb
	/*mThumbFbos[mParameterBag->mPreviewFragIndex].bindFramebuffer();
	gl::setViewport(mThumbFbos[mParameterBag->mPreviewFragIndex].getBounds());
	gl::draw(sTextures[4]);
	mThumbFbos[mParameterBag->mPreviewFragIndex].unbindFramebuffer();*/
	//writeImage(getHomeDirectory() / "cinder" / "saveImage_" / (toString(1) + ".png"), copyWindowSurface());
	// end of mLibraryFbos[mParameterBag->mCurrentPreviewFboIndex]
	
	
	/***********************************************
	* live FBO begin
	

	// draw using the live shader
	mFbos[mParameterBag->mLiveFboIndex].bindFramebuffer();

	gl::setViewport(mFbos[mParameterBag->mLiveFboIndex].getBounds());

	// clear the FBO
	gl::clear();
	gl::setMatricesWindow(mParameterBag->mFboWidth, mParameterBag->mFboHeight, mParameterBag->mOriginUpperLeft);

	aShader = mShaders->getLiveShader();
	aShader->bind();
	aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
	//20140703 aShader->uniform("iResolution", Vec3f(mParameterBag->mRenderResoXY.x, mParameterBag->mRenderResoXY.y, 1.0));
	aShader->uniform("iResolution", Vec3f(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 1.0));
	aShader->uniform("iChannelResolution", mParameterBag->iChannelResolution, 4);
	aShader->uniform("iMouse", Vec4f(mParameterBag->mRenderPosXY.x, mParameterBag->mRenderPosXY.y, mParameterBag->iMouse.z, mParameterBag->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	aShader->uniform("iChannel0", 0);
	aShader->uniform("iChannel1", 1);
	aShader->uniform("iChannel2", 2);
	aShader->uniform("iChannel3", 3);
	aShader->uniform("iChannel4", 4);
	aShader->uniform("iChannel5", 5);
	aShader->uniform("iChannel6", 6);
	aShader->uniform("iChannel7", 7);
	aShader->uniform("iAudio0", 0);
	aShader->uniform("iFreq0", mParameterBag->iFreqs[0]);
	aShader->uniform("iFreq1", mParameterBag->iFreqs[1]);
	aShader->uniform("iFreq2", mParameterBag->iFreqs[2]);
	aShader->uniform("iFreq3", mParameterBag->iFreqs[3]);
	aShader->uniform("iChannelTime", mParameterBag->iChannelTime, 4);
	aShader->uniform("iColor", Vec3f(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3]));// mParameterBag->iColor);
	aShader->uniform("iBackgroundColor", Vec3f(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));// mParameterBag->iBackgroundColor);
	aShader->uniform("iSteps", (int)mParameterBag->controlValues[16]);
	aShader->uniform("iRatio", mParameterBag->controlValues[11]);//check if needed: +1;//mParameterBag->iRatio); 
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mParameterBag->mRenderXY);
	aShader->uniform("iZoom", mParameterBag->controlValues[13]);
	aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
	aShader->uniform("iBlendmode", (int)mParameterBag->controlValues[20]);
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

	sTextures[6].bind(0);
	sTextures[7].bind(1);
	gl::drawSolidRect(Rectf(0, 0, mParameterBag->mFboWidth, mParameterBag->mFboHeight));
	// stop drawing into the FBO
	mFbos[mParameterBag->mLiveFboIndex].unbindFramebuffer();
	sTextures[6].unbind();
	sTextures[7].unbind();

	aShader->unbind();
	sTextures[3] = mFbos[mParameterBag->mLiveFboIndex].getTexture();

	**********************************************
	* live FBO end
	*/

	/***********************************************
	* mix 2 FBOs begin
	* first render the 2 frags to fbos (done before)
	* then use them as textures for the mix shader
	*/

	// draw using the mix shader
	mFbos[mParameterBag->mMixFboIndex].bindFramebuffer();

	gl::setViewport(mFbos[mParameterBag->mMixFboIndex].getBounds());

	// clear the FBO
	gl::clear();
	gl::setMatricesWindow(mParameterBag->mFboWidth, mParameterBag->mFboHeight, mParameterBag->mOriginUpperLeft);

	aShader = mShaders->getMixShader();
	aShader->bind();
	aShader->uniform("iGlobalTime", mParameterBag->iGlobalTime);
	//20140703 aShader->uniform("iResolution", Vec3f(mParameterBag->mRenderResoXY.x, mParameterBag->mRenderResoXY.y, 1.0));
	aShader->uniform("iResolution", Vec3f(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 1.0));
	aShader->uniform("iChannelResolution", mParameterBag->iChannelResolution, 4);
	aShader->uniform("iMouse", Vec4f(mParameterBag->mRenderPosXY.x, mParameterBag->mRenderPosXY.y, mParameterBag->iMouse.z, mParameterBag->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	aShader->uniform("iChannel0", 0);
	aShader->uniform("iChannel1", 1);
	aShader->uniform("iChannel2", 2);
	aShader->uniform("iChannel3", 3);
	aShader->uniform("iChannel4", 4);
	aShader->uniform("iChannel5", 5);
	aShader->uniform("iChannel6", 6);
	aShader->uniform("iChannel7", 7);
	aShader->uniform("iAudio0", 0);
	aShader->uniform("iFreq0", mParameterBag->iFreqs[0]);
	aShader->uniform("iFreq1", mParameterBag->iFreqs[1]);
	aShader->uniform("iFreq2", mParameterBag->iFreqs[2]);
	aShader->uniform("iFreq3", mParameterBag->iFreqs[3]);
	aShader->uniform("iChannelTime", mParameterBag->iChannelTime, 4);
	aShader->uniform("iColor", Vec3f(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3]));// mParameterBag->iColor);
	aShader->uniform("iBackgroundColor", Vec3f(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));// mParameterBag->iBackgroundColor);
	aShader->uniform("iSteps", (int)mParameterBag->controlValues[16]);
	aShader->uniform("iRatio", mParameterBag->controlValues[11]);//check if needed: +1;//mParameterBag->iRatio); 
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mParameterBag->mRenderXY);
	aShader->uniform("iZoom", mParameterBag->controlValues[13]);
	aShader->uniform("iAlpha", mParameterBag->controlValues[4]);
	aShader->uniform("iBlendmode", (int)mParameterBag->controlValues[20]);
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

	sTextures[6].bind(0);
	sTextures[7].bind(1);
	gl::drawSolidRect(Rectf(0, 0, mParameterBag->mFboWidth, mParameterBag->mFboHeight));
	// stop drawing into the FBO
	mFbos[mParameterBag->mMixFboIndex].unbindFramebuffer();
	sTextures[6].unbind();
	sTextures[7].unbind();

	aShader->unbind();
	sTextures[5] = mFbos[mParameterBag->mMixFboIndex].getTexture();

	//}
	/***********************************************
	* mix 2 FBOs end
	*/
}

/*void Textures::updateSequence()
{
// sequence
previousTexture = getCurrentSequenceTexture();
//timeline().apply( &mAlpha, 1.0f, 2.0f ).finishFn( [&]{ textureSequence.update(); mAlpha= 1.0f; } );

// Call on each frame to update the playhead

if (!paused && playing)
{
int newPosition = playheadPosition + playheadFrameInc;
if (newPosition > totalFrames - 1)
{
if (looping)
{
complete = false;
playheadPosition = newPosition - totalFrames;
}
else {
complete = true;
}

}
else if (newPosition < 0) {
if (looping)
{
complete = false;
playheadPosition = totalFrames - abs(newPosition);
}
else {
complete = true;
}

}
else {
complete = false;
playheadPosition = newPosition;
}
}
sTextures[9] = getCurrentSequenceTexture();
currentTexture = getCurrentSequenceTexture();
}*/
Textures::~Textures()
{
	log->logTimedString("Textures destructor");
	sTextures.clear();
	//sequenceTextures.clear();
}

// sequence

/*
//Begins playback of sequence
void Textures::play()
{
paused = false;
playing = true;
}

// Pauses playback
void Textures::pause()
{
paused = true;
playing = false;
}

// Stops playback and resets the playhead to zero
void Textures::stop()
{
playheadPosition = 0;
playing = false;
paused = false;
}

// Seek to a new position in the sequence
void Textures::setPlayheadPosition(int newPosition)
{
playheadPosition = max(0, min(newPosition, totalFrames));
}*/

/**
*  -- Loads all files contained in the supplied folder and creates Textures from them
*/
/*void Textures::createFromDir(string filePath)
{
bool noValidFile = true; // if no valid files in the folder, we keep existing vector
int i = 0;
string ext = "";
fs::path p(filePath);
for (fs::directory_iterator it(p); it != fs::directory_iterator(); ++it)
{
if (fs::is_regular_file(*it))
{
string fileName = it->path().filename().string();
if (fileName.find_last_of(".") != std::string::npos) ext = fileName.substr(fileName.find_last_of(".") + 1);
if (ext == "png" || ext == "jpg")
{
if (noValidFile)
{
// we found a valid file
noValidFile = false;
sequenceTextures.clear();
// reset playhead to the start
playheadPosition = 0;
}
sequenceTextures.push_back(ci::gl::Texture(loadImage(filePath + fileName)));
}
}
}

totalFrames = sequenceTextures.size();
}

// Loads all of the images in the supplied list of file paths

void Textures::createFromPathList(vector<string> paths)
{
sequenceTextures.clear();
for (int i = 0; i < paths.size(); ++i)
{
sequenceTextures.push_back(ci::gl::Texture(loadImage(paths[i])));
}
totalFrames = sequenceTextures.size();
}

void Textures::createFromTextureList(vector<ci::gl::Texture> textureList)
{
sequenceTextures.clear();
sequenceTextures = textureList;
totalFrames = sequenceTextures.size();
}*/
void Textures::setSenderTextureSize(int index, int width, int height)
{
	sTextures[8] = gl::Texture(width, height);
}

int Textures::createSpoutTexture(char name[256], unsigned int width, unsigned int height)
{
	// replace spout image at index 8
	log->logTimedString("createSpoutTexture, replace: " + toString(name));

	memcpy(&spoutSenderName[0], name, strlen(name) + 1);
	sTextures[8] = gl::Texture(width, height);
	return 8;
}