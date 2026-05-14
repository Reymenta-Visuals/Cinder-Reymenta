//
//  VDSettings.cpp
//

#include "VDSettings.h"

using namespace videodromm;

VDSettings::VDSettings(const std::string& filename)
{
	// reset no matter what, so we don't miss anything
	reset();
	settingsFileName = "VD" + filename + ".xml";
	// check to see if Settings.xml file exists and restore if it does
	fs::path params = getAppPath() / settingsFileName;
	if (fs::exists(params))
	{
		restore();
	}
	else
	{
		save();
	}
}

VDSettingsRef VDSettings::create(const std::string& filename)
{
	return std::shared_ptr<VDSettings>(new VDSettings(filename));
}


//! to json
/*ci::Json VDSettings::toJson( bool save ) const
{
	ci::Json json;
	ci::Json osc = ci::Json::array();
	osc.push_back(ci::Json{ {"name", "osc"} });

	json.push_back( osc );

	return json;
}*/
bool VDSettings::save()
{
	fs::path directory = getAppPath();
	/*if (!fs::exists(directory)) {
		if (!createDirectories(directory / settingsFileName)) {
		return false;
		}
		}*/

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

	XmlTree MainWindowX("MainWindowX", "");
	MainWindowX.setAttribute("value", toString(mMainWindowX));
	settings.push_back(MainWindowX);

	XmlTree MainWindowY("MainWindowY", "");
	MainWindowY.setAttribute("value", toString(mMainWindowY));
	settings.push_back(MainWindowY);

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

	XmlTree MIDIEnabled("MIDIEnabled", "");
	MIDIEnabled.setAttribute("value", toString(mMIDIEnabled));
	settings.push_back(MIDIEnabled);

	XmlTree OSCEnabled("OSCEnabled", "");
	OSCEnabled.setAttribute("value", toString(mOSCEnabled));
	settings.push_back(OSCEnabled);

	/*XmlTree OSCReceiverPort("OSCReceiverPort", "");
	OSCReceiverPort.setAttribute("value", toString(mOSCReceiverPort));
	settings.push_back(OSCReceiverPort);*/

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

	XmlTree AreSocketIOEnabledAtStartup("AreSocketIOEnabledAtStartup", "");
	AreSocketIOEnabledAtStartup.setAttribute("value", toString(mAreSocketIOEnabledAtStartup));
	settings.push_back(AreSocketIOEnabledAtStartup);

	XmlTree IsRouter("IsRouter", "");
	IsRouter.setAttribute("value", toString(mIsRouter));
	settings.push_back(IsRouter);

	XmlTree IsSocketIOServer("IsSocketIOServer", "");
	IsSocketIOServer.setAttribute("value", toString(mIsSocketIOServer));
	settings.push_back(IsSocketIOServer);

	XmlTree SocketIOProtocol("SocketIOProtocol", "");
	SocketIOProtocol.setAttribute("value", mSocketIOProtocol);
	settings.push_back(SocketIOProtocol);

	XmlTree SocketIOHost("SocketIOHost", "");
	SocketIOHost.setAttribute("value", mSocketIOHost);
	settings.push_back(SocketIOHost);

	XmlTree SocketIOPort("SocketIOPort", "");
	SocketIOPort.setAttribute("value", toString(mSocketIOPort));
	settings.push_back(SocketIOPort);

	XmlTree SocketIORoom("SocketIORoom", "");
	SocketIORoom.setAttribute("value", mSocketIORoom);
	settings.push_back(SocketIORoom);

	XmlTree SocketIONickname("SocketIONickname", "");
	SocketIONickname.setAttribute("value", mSocketIONickname);
	settings.push_back(SocketIONickname);

	XmlTree Info("Info", "");
	Info.setAttribute("value", toString(mInfo));
	settings.push_back(Info);

	XmlTree SplitWarpH("SplitWarpH", "");
	SplitWarpH.setAttribute("value", toString(mSplitWarpH));
	settings.push_back(SplitWarpH);

	XmlTree SplitWarpV("SplitWarpV", "");
	SplitWarpV.setAttribute("value", toString(mSplitWarpV));
	settings.push_back(SplitWarpV);

	// write XML file
	settings.write(writeFile(path));

	return true;
}

bool VDSettings::restore()
{
	// check to see if Settings.xml file exists
	fs::path params = getAppPath() / settingsFileName;
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


			if (settings.hasChild("MIDIEnabled")) {
				XmlTree MIDIEnabled = settings.getChild("MIDIEnabled");
				mMIDIEnabled = MIDIEnabled.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("OSCEnabled")) {
				XmlTree OSCEnabled = settings.getChild("OSCEnabled");
				mOSCEnabled = OSCEnabled.getAttributeValue<bool>("value");
			}
			/*if (settings.hasChild("OSCReceiverPort")) {
				XmlTree OSCReceiverPort = settings.getChild("OSCReceiverPort");
				mOSCReceiverPort = OSCReceiverPort.getAttributeValue<int>("value");
			}*/
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
				mOSCDestinationHost = OSCDestinationHost.getAttributeValue<std::string>("value");
			}
			if (settings.hasChild("OSCDestinationPort2")) {
				XmlTree OSCDestinationPort2 = settings.getChild("OSCDestinationPort2");
				mOSCDestinationPort2 = OSCDestinationPort2.getAttributeValue<int>("value");
			}
			if (settings.hasChild("OSCDestinationHost2")) {
				XmlTree OSCDestinationHost2 = settings.getChild("OSCDestinationHost2");
				mOSCDestinationHost2 = OSCDestinationHost2.getAttributeValue<std::string>("value");
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
			if (settings.hasChild("AreSocketIOEnabledAtStartup")) {
				XmlTree AreSocketIOEnabledAtStartup = settings.getChild("AreSocketIOEnabledAtStartup");
				mAreSocketIOEnabledAtStartup = AreSocketIOEnabledAtStartup.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("IsRouter")) {
				XmlTree IsRouter = settings.getChild("IsRouter");
				mIsRouter = IsRouter.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("IsSocketIOServer")) {
				XmlTree IsSocketIOServer = settings.getChild("IsSocketIOServer");
				mIsSocketIOServer = IsSocketIOServer.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("SocketIOProtocol")) {
				XmlTree SocketIOProtocol = settings.getChild("SocketIOProtocol");
				mSocketIOProtocol = SocketIOProtocol.getAttributeValue<std::string>("value");
			}
			if (settings.hasChild("SocketIOHost")) {
				XmlTree SocketIOHost = settings.getChild("SocketIOHost");
				mSocketIOHost = SocketIOHost.getAttributeValue<std::string>("value");
			}
			if (settings.hasChild("SocketIOPort")) {
				XmlTree SocketIOPort = settings.getChild("SocketIOPort");
				mSocketIOPort = SocketIOPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("SocketIORoom")) {
				XmlTree SocketIORoom = settings.getChild("SocketIORoom");
				mSocketIORoom = SocketIORoom.getAttributeValue<std::string>("value");
			}
			if (settings.hasChild("SocketIONickname")) {
				XmlTree SocketIONickname = settings.getChild("SocketIONickname");
				mSocketIONickname = SocketIONickname.getAttributeValue<std::string>("value");
			}

			if (settings.hasChild("Info")) {
				XmlTree Info = settings.getChild("Info");
				mInfo = Info.getAttributeValue<std::string>("value");
			}

			if (settings.hasChild("SplitWarpH")) {
				XmlTree SplitWarpH = settings.getChild("SplitWarpH");
				mSplitWarpH = SplitWarpH.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("SplitWarpV")) {
				XmlTree SplitWarpV = settings.getChild("SplitWarpV");
				mSplitWarpV = SplitWarpV.getAttributeValue<bool>("value");
			}
			// if AutoLayout is false we have to read the custom screen layout
			if (mAutoLayout)
			{
				// init, overriden by GetWindowsResolution
				mMainWindowWidth = 1280;
				mMainWindowHeight = 720;
				mRenderWidth = 1280;
				mRenderHeight = 720;
				mRenderX = 1024;
				mRenderY = 0;
			}
			else
			{
				if (settings.hasChild("MainWindowWidth")) {
					XmlTree MainWindowWidth = settings.getChild("MainWindowWidth");
					mMainWindowWidth = MainWindowWidth.getAttributeValue<int>("value");
				}
				if (settings.hasChild("MainWindowHeight")) {
					XmlTree MainWindowHeight = settings.getChild("MainWindowHeight");
					mMainWindowHeight = MainWindowHeight.getAttributeValue<int>("value");
				}
				if (settings.hasChild("MainWindowX")) {
					XmlTree MainWindowX = settings.getChild("MainWindowX");
					mMainWindowX = MainWindowX.getAttributeValue<int>("value");
				}
				if (settings.hasChild("MainWindowY")) {
					XmlTree MainWindowY = settings.getChild("MainWindowY");
					mMainWindowY = MainWindowY.getAttributeValue<int>("value");
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

			}
			return true;
		}
	}
	else {
		// if it doesn't, return false
		return false;
	}
}

void VDSettings::resetSomeParams() {
	mCamPosXY = vec2(0.0f);
	mCount = 1;

	// EyePointZ
	defaultEyePointZ = -400.f;
	minEyePointZ = -800.0;
	maxEyePointZ = 0.0;
	tEyePointZ = autoEyePointZ = false;
	// unicorns
	iAlpha = 1.0f;
	//iSpeedMultiplier = 1.0f;// = 0.0985f;// nearly ok for 160 = 0.0985f;
	iFade = mSendToOutput = iRepeat = false;

	// transition
	iTransition = 0;
	iAnim = 0.0;
	mTransitionDuration = 2.0f;
	mTransitionTime = 1.0f;

	autoInvert = false;


	shaderEditIndex = 0;
}

void VDSettings::reset()
{
	// parameters exposed in XML
	mMIDIOpenAllInputPorts = mAutoLayout = mShowUI = mCursorVisible = true;
	mRenderThumbs = mStandalone = mSplitWarpH = mSplitWarpV = false;
	//mOutputVideoResolution = 1024;
	mInfo = "";
	//mTrackName = "";
	mMainWindowX = mRenderX = 0;
	mMainWindowY = mRenderY = 0;
	mxLeft = 0;
	mxRight = 1280;
	myLeft = 0;
	myRight = 720;
	mMainWindowWidth = 1280;
	mMainWindowHeight = 720;
	mRenderWidth = 1280;
	mRenderHeight = 720;
	// parameters not exposed in XML mRenderXY = 
	mTexMult = vec2(1.0f);
	mLeftRenderXY = mRightRenderXY = mPreviewRenderXY = mWarp1RenderXY = mWarp2RenderXY = vec2(0.0f);
	mRenderPosXY = vec2(0.0, 320.0);
	mPreviewFragXY = vec2(0.0, 0.0);


	mRenderCodeEditorXY.x = 0;
	mRenderCodeEditorXY.y = 0;
	mCodeEditorWidth = 800;
	mCodeEditorHeight = 600;
	mCurrentFilePath = "currentMix.frag";
	mMarginSmall = 2;
	mPreviewEnabled = true;
	//audio
	mIsPlaying = false;
	iSeed = 0.1;
	//liveMeter = 0.0f;

	// shader uniforms
	for (int i{ 0 }; i < 4; i++)
	{
		iChannelTime[i] = i;
	}

	iShowFps = true;

	currentSelectedIndex = 0;
	selectedWarp = 0;

	/*for (int a{0}; a < MAX; a++)
	{
		iChannels[a] = a;
	}
	iChannels[4] = 10;*///spout
	selectedChannel = 0;
	// fbo indexes for warp (should be constants)
	mFboResolution = 2048;

	mWarpCount = 3;
	FPSColor = ColorA(0.0f, 1.0f, 0.0f, 1.0f);
	ColorGreen = ColorA(0.0f, 1.0f, 0.0f, 1.0f);
	ColorRed = ColorA(1.0f, 0.0f, 0.0f, 1.0f);
	ColorYellow = ColorA(1.0f, 1.0f, 0.0f, 1.0f);
	ColorPurple = ColorA(0.5f, 0.0f, 1.0f, 1.0f);
	isUIDirty = true;
	mLiveCode = false;
	mStateTransition = 1.0;
	mOptimizeUI = false;

	// initialize our camera
	mCamEyePointXY = vec2(0.f, 0.f);
	mCamEyePointZ = -400.f;
	mCamera.setEyePoint(vec3(mCamEyePointXY.x, mCamEyePointXY.y, mCamEyePointZ));
	//mCamera.setCenterOfInterestPoint(vec3(0.f, 0.f, 0.f));

	mUIZoom = 1;

	// midi
	mMIDIEnabled = true;

	// OSC
	mOSCEnabled = false;
	mOSCDestinationHost = "127.0.0.1";
	mOSCDestinationPort = 7001;
	mOSCDestinationHost2 = "127.0.0.1";
	mOSCDestinationPort2 = 7002;
	//mOSCReceiverPort = 7000;

	mMsg = "";
	mErrorMsg = "";
	mSocketIOMsg = "";
	//mShaderMsg = "";
	mIsOSCSender = false;
	/*xFade = 1.0f;
	xFadeChanged = false;*/
	// web sockets
	mIsRouter = false;
	mAreSocketIOEnabledAtStartup = true;
	mIsSocketIOServer = false;
	mSocketIOProtocol = "http://";
	mSocketIOHost = "127.0.0.1";
	mSocketIOPort = 8088;
	mSocketIORoom = "roomtest";
	mSocketIONickname = "bruce";

	resetSomeParams();
}
