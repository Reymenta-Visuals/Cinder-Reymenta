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
	fs::path params = getAssetPath("") / settingsFileName;
	if (fs::exists(params))
	{
		restore();
	}
	else
	{
		save();
	}
}

ParameterBagRef ParameterBag::create()
{
	return shared_ptr<ParameterBag>(new ParameterBag());
}

bool ParameterBag::save()
{
	fs::path directory = getAssetPath("");
	if (!fs::exists(directory)) {
		if (!createDirectories(directory / settingsFileName)) {
			return false;
		}
	}

	fs::path path = directory / settingsFileName;

	XmlTree settings("settings", "");

	XmlTree AutoLayout("AutoLayout", "");
	AutoLayout.setAttribute("value", toString(mAutoLayout));
	settings.push_back(AutoLayout);

	XmlTree Standalone("Standalone", "");
	Standalone.setAttribute("value", toString(mStandalone));
	settings.push_back(Standalone);

	XmlTree MainWindowWidth("MainWindowWidth", "");
	MainWindowWidth.setAttribute("value", toString(mMainWindowWidth));
	settings.push_back(MainWindowWidth);

	XmlTree MainWindowHeight("MainWindowHeight", "");
	MainWindowHeight.setAttribute("value", toString(mMainWindowHeight));
	settings.push_back(MainWindowHeight);

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

	XmlTree OSCDestinationPort2("OSCDestinationPort2", "");
	OSCDestinationPort2.setAttribute("value", toString(mOSCDestinationPort2));
	settings.push_back(OSCDestinationPort2);

	XmlTree OSCDestinationHost2("OSCDestinationHost2", "");
	OSCDestinationHost2.setAttribute("value", toString(mOSCDestinationHost2));
	settings.push_back(OSCDestinationHost2);

	XmlTree MIDIOpenAllInputPorts("MIDIOpenAllInputPorts", "");
	MIDIOpenAllInputPorts.setAttribute("value", toString(mMIDIOpenAllInputPorts));
	settings.push_back(MIDIOpenAllInputPorts);

	XmlTree ShowUI("ShowUI", "");
	ShowUI.setAttribute("value", toString(mShowUI));
	settings.push_back(ShowUI);

	XmlTree CursorVisible("CursorVisible", "");
	CursorVisible.setAttribute("value", toString(mCursorVisible));
	settings.push_back(CursorVisible);

	XmlTree IsOSCSender("IsOSCSender", "");
	IsOSCSender.setAttribute("value", toString(mIsOSCSender));
	settings.push_back(IsOSCSender);

	XmlTree IsWebSocketsServer("IsWebSocketsServer", "");
	IsWebSocketsServer.setAttribute("value", toString(mIsWebSocketsServer));
	settings.push_back(IsWebSocketsServer);

	XmlTree WebSocketsHost("WebSocketsHost", "");
	WebSocketsHost.setAttribute("value", toString(mWebSocketsHost));
	settings.push_back(WebSocketsHost);

	XmlTree WebSocketsPort("WebSocketsPort", "");
	WebSocketsPort.setAttribute("value", toString(mWebSocketsPort));
	settings.push_back(WebSocketsPort);

	XmlTree FftSize("FftSize", "");
	FftSize.setAttribute("value", toString(mFftSize));
	settings.push_back(FftSize);

	XmlTree WindowSize("WindowSize", "");
	WindowSize.setAttribute("value", toString(mWindowSize));
	settings.push_back(WindowSize);

	// write XML file
	settings.write(writeFile(path));

	return true;
}

bool ParameterBag::restore()
{
	// check to see if Settings.xml file exists
	fs::path params = getAssetPath("") / settingsFileName;
	if (fs::exists(params)) {
		// if it does, restore
		const XmlTree xml(loadFile(params));

		if (!xml.hasChild("settings")) {
			return false;
		}
		else {
			const XmlTree settings = xml.getChild("settings");

			if (settings.hasChild("Standalone")) {
				XmlTree Standalone = settings.getChild("Standalone");
				mStandalone = Standalone.getAttributeValue<bool>("value");
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
			if (settings.hasChild("IsOSCSender")) {
				XmlTree IsOSCSender = settings.getChild("IsOSCSender");
				mIsOSCSender = IsOSCSender.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("OSCDestinationPort")) {
				XmlTree OSCDestinationPort = settings.getChild("OSCDestinationPort");
				mOSCDestinationPort = OSCDestinationPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("OSCDestinationHost")) {
				XmlTree OSCDestinationHost = settings.getChild("OSCDestinationHost");
				mOSCDestinationHost = OSCDestinationHost.getAttributeValue<string>("value");
			}
			if (settings.hasChild("OSCDestinationPort2")) {
				XmlTree OSCDestinationPort2 = settings.getChild("OSCDestinationPort2");
				mOSCDestinationPort2 = OSCDestinationPort2.getAttributeValue<int>("value");
			}
			if (settings.hasChild("OSCDestinationHost2")) {
				XmlTree OSCDestinationHost2 = settings.getChild("OSCDestinationHost2");
				mOSCDestinationHost2 = OSCDestinationHost2.getAttributeValue<string>("value");
			}
			if (settings.hasChild("MIDIOpenAllInputPorts")) {
				XmlTree MIDIOpenAllInputPorts = settings.getChild("MIDIOpenAllInputPorts");
				mMIDIOpenAllInputPorts = MIDIOpenAllInputPorts.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("ShowUI")) {
				XmlTree ShowUI = settings.getChild("ShowUI");
				mShowUI = ShowUI.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("CursorVisible")) {
				XmlTree CursorVisible = settings.getChild("CursorVisible");
				mCursorVisible = CursorVisible.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("IsWebSocketsServer")) {
				XmlTree IsWebSocketsServer = settings.getChild("IsWebSocketsServer");
				mIsWebSocketsServer = IsWebSocketsServer.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("WebSocketsHost")) {
				XmlTree WebSocketsHost = settings.getChild("WebSocketsHost");
				mWebSocketsHost = WebSocketsHost.getAttributeValue<string>("value");
			}
			if (settings.hasChild("WebSocketsPort")) {
				XmlTree WebSocketsPort = settings.getChild("WebSocketsPort");
				mWebSocketsPort = WebSocketsPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("FftSize")) {
				XmlTree FftSize = settings.getChild("FftSize");
				mFftSize = FftSize.getAttributeValue<int>("value");
			}
			if (settings.hasChild("WindowSize")) {
				XmlTree WindowSize = settings.getChild("WindowSize");
				mWindowSize = WindowSize.getAttributeValue<int>("value");
			}
			// if AutoLayout is false we have to read the custom screen layout
			if (!mAutoLayout)
			{
				if (settings.hasChild("MainWindowWidth")) {
					XmlTree MainWindowWidth = settings.getChild("MainWindowWidth");
					mMainWindowWidth = MainWindowWidth.getAttributeValue<int>("value");
				}
				if (settings.hasChild("MainWindowHeight")) {
					XmlTree MainWindowHeight = settings.getChild("MainWindowHeight");
					mMainWindowHeight = MainWindowHeight.getAttributeValue<int>("value");
				}
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
				iResolution = Vec3f(mRenderWidth, mRenderHeight, 1.0);

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
	mMIDIOpenAllInputPorts = true;
	mAutoLayout = true;
	mStandalone = false;
	mShowUI = true;
	mCursorVisible = true;
	mOutputVideoResolution = 1024;

	// parameters not exposed in XML
	mMainWindowX = 0;
	mMainWindowY = 0;
	mMainWindowWidth = 1280;
	mMainWindowHeight = 720;
	// render widths
	mRenderWidth = 1024;
	mRenderHeight = 768;
	mRenderXY = mLeftRenderXY = mRightRenderXY = mPreviewRenderXY = mWarp1RenderXY = mWarp2RenderXY = Vec2f::zero();
	mRenderPosXY = Vec2f(0.0, 320.0);
	mRenderResoXY = Vec2f(mRenderWidth, mRenderHeight);
	mRenderResolution = Vec2i(mRenderWidth, mRenderHeight);
	mPreviewFragXY = Vec2f(0.0, 0.0);
	mFboWidth = 640;
	mFboHeight = 480;
	mPreviewFboWidth = 80;
	mPreviewFboHeight = 60;
	mPreviewWidth = 160;
	mPreviewHeight = 120;
	mRenderCodeEditorXY.x = 0;
	mRenderCodeEditorXY.y = 0;
	mCodeEditorWidth = 800;
	mCodeEditorHeight = 600;

	mWindowToCreate = NONE;

	mMode = mPreviousMode = mNewMode = 1; // Mix mode by default
	mCurrentFilePath = "currentMix.frag";
	mMarginSmall = 2;

	mCamPosXY = Vec2f::zero();
	mCount = 1;
	mLockFR = mLockFG = mLockFB = mLockFA = mLockBR = mLockBG = mLockBB = mLockBA = false;
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
	mIsPlaying = false;
	mBeat = 0;
	mFftSize = 512;
	mWindowSize = 1024;

	maxVolume = 0.0f;
	liveMeter = 0.0f;
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
	controlValues[18] = controlValues[17] = 1.0;
#ifdef _DEBUG
	iDebug = true;
#else
	iDebug = false;
#endif  // _DEBUG
	iFade = mSendToOutput = iLight = iLightAuto = iRepeat = false;
	iFps = 60.0;
	sFps = "60";
	iShowFps = true;
	iMouse = Vec4f(mRenderWidth / 2, mRenderHeight / 2, 1.0, 1.0);
	iGreyScale = false;

	// transition
	iTransition = 0;
	iAnim = 0.0;
	mTransitionDuration = 1.0f;

	mOriginUpperLeft = true;
	multFactor = 126.0;
	currentSelectedIndex = 0;
	selectedWarp = 0;
	iChannels[0] = 0;
	for (int a = 1; a < 8; a++)
	{
		iChannels[a] = a;
	}
	//iChannels[1] = 2;
	//iChannels[2] = 3;
	iChannels[4] = 10;
	// fbo indexes for warp
	mFboResolution = 2048;
	/*mCurrentPreviewFboIndex = 0;
	mMixFboIndex = 1;
	mAudioFboIndex = 2;
	mWarpFboIndex = 3;
	mSphereFboIndex = 4;
	mMeshFboIndex = 5;
	mLeftFboIndex = 6;
	mRightFboIndex = 7;
	mWarp1FboIndex = 8; 
	mWarp2FboIndex = 9;
	mLiveFboIndex = 10;
	mVertexSphereFboIndex = 11;*/
	mMixFboIndex = 0;
	mLeftFboIndex = 1;
	mRightFboIndex = 2;
	mWarp1FboIndex = 3;
	mWarp2FboIndex = 4;
	mCurrentPreviewFboIndex = 5;
	mABPFboIndex = 6;
	mWarpFboIndex = 7;
	mSphereFboIndex = 8;
	mMeshFboIndex = 9;
	mAudioFboIndex = 10;
	mVertexSphereFboIndex = 11;

	mPreviewFragIndex = 0;
	mPreviousFragIndex = 1;
	mLeftFragIndex = 0;
	mRightFragIndex = 1;
	mWarp1FragIndex = 2;
	mWarp2FragIndex = 3;
	mFlipH = false;
	mWarpCount = 3;
	FPSColor = ColorA(0.0f, 1.0f, 0.0f, 1.0f);
	ColorGreen = ColorA(0.0f, 1.0f, 0.0f, 1.0f);
	ColorRed = ColorA(1.0f, 0.0f, 0.0f, 1.0f);
	ColorYellow = ColorA(1.0f, 1.0f, 0.0f, 1.0f);
	ColorPurple = ColorA(0.5f, 0.0f, 1.0f, 1.0f);
	isUIDirty = true;
	//isShaderDirty = false;
	mShaderToLoad = "";

	mStateTransition = 1.0;

	mOptimizeUI = false;
	// spout
	mOutputResolution = Vec2f(640, 480);
	// meshes
	mMeshIndex = 0;
	// vertex sphere
	mVertexSphereTextureIndex = 1;

	// initialize our camera
	mCamEyePointXY = Vec2f(0.f, 0.f);
	mCamEyePointZ = -400.f;
	mCamera.setEyePoint(Vec3f(mCamEyePointXY.x, mCamEyePointXY.y, mCamEyePointZ));
	mCamera.setCenterOfInterestPoint(Vec3f(0.f, 0.f, 0.f));

	mUIRefresh = 1;

	// midi
	for (int c = 0; c < 128; c++)
	{
		controlValues[c] = 0.01f;
	}
	// OSC
	mOSCDestinationHost = "127.0.0.1";
	mOSCDestinationPort = 7001;
	mOSCDestinationHost = "127.0.0.1";
	mOSCDestinationPort = 7002;
	mOSCReceiverPort = 7000;
	OSCMsg = "";
	newOSCMsg = false;
	WSMsg = "";
	newWSMsg = false;
	InfoMsg = "";
	mIsOSCSender = false;
	// web sockets
	mIsWebSocketsServer = false;
	mWebSocketsHost = "localhost";
	mWebSocketsPort = 9002;
	// abp
	mBend = 0.0f;
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
	// pointsphere zPosition
	controlValues[9] = -0.7f;
	// ratio
	controlValues[11] = 20.0f;
	// Speed 
	controlValues[12] = 12.0f;
	// zoom
	controlValues[13] = iZoomLeft = iZoomRight = 1.0f;
	// exposure
	controlValues[14] = 1.0f;
	// Pixelate
	controlValues[15] = 1.0f;
	// Steps
	controlValues[16] = 16.0f;
	// iPreviewCrossfade
	controlValues[17] = 1.0f;
	// iCrossfade
	controlValues[18] = 1.0f;
	// RotationSpeed
	controlValues[19] = 0.0f;
	// Blendmode 
	controlValues[20] = 0.0f;
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
