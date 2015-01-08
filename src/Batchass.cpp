#include "Batchass.h"

using namespace Reymenta;

Batchass::Batchass(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;
	// instanciate the logger class
	log = Logger::create("BatchassLog.txt");
	log->logTimedString("Batchass constructor");
}
float Batchass::formatFloat(float f)
{
	int i;
	f *= 100;
	i = ((int)f)/100;
	return (float)i;
}
void Batchass::setup()
{
	// instanciate the Shaders class, must not be in prepareSettings
	mShaders = Shaders::create(mParameterBag);

	// instanciate the textures class
	mTextures = Textures::create(mParameterBag, mShaders);

	createWarpFbos();
}
void Batchass::createWarpFbos()
{
	for (int a = 0; a < mParameterBag->MAX; a++)
	{
		WarpFbo newWarpFbo;
		newWarpFbo.textureIndex = 0;
		newWarpFbo.textureMode = 1;
		newWarpFbo.fbo = gl::Fbo(mParameterBag->mFboWidth, mParameterBag->mFboHeight);
		if (a == 0) newWarpFbo.active = true; else newWarpFbo.active = false;
		mParameterBag->mWarpFbos[a] = newWarpFbo;
	}
}
void Batchass::getWindowsResolution()
{
	mParameterBag->mDisplayCount = 0;
	if (mParameterBag->mAutoLayout)
	{
		// Display sizes
		mParameterBag->mMainDisplayWidth = Display::getMainDisplay()->getWidth();
		mParameterBag->mRenderX = mParameterBag->mMainDisplayWidth;
		mParameterBag->mRenderY = 0;
		for (auto display : Display::getDisplays())
		{
			mParameterBag->mDisplayCount++;
			mParameterBag->mRenderWidth = display->getWidth();
			mParameterBag->mRenderHeight = display->getHeight();
			log->logTimedString("Window " + toString(mParameterBag->mDisplayCount) + ": " + toString(mParameterBag->mRenderWidth) + "x" + toString(mParameterBag->mRenderHeight));
		}
		log->logTimedString(" mRenderWidth" + toString(mParameterBag->mRenderWidth) + "mRenderHeight" + toString(mParameterBag->mRenderHeight));
		mParameterBag->mRenderResoXY = Vec2f(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);
		// in case only one screen , render from x = 0
		if (mParameterBag->mDisplayCount == 1) mParameterBag->mRenderX = 0;
	}	
	else
	{
		for (auto display : Display::getDisplays())
		{
			mParameterBag->mDisplayCount++;
		}
	}

}