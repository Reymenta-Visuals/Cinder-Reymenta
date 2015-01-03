#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"
#include "cinder/Json.h"
#include "cinder/Xml.h"

// webcam
#include "cinder/Capture.h"
#include "cinder/Surface.h"
#include "cinder/MayaCamUI.h"
// fonts
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta {

	typedef std::shared_ptr<class ParameterBag> ParameterBagRef;

	class ParameterBag
	{
	public:
		ParameterBag();
		static ParameterBagRef create();

		bool save();
		bool restore();
		void reset();

		std::string mParamString;
		//fonts
		ci::Font mLabelFont, mSmallLabelFont, mIconFont, mHeaderFont, mBodyFont, mFooterFont;

		// params
		int							mMode, mPreviousMode, mNewMode;
		int							mPreviewWidth, mPreviewHeight, mPreviewFboWidth, mPreviewFboHeight;
		int							mMainWindowX, mMainWindowY, mMarginSmall, mMainWindowWidth, mMainWindowHeight, mFboWidth, mFboHeight;
		float						mCodeEditorWidth, mCodeEditorHeight;
		std::string					mOSCDestinationHost;
		int							mOSCDestinationPort;
		int							mOSCReceiverPort;
		std::string					OSCMsg;
		bool						newOSCMsg;
		std::string					InfoMsg;
		// render windows
		int							mRenderWidth;
		int							mRenderHeight;
		vec2						mRenderXY, mLeftRenderXY, mRightRenderXY, mPreviewRenderXY;
		vec2						mRenderPosXY;
		vec2						mPreviewFragXY;
		vec2						mCamPosXY;
		vec2						mRenderResoXY;
		bool						mAutoLayout;

		// code editor
		vec2						mRenderCodeEditorXY;
		bool						mShowUI;
		bool						isUIDirty;
		bool						mMIDIOpenAllInputPorts;
		int							mCount;
		float						mZPosition;
		bool						mLockFR, mLockFG, mLockFB, mLockFA, mLockBR, mLockBG, mLockBB, mLockBA;
		string						mImageFile;
		// tap tempo
		float						mTempo;
		float						iDeltaTime;
		float						iTempoTime;
		float						iTimeFactor;
		bool						mUseTimeWithTempo;
		// shader uniforms	
		float						iGlobalTime;        // shader playback time (in seconds)
		float						iChannelTime[4];
		vec3						iResolution;        // viewport resolution (in pixels)
		vec3						iChannelResolution[4];	// channel resolution (in pixels)
		vec4						iMouse;             // mouse pixel coords. xy: current (if MLB down), zw: click
		bool						iFade;
		bool						iRepeat;
		bool						iLight;
		bool						iLightAuto;
		float						iCrossfade, iPreviewCrossfade;
		bool						iShowFps;
		bool						iDebug;
		float						iFps;
		bool						iGreyScale;
		// transition
		int							iTransition;
		Anim<float>					iAnim;
		float						mTransitionDuration;
		int							multFactor;
		// windows and params
		int							mMainDisplayWidth;
		int							mMainDisplayHeight;
		int							mRenderX;
		int							mRenderY;
		int							mDisplayCount;
		bool						mPreviewEnabled;
		string						mCurrentFilePath;
		// Textures
		bool						mOriginUpperLeft;

		int							currentSelectedIndex;

		// modes, should be the same in App
		//static const int			MODE_NORMAL = 0;
		static const int			MODE_MIX = 1;
		static const int			MODE_AUDIO = 2;
		static const int			MODE_WARP = 3;
		static const int			MODE_SPHERE = 4;
		static const int			MODE_MESH = 5;

		// windows to create, should be the same in App and UI
		static const int			NONE = 0;
		static const int			RENDER_1 = 1;
		static const int			RENDER_DELETE = 5;
		static const int			MIDI_IN = 6;

		int							mWindowToCreate;
		ColorA						FPSColor;
		ColorA						ColorGreen;
		ColorA						ColorRed;
		ColorA						ColorYellow;
		ColorA						ColorPurple;
		// audio
		float						*mData;
		float						maxVolume;
		bool						mUseLineIn;
		float						mAudioMultFactor;
		float						iFreqs[4];

		// z EyePointZ
		float						defaultEyePointZ;
		float						minEyePointZ;
		float						maxEyePointZ;
		bool						tEyePointZ;
		bool						autoEyePointZ;
		// colors
		bool						tFR, tFG, tFB, tFA, tBR, tBG, tBB, tBA;

		ci::Anim<float>				mStateTransition;

		ivec2						mRenderResolution;        // render resolution (replaces iResolution which increments magically)
		// 
		bool						mSendToOutput;
		bool						autoInvert;
		// shader
		string						mShaderToLoad;

		// spout
		int							mFboResolution;
		int							mOutputVideoResolution;
		vec2						mOutputResolution;
		// OSC/MIDI/JSON controlled UI and params
		map<int, float>				controlValues;
		// indexes for textures
		map<int, int>				iChannels;
		// fbo indexes for warping
		map<int, int>				iWarpFboChannels;
		int							mCurrentShadaFboIndex;
		int							selectedWarp;
		int							mWarpCount;
		bool						mOptimizeUI;
		int							mUIRefresh;
		int							mCurrentPreviewFboIndex;
		int							mSphereFboIndex, mMeshFboIndex, mWarpFboIndex, mMixFboIndex, mAudioFboIndex, mLiveFboIndex;
		int							mLeftFboIndex, mRightFboIndex;
		int							mLeftFragIndex, mRightFragIndex, mPreviewFragIndex, mPreviousFragIndex;
		float						iZoomLeft, iZoomRight;
		// meshes
		int							mMeshIndex;
		// camera
		CameraPersp					mCamera;
		MayaCamUI					mMayaCam;
		vec2						mCamEyePointXY;
		float						mCamEyePointZ;
		//! maximum number of fbos, shaders, textures
		static const int			MAX = 16;
	private:
		const string settingsFileName = "Settings.xml";
	};

}
