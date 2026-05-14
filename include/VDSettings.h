/*
	VDSettings
	Global settings for the app
*/
// TODO switch from xml to json
// TODO remove public Si beans on laisse à public


#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Timeline.h"
#include "cinder/Xml.h"
//#include "cinder/Json.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;

namespace videodromm {

	typedef std::shared_ptr<class VDSettings> VDSettingsRef;

	class VDSettings
	{
	public:
		VDSettings(const std::string& filename);
		static VDSettingsRef create(const std::string& filename);

		enum VDAnims {
			ANIM_NONE = 0,
			ANIM_TIME = 1,
			ANIM_AUTO = 2,
			ANIM_BASS = 3,
			ANIM_MID = 4,
			ANIM_SMOOTH = 5
		};
		//const int MAX = 14;//todo remove as it is in VDUniforms.h
		bool						save();
		bool						restore();
		void						reset();
		void						resetSomeParams();
		// json
		//ci::Json					toJson(bool save = false) const;
		// params
		int							mMainWindowX, mMainWindowY, mMarginSmall, mMainWindowWidth, mMainWindowHeight;
		float						mCodeEditorWidth, mCodeEditorHeight;
		// MIDI
		bool						mMIDIEnabled;
		// OSC
		bool						mOSCEnabled;
		std::string					mOSCDestinationHost;
		uint16_t					mOSCDestinationPort;
		std::string					mOSCDestinationHost2;
		uint16_t					mOSCDestinationPort2;

		//std::string					mShaderMsg;
		bool						mIsOSCSender;
		//static const int			mMsgLength = 150;

		// render windows
		int							mRenderWidth;
		int							mRenderHeight;//mRenderXY,
		vec2						mTexMult, mLeftRenderXY, mRightRenderXY, mPreviewRenderXY, mWarp1RenderXY, mWarp2RenderXY;
		vec2						mRenderPosXY;
		vec2						mPreviewFragXY;
		vec2						mCamPosXY;
		int							mxLeft;
		int							mxRight;
		int							myLeft;
		int							myRight;
		bool						mAutoLayout;
		bool						mStandalone;

		// code editor
		vec2						mRenderCodeEditorXY;
		bool						mLiveCode;
		bool						mShowUI;
		bool						mCursorVisible;
		bool						isUIDirty;
		bool						mMIDIOpenAllInputPorts;
		int							mCount;
		std::string					mImageFile;

		// shader uniforms	
		//float						iSpeedMultiplier;   // speed multiplier 20211018 replaced by iSpeed
		float						iStart = 1.0f;		// start adjustment
		float						iBarDuration = 1.0f;// between 2 bars (seconds)
		float						iChannelTime[4];
		bool						iFade;
		bool						iRepeat;
		ci::Anim<float> 			iAlpha;
		bool						iShowFps;
		unsigned int				shaderEditIndex;
		// transition
		int							iTransition;
		Anim<float>					iAnim;
		double						mTransitionDuration;
		//! Time in seconds at which the transition to the next shader starts.
		double						mTransitionTime;

		// windows and params
		int							mRenderX;
		int							mRenderY;
		int							mDisplayCount;
		bool						mPreviewEnabled;
		std::string					mCurrentFilePath;
		// Textures
		bool						mRenderThumbs;
		int							currentSelectedIndex;


		// windows to create, should be the same in App and UI
		/*			NONE = 0;
					RENDER_1 = 1;
					RENDER_DELETE = 5;
					MIDI_IN = 6;*/

		int							mWindowToCreate;
		ColorA						FPSColor;
		ColorA						ColorGreen;
		ColorA						ColorRed;
		ColorA						ColorYellow;
		ColorA						ColorPurple;
		// audio
		bool						mIsPlaying;

		float						iSeed;

		// z EyePointZ
		float						defaultEyePointZ;
		float						minEyePointZ;
		float						maxEyePointZ;
		bool						tEyePointZ;
		bool						autoEyePointZ;

		ci::Anim<float>				mStateTransition;

		// 
		bool						mSendToOutput;
		bool						autoInvert;

		// spout
		int							mFboResolution;
		// indexes for textures
		//std::map<int, int>			iChannels;
		int							selectedChannel;
		int							selectedWarp;
		int							mWarpCount;
		bool						mOptimizeUI;
		bool						mSplitWarpH;
		bool						mSplitWarpV;
		int							mUIZoom;
		int							mCurrentPreviewFboIndex;

		// camera
		CameraPersp					mCamera;
		vec2						mCamEyePointXY;
		float						mCamEyePointZ;
		// web sockets
		bool						mIsRouter;
		bool						mAreSocketIOEnabledAtStartup;
		bool						mIsSocketIOServer;
		std::string					mSocketIOProtocol;
		std::string					mSocketIOHost;
		std::string					mSocketIORoom;
		std::string					mSocketIONickname;
		uint16_t					mSocketIOPort;
		std::string					mSocketIOMsg;

		// info to backup in XML
		std::string					mInfo;
		// ableton liveOSC
		//std::string					mTrackName;
		// messages for UI
		std::string					getMsg() { return mMsg; }
		void						setMsg(const std::string& aMsg) { mMsg = aMsg; }
		std::string					getErrorMsg() { return mErrorMsg; };
		void						setErrorMsg(const std::string& aMsg) { mErrorMsg = aMsg; }
	private:
		std::string					settingsFileName;
		std::string					mMsg;
		std::string					mErrorMsg;
	};

}