//
//  ParameterBag.cpp
//  From Nathan Selikoff on 4/26/14.
//
//

#include "ParameterBag.h"

using namespace ci;
using namespace std;
using namespace Reymenta;

ParameterBag::ParameterBag()
{
	// reset no matter what, so we don't miss anything
	reset();

	// check to see if ReymentaSettings.xml file exists and restore if it does
	fs::path params = getDocumentsDirectory() / "Reymenta" / "ReymentaSettings.xml";
	if (fs::exists(params))
		restore();
}

ParameterBagRef ParameterBag::create()
{
	return shared_ptr<ParameterBag>(new ParameterBag());
}

bool ParameterBag::save()
{
	string filename = "ReymentaSettings.xml";

	// attempt to create "Beautiful Chaos" directory in Documents directory
	fs::path directory = getDocumentsDirectory() / "Reymenta";
	if (!fs::exists(directory)) {
		if (!createDirectories(directory / filename)) {
			return false;
		}
	}

	fs::path path = directory / filename;

	XmlTree settings("settings", "");

	XmlTree AutoLayout("AutoLayout", "");
	AutoLayout.setAttribute("value", toString(mAutoLayout));
	settings.push_back(AutoLayout);

	XmlTree RenderWidth("RenderWidth", "");
	RenderWidth.setAttribute("value", toString(mRenderWidth));
	settings.push_back(RenderWidth);

	XmlTree RenderHeight("RenderHeight", "");
	RenderHeight.setAttribute("value", toString(mRenderHeight));
	settings.push_back(RenderHeight);

	XmlTree RenderX("RenderX", "");
	RenderX.setAttribute("value", toString(mRenderX));
	settings.push_back(RenderX);

	XmlTree RenderY("RenderY", "");
	RenderY.setAttribute("value", toString(mRenderY));
	settings.push_back(RenderY);

	XmlTree FboWidth("FboWidth", "");
	FboWidth.setAttribute("value", toString(mFboWidth));
	settings.push_back(FboWidth);

	XmlTree FboHeight("FboHeight", "");
	FboHeight.setAttribute("value", toString(mFboHeight));
	settings.push_back(FboHeight);

	XmlTree OSCReceiverPort("OSCReceiverPort", "");
	OSCReceiverPort.setAttribute("value", toString(mOSCReceiverPort));
	settings.push_back(OSCReceiverPort);

	XmlTree OSCDestinationPort("OSCDestinationPort", "");
	OSCDestinationPort.setAttribute("value", toString(mOSCDestinationPort));
	settings.push_back(OSCDestinationPort);

	XmlTree OSCDestinationHost("OSCDestinationHost", "");
	OSCDestinationHost.setAttribute("value", toString(mOSCDestinationHost));
	settings.push_back(OSCDestinationHost);

	// TODO: test for successful writing of XML
	settings.write(writeFile(path));

	return true;
}

bool ParameterBag::restore()
{
	// check to see if ReymentaSettings.xml file exists
	fs::path params = getDocumentsDirectory() / "Reymenta" / "ReymentaSettings.xml";
	if (fs::exists(params)) {
		// if it does, restore
		const XmlTree xml(loadFile(params));

		if (!xml.hasChild("settings")) {
			return false;
		}
		else {
			const XmlTree settings = xml.getChild("settings");

			if (settings.hasChild("OSCReceiverPort")) {
				XmlTree OSCReceiverPort = settings.getChild("OSCReceiverPort");
				mOSCReceiverPort = OSCReceiverPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("AutoLayout")) {
				XmlTree AutoLayout = settings.getChild("AutoLayout");
				mAutoLayout = AutoLayout.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("FboWidth")) {
				XmlTree FboWidth = settings.getChild("FboWidth");
				mFboWidth = FboWidth.getAttributeValue<int>("value");
			}
			if (settings.hasChild("FboHeight")) {
				XmlTree FboHeight = settings.getChild("FboHeight");
				mFboHeight = FboHeight.getAttributeValue<int>("value");
			}
			if (settings.hasChild("OSCReceiverPort")) {
				XmlTree OSCReceiverPort = settings.getChild("OSCReceiverPort");
				mOSCReceiverPort = OSCReceiverPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("OSCDestinationPort")) {
				XmlTree OSCDestinationPort = settings.getChild("OSCDestinationPort");
				mOSCDestinationPort = OSCDestinationPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("OSCDestinationHost")) {
				XmlTree OSCDestinationHost = settings.getChild("OSCDestinationHost");
				mOSCDestinationHost = OSCDestinationHost.getAttributeValue<string>("value");
			}
			// if AutoLayout is false we have to read the custom screen layout
			if (!mAutoLayout)
			{
				if (settings.hasChild("RenderWidth")) {
					XmlTree RenderWidth = settings.getChild("RenderWidth");
					mRenderWidth = RenderWidth.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderHeight")) {
					XmlTree RenderHeight = settings.getChild("RenderHeight");
					mRenderHeight = RenderHeight.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderX")) {
					XmlTree RenderX = settings.getChild("RenderX");
					mRenderX = RenderX.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderY")) {
					XmlTree RenderY = settings.getChild("RenderY");
					mRenderY = RenderY.getAttributeValue<int>("value");
				}

			}
			return true;
		}
	}
	else {
		// if it doesn't, return false
		return false;
	}
}

void ParameterBag::reset()
{
	// parameters exposed in XML
	//mDirectRender = false;
	mAutoLayout = true;
	mOutputVideoResolution = 1024;

	// parameters not exposed in XML
	mMainWindowX = 150;
	mMainWindowY = 140;
	mMainWindowWidth = 1280;
	mMainWindowHeight = 720;
	// render widths
	mRenderWidth = 1024;
	mRenderHeight = 768;
	mRenderXY = mLeftRenderXY = mRightRenderXY = mPreviewRenderXY = Vec2f::zero();
	mRenderPosXY = Vec2f(0.0, 320.0);
	mRenderResoXY = Vec2f(mRenderWidth, mRenderHeight);
	mRenderResolution = Vec2i(mRenderWidth, mRenderHeight);
	mPreviewFragXY = Vec2f(0.0, 0.0);
	mFboWidth = 640;
	mFboHeight = 480;
	mPreviewFboWidth = mFboWidth/4;
	mPreviewFboHeight = mFboHeight / 4;
	mPreviewWidth = 156;
	mPreviewHeight = 88;
	mRenderCodeEditorXY.x = 0;
	mRenderCodeEditorXY.y = 0;
	mCodeEditorWidth = 800;
	mCodeEditorHeight = 600;

	mWindowToCreate = NONE;

	mMode = mPreviousMode = mNewMode = 1; // Mix mode by default
	mCurrentFilePath = "currentMix.frag";
	mMarginSmall = 2;

	// OSC
	mOSCDestinationHost = "127.0.0.1";// "192.168.0.18";
	mOSCDestinationPort = 7001;
	mOSCReceiverPort = 7000;
	OSCMsg = "OSC listening on port 7000";
	InfoMsg = "";// "Host: " + mOSCDestinationHost;

	mCamPosXY = Vec2f::zero();
	mCount = 1;
	mZPosition = -0.7f;
	mLockFR = mLockFG = mLockFB = mLockFA = mLockBR = mLockBG = mLockBB = mLockBA = false;
	//autoChannel1Speed = autoChannel2Speed = autoChannel3Speed = autoChannel4Speed = autoChannel5Speed = autoChannel6Speed = autoChannel7Speed =  mLockF0 = mLockF0 = mLockF1 = mLockF2 = false;
	//autoRenderX = autoRenderY = autoPosX = autoPosY = autoResoX = autoResoY= false;
	tFR = tFG = tFB = tFA = tBR = tBG = tBB = tBA = false;

	// EyePointZ
	defaultEyePointZ = -400.f;
	minEyePointZ = -800.0;
	maxEyePointZ = 0.0;
	tEyePointZ = autoEyePointZ = false;

	mPreviewEnabled = true;
	//audio
	// audio in multiplication factor
	mAudioMultFactor = 1.0;
	mUseLineIn = true;
	maxVolume = 0.0f;
	mData = new float[1024];
	for (int i = 0; i < 1024; i++)
	{
		mData[i] = 0;
	}
	for (int i = 0; i < 4; i++)
	{
		iFreqs[i] = i;
	}

	// tempo
	mTempo = 166.0;
	mUseTimeWithTempo = false;
	iDeltaTime = 60 / mTempo;
	iTempoTime = 0.0;
	iTimeFactor = 1.0;
	// shader uniforms
	iGlobalTime = 1.0f;
	iResolution = Vec3f(mRenderWidth, mRenderHeight, 1.0);
	for (int i = 0; i < 4; i++)
	{
		iChannelTime[i] = i;
	}
	for (int i = 0; i < 4; i++)
	{
		iChannelResolution[i] = Vec3f(mRenderWidth, mRenderHeight, 1.0);
	}	
	iCrossfade = iPreviewCrossfade = 1.0;
	iDebug = iFade = mSendToOutput = iLight = iLightAuto = iRepeat = false;
	iFps = 60.0;
	iShowFps = true;
	iMouse = Vec4f(mRenderWidth / 2, mRenderHeight / 2, 1.0, 1.0);
	iGreyScale = false;

	// transition
	iTransition = 0;
	iAnim = 0.0;
	mTransitionDuration = 1.0f;

	mOriginUpperLeft = false;
	multFactor = 126.0;
	currentSelectedIndex = 0;
	iChannels[0] = 1;
	for (int a = 1; a < 8; a++)
	{
		iChannels[a] = a;
	}

	iWarpFboChannels[0] = 1;
	iWarpFboChannels[1] = 1;
	iWarpFboChannels[2] = 1;
	iWarpFboChannels[3] = 3;
	iWarpFboChannels[4] = 4;
	iWarpFboChannels[5] = 5;
	iWarpFboChannels[6] = 6;
	iWarpFboChannels[7] = 7;

	FPSColor = ColorA(0.0f, 1.0f, 0.0f, 1.0f);
	ColorGreen = ColorA(0.0f, 1.0f, 0.0f, 1.0f);
	ColorRed = ColorA(1.0f, 0.0f, 0.0f, 1.0f);
	ColorYellow = ColorA(1.0f, 1.0f, 0.0f, 1.0f);
	ColorPurple = ColorA(0.5f, 0.0f, 1.0f, 1.0f);
	isUIDirty = true;
	isShaderDirty = false;
	mShaderToLoad = "";

	mStateTransition = 1.0;

	mOptimizeUI = false;
	// spout
	mOutputResolution = Vec2f(630, 345);
	// meshes
	mMeshIndex = 0;
	// fbo indexes for warp

	mFboResolution = 2048;
	mCurrentPreviewFboIndex = 0;
	mMixFboIndex = 1;
	mAudioFboIndex = 2;
	mWarpFboIndex = 3;
	mSphereFboIndex = 4;
	mMeshFboIndex = 5;
	mLeftFboIndex = 6;
	mRightFboIndex = 7;
	mLiveFboIndex = 8;

	mPreviewFragIndex = 0;
	mPreviousFragIndex = 1;
	mLeftFragIndex = 0;
	mRightFragIndex = 1;

	mWarpCount = 3;
	// initialize our camera
	mCamEyePointXY = Vec2f(0.f, 0.f);
	mCamEyePointZ = -400.f;
	mCamera.setEyePoint(Vec3f(mCamEyePointXY.x, mCamEyePointXY.y, mCamEyePointZ));
	mCamera.setCenterOfInterestPoint(Vec3f(0.f, 0.f, 0.f));

	mUIRefresh = 1;

	// midi and OSC
	for (int c = 0; c < 128; c++)
	{
		controlValues[c] = 0.01f;
	}
	// red
	controlValues[1] = 1.0f;
	// green
	controlValues[2] = 0.3f;
	// blue
	controlValues[3] = 0.0f;
	// Alpha 
	controlValues[4] = 1.0f;
	// background red
	controlValues[5] = 0.1f;
	// background green
	controlValues[6] = 0.1f;
	// background blue
	controlValues[7] = 0.1f;
	// background alpha
	controlValues[8] = 0.2f;
	// ratio
	controlValues[11] = 20.0f;
	// Speed 
	controlValues[12] = 12.0f;
	// zoom
	controlValues[13] = iZoomLeft = iZoomRight = 1.0f;
	// exposure
	controlValues[14] = 1.0f;
	// Blendmode 
	controlValues[15] = 0.0f;
	// Steps
	controlValues[16] = 16.0f;
	// Pixelate
	controlValues[18] = 1.0f;
	// RotationSpeed
	controlValues[19] = 1.0f;
	// glitch
	controlValues[45] = 0.0f;
	// toggle
	controlValues[46] = 0.0f;
	// vignette
	controlValues[47] = 0.0f;
	// invert
	controlValues[48] = 0.0f;
	autoInvert = false;


}
