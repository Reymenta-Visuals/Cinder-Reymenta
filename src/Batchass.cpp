#include "Batchass.h"

using namespace Reymenta;

Batchass::Batchass(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;
	// instanciate the logger class
	log = Logger::create("BatchassLog.txt");
	log->logTimedString("Batchass constructor");
}
void Batchass::getWindowsResolution()
{
	mParameterBag->mDisplayCount = 0;
	if (mParameterBag->mAutoLayout)
	{
		// Display sizes
		mParameterBag->mMainDisplayWidth = Display::getMainDisplay()->getWidth();
		mParameterBag->mMainDisplayHeight = Display::getMainDisplay()->getHeight();
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
		mParameterBag->mRenderResoXY = vec2(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);
		// in case only one screen , render from x = 0
		if (mParameterBag->mDisplayCount == 1) mParameterBag->mRenderX = 0; else mParameterBag->mRenderX = mParameterBag->mMainDisplayWidth;
	}	
	else
	{
		for (auto display : Display::getDisplays())
		{
			mParameterBag->mDisplayCount++;
		}
	}
}

