#include "Batchass.h"

using namespace Reymenta;

Batchass::Batchass(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;
	// instanciate the logger class
	mLog = Logger::create("BatchassLog.txt");
	mLog->logTimedString("Batchass constructor");
	// zoom
	defaultZoom = 1.0f;
	minZoom = 0.1;
	maxZoom = 3.0;
	tZoom = autoZoom = false;
	// exposure
	defaultExposure = 1.0;
	minExposure = 0.0001;
	maxExposure = 2.0;
	tExposure = autoExposure = false;
	// Chromatic
	defaultChromatic = 0.0;
	minChromatic = 0.0;
	maxChromatic = 1.0;
	tChromatic = autoChromatic = false;
	// ratio
	defaultRatio = 1.0;
	minRatio = 0.00000000001;
	maxRatio = 1.0;
	tRatio = autoRatio = false;
	// z position
	defaultZPos = -0.7;
	minZPos = -1.0;
	maxZPos = 1.0;
	tZPos = autoZPos = false;
	// RotationSpeed
	defaultRotationSpeed = 0.0;
	minRotationSpeed = -2.0;
	maxRotationSpeed = 2.0;
	tRotationSpeed = autoRotationSpeed = false;
	// tempo
	counter = 0;
	//startTime = timer.getSeconds();
	//currentTime = timer.getSeconds();
	mParameterBag->iDeltaTime = 60 / mParameterBag->mTempo;
	previousTime = 0.0f;
	beatIndex = 0;
}
float Batchass::formatFloat(float f)
{
	int i;
	f *= 100;
	i = ((int)f) / 100;
	return (float)i;
}
void Batchass::setup()
{
	// instanciate the Shaders class, must not be in prepareSettings
	mShaders = Shaders::create(mParameterBag);

	// instanciate the textures class
	mTextures = Textures::create(mParameterBag, mShaders);
	// instanciate the warps class
	mWarpings = WarpWrapper::create(mParameterBag, mTextures, mShaders);

	//createWarpFbos();
}
void Batchass::createWarpFbos()
{
	// vector + dynamic resize
	/*for (int a = 0; a < 30; a++)
	{
	WarpFbo newWarpFbo;
	if (a == 0)
	{
	newWarpFbo.textureIndex = 0; // spout
	newWarpFbo.textureMode = mParameterBag->TEXTUREMODEINPUT;
	newWarpFbo.active = true;
	newWarpFbo.fbo = gl::Fbo(mParameterBag->mFboWidth, mParameterBag->mFboHeight);
	}
	else
	{
	newWarpFbo.textureIndex = 0; // index of MixFbo for shadamixa
	newWarpFbo.textureMode = mParameterBag->TEXTUREMODESHADER;
	newWarpFbo.active = false;
	newWarpFbo.fbo = gl::Fbo(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight);
	}
	mParameterBag->mWarpFbos.push_back( newWarpFbo );
	}*/
}
void Batchass::createWarp()
{
	mWarpings->createWarp();
}
void Batchass::assignFboToWarp(int index, int fbo)
{
	if (index > -1 && index < mParameterBag->MAX && fbo > -1 && fbo < mTextures->getFboCount())
		mParameterBag->mWarpFbos[index].textureIndex = fbo;
}
void Batchass::assignTextureToChannel(int selectedTexture_index, int selectedChannel)
{
	if (selectedTexture_index > -1 && selectedTexture_index < mTextures->getTextureCount() && selectedChannel > -1 && selectedChannel < mParameterBag->iChannels.size())
		mParameterBag->iChannels[selectedChannel] = selectedTexture_index;
}

void Batchass::changeMode(int newMode)
{
	if (mParameterBag->mMode != newMode)
	{
		mParameterBag->controlValues[4] = 1.0f;
		mParameterBag->controlValues[8] = 1.0f;

		mParameterBag->mPreviousMode = mParameterBag->mMode;
		switch (mParameterBag->mPreviousMode)
		{
		case 5: //mesh
			mParameterBag->iLight = false;
			mParameterBag->controlValues[19] = 0.0; //reset rotation
			break;
		}
		mParameterBag->mMode = newMode;

		switch (newMode)
		{
		case 1: // Mix
			break;
		case 2: // Audio
			break;
		case 3: // Warp
			break;
		case 4: // sphere
			mParameterBag->mCamPosXY = Vec2f(-155.6, -87.3);
			mParameterBag->mCamEyePointZ = -436.f;
			mParameterBag->controlValues[5] = mParameterBag->controlValues[6] = mParameterBag->controlValues[7] = 0;
			break;
		case 5: // mesh
			mParameterBag->controlValues[19] = 1.0; //reset rotation
			mParameterBag->mRenderPosXY = Vec2f(0.0, 0.0);
			mParameterBag->mCamEyePointZ = -10.f;
			mParameterBag->controlValues[5] = mParameterBag->controlValues[6] = mParameterBag->controlValues[7] = 0;
			mParameterBag->currentSelectedIndex = 5;
			mParameterBag->iLight = true;
			break;
		case 7: // Live
			//mShaders->setupLiveShader();
			mParameterBag->mLiveCode = true;
			break;


		}
	}
}
void Batchass::update()
{
	mTextures->update();
	//mShaders->update();
	if (mParameterBag->controlValues[12] == 0.0) mParameterBag->controlValues[12] = 0.01;

#pragma region animation
	// check this line position: can't remember
	currentTime = timer.getSeconds();

	int time = (currentTime - startTime)*1000000.0;
	int elapsed = mParameterBag->iDeltaTime*1000000.0;
	if (elapsed > 0)
	{
		double modulo = (time % elapsed) / 1000000.0;
		mParameterBag->iTempoTime = (float)modulo;
		if (mParameterBag->iTempoTime < previousTime)
		{
			beatIndex++;
			if (beatIndex > 3) beatIndex = 0;
		}
		previousTime = mParameterBag->iTempoTime;

		//(modulo < 0.1) ? tempoMvg->setNameColor(ColorA::white()) : tempoMvg->setNameColor(UIController::DEFAULT_NAME_COLOR);
		// exposure
		if (tExposure)
		{
			mParameterBag->controlValues[14] = (modulo < 0.1) ? maxExposure : minExposure;
		}
		else
		{
			mParameterBag->controlValues[14] = autoExposure ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, minExposure, maxExposure) : mParameterBag->controlValues[14];
			//mParameterBag->controlValues[14] = autoExposure ? (sin(getElapsedFrames() / (mParameterBag->controlValues[12] + 1.0))) : mParameterBag->controlValues[14];
		}
		// zoom
		if (tZoom)
		{
			mParameterBag->controlValues[13] = (modulo < 0.1) ? maxZoom : minZoom;
		}
		else
		{
			mParameterBag->controlValues[13] = autoZoom ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, minZoom, maxZoom) : mParameterBag->controlValues[13];
		}
		// ratio
		if (tRatio)
		{
			mParameterBag->controlValues[11] = (modulo < 0.1) ? maxRatio : minRatio;
		}
		else
		{
			mParameterBag->controlValues[11] = autoRatio ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, minRatio, maxRatio) : mParameterBag->controlValues[11];
		}
		// Chromatic
		if (tChromatic)
		{
			mParameterBag->controlValues[10] = (modulo < 0.1) ? maxChromatic : minChromatic;
		}
		else
		{
			mParameterBag->controlValues[10] = autoChromatic ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, minChromatic, maxChromatic) : mParameterBag->controlValues[10];
		}
		// RotationSpeed
		if (tRotationSpeed)
		{
			mParameterBag->controlValues[19] = (modulo < 0.1) ? maxRotationSpeed : minRotationSpeed;
		}
		else
		{
			mParameterBag->controlValues[19] = autoRotationSpeed ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, minRotationSpeed, maxRotationSpeed) : mParameterBag->controlValues[19];
		}
		// ZPos
		if (tZPos)
		{
			mParameterBag->controlValues[9] = (modulo < 0.1) ? maxZPos : minZPos;
		}
		else
		{
			mParameterBag->controlValues[9] = autoZPos ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, minZPos, maxZPos) : mParameterBag->controlValues[9];
		}

		// Front Red
		if (mParameterBag->tFR)
		{
			mParameterBag->controlValues[1] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[1] = mParameterBag->mLockFR ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[1];
		}
		// Front Green
		if (mParameterBag->tFG)
		{
			mParameterBag->controlValues[2] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[2] = mParameterBag->mLockFG ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[2];
		}
		// front blue
		if (mParameterBag->tFB)
		{
			mParameterBag->controlValues[3] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[3] = mParameterBag->mLockFB ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[3];
		}
		// front alpha
		if (mParameterBag->tFA)
		{
			mParameterBag->controlValues[4] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[4] = mParameterBag->mLockFA ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[4];
		}
		// 
		if (mParameterBag->tBR)
		{
			mParameterBag->controlValues[5] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[5] = mParameterBag->mLockBR ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[5];
		}
		// 
		if (mParameterBag->tBG)
		{
			mParameterBag->controlValues[6] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[6] = mParameterBag->mLockBG ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[6];
		}
		// 
		if (mParameterBag->tBB)
		{
			mParameterBag->controlValues[7] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[7] = mParameterBag->mLockBB ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[7];
		}
		// 
		if (mParameterBag->tBA)
		{
			mParameterBag->controlValues[8] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[8] = mParameterBag->mLockBA ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[8];
		}
		if (mParameterBag->autoInvert)
		{
			mParameterBag->controlValues[48] = (modulo < 0.1) ? 1.0 : 0.0;
		}

		if (mParameterBag->tEyePointZ)
		{
			mParameterBag->mCamEyePointZ = (modulo < 0.1) ? mParameterBag->minEyePointZ : mParameterBag->maxEyePointZ;
		}
		else
		{
			mParameterBag->mCamEyePointZ = mParameterBag->autoEyePointZ ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, mParameterBag->minEyePointZ, mParameterBag->maxEyePointZ) : mParameterBag->mCamEyePointZ;
		}

	}

#pragma endregion animation
}
void Batchass::shutdownLoader()
{
	mShaders->shutdownLoader();
}
int Batchass::getWindowsResolution()
{
	mParameterBag->mDisplayCount = 0;
	int w = Display::getMainDisplay()->getWidth();
	int h = Display::getMainDisplay()->getHeight();
	// Display sizes
	mParameterBag->mMainDisplayWidth = w;
	mParameterBag->mMainDisplayHeight = h;
	if (mParameterBag->mCustomLayout)
	{
		// for MODE_MIX and triplehead(or doublehead), we only want 1/3 of the screen centered	
		for (auto display : Display::getDisplays())
		{
			mParameterBag->mDisplayCount++;
		}
	}
	else
	{
		mParameterBag->mRenderX = mParameterBag->mMainDisplayWidth;
		for (auto display : Display::getDisplays())
		{
			mParameterBag->mDisplayCount++;
			mParameterBag->mRenderWidth = display->getWidth();
			mParameterBag->mRenderHeight = display->getHeight();
		}	
	}
	mParameterBag->mRenderY = 0;
	mLog->logTimedString("Window " + toString(mParameterBag->mDisplayCount) + ": " + toString(mParameterBag->mRenderWidth) + "x" + toString(mParameterBag->mRenderHeight));

	mLog->logTimedString(" mMainDisplayWidth" + toString(mParameterBag->mMainDisplayWidth) + " mMainDisplayHeight" + toString(mParameterBag->mMainDisplayHeight));
	mLog->logTimedString(" mRenderX" + toString(mParameterBag->mRenderX) + " mRenderY" + toString(mParameterBag->mRenderY));
	mParameterBag->mRenderResoXY = Vec2f(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);
	// in case only one screen , render from x = 0
	if (mParameterBag->mDisplayCount == 1) mParameterBag->mRenderX = 0;
	return w;
}
void Batchass::log(string msg)
{
	mLog->logTimedString(msg);
}
void Batchass::tempoZoom()
{
	tZoom = !tZoom;
	if (!tZoom) resetZoom();
}
void Batchass::resetZoom()
{
	autoZoom = false;
	tZoom = false;
	mParameterBag->controlValues[13] = defaultZoom;
}

void Batchass::tempoZPos()
{
	tZPos = !tZPos;
	if (!tZPos) resetZPos();
}
void Batchass::resetZPos()
{
	autoZPos = false;
	tZPos = false;
	mParameterBag->controlValues[9] = defaultZPos;
}
void Batchass::tempoRotationSpeed()
{
	tRotationSpeed = !tRotationSpeed;
	if (!tRotationSpeed) resetRotationSpeed();
}
void Batchass::resetRotationSpeed()
{
	autoRotationSpeed = false;
	tRotationSpeed = false;
	mParameterBag->controlValues[19] = defaultRotationSpeed;
}

void Batchass::tempoExposure()
{
	tExposure = !tExposure;
	if (!tExposure) resetExposure();
}
void Batchass::resetExposure()
{
	autoExposure = false;
	tExposure = false;
	mParameterBag->controlValues[14] = defaultExposure;
}
// chromatic
void Batchass::tempoChromatic()
{
	tChromatic = !tChromatic;
	if (!tChromatic) resetChromatic();
}
void Batchass::resetChromatic()
{
	autoChromatic = false;
	tChromatic = false;
	mParameterBag->controlValues[10] = defaultChromatic;
}
// ratio
void Batchass::tempoRatio()
{
	tRatio = !tRatio;
	if (!tRatio) resetRatio();
}
void Batchass::resetRatio()
{
	autoRatio = false;
	tRatio = false;
	mParameterBag->controlValues[11] = defaultRatio;
}
// tempo
void Batchass::tapTempo()
{
	startTime = currentTime = timer.getSeconds();

	timer.stop();
	timer.start();

	// check for out of time values - less than 50% or more than 150% of from last "TAP and whole time budder is going to be discarded....
	if (counter > 2 && (buffer.back() * 1.5 < currentTime || buffer.back() * 0.5 > currentTime))
	{
		buffer.clear();
		counter = 0;
		averageTime = 0;
	}
	if (counter >= 1)
	{
		buffer.push_back(currentTime);
		calculateTempo();
	}
	counter++;
}
void Batchass::calculateTempo()
{
	// NORMAL AVERAGE
	double tAverage = 0;
	for (int i = 0; i < buffer.size(); i++)
	{
		tAverage += buffer[i];
	}
	averageTime = (double)(tAverage / buffer.size());
	mParameterBag->iDeltaTime = averageTime;
	mParameterBag->mTempo = 60 / averageTime;
}
void Batchass::setTimeFactor(const int &aTimeFactor)
{
	switch (aTimeFactor)
	{
	case 0:
		mParameterBag->iTimeFactor = 0.0001;
		break;
	case 1:
		mParameterBag->iTimeFactor = 0.125;
		break;
	case 2:
		mParameterBag->iTimeFactor = 0.25;
		break;
	case 3:
		mParameterBag->iTimeFactor = 0.5;
		break;
	case 4:
		mParameterBag->iTimeFactor = 0.75;
		break;
	case 5:
		mParameterBag->iTimeFactor = 1.0;
		break;
	case 6:
		mParameterBag->iTimeFactor = 2.0;
		break;
	case 7:
		mParameterBag->iTimeFactor = 4.0;
		break;
	case 8:
		mParameterBag->iTimeFactor = 16.0;
		break;
	default:
		mParameterBag->iTimeFactor = 1.0;
		break;
	}
}
