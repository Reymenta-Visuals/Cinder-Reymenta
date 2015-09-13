#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Timeline.h"
#include "cinder/Json.h"
#include "cinder/Xml.h"
#include "cinder/gl/Fbo.h"

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
	//! struct for textures 
	struct Texta
	{
		char						name[32];
		int							sequenceIndex;
		bool						isSequence;
	};
	//! struct to keep track of the texture names for spout senders and shader fbo-rendered textures 
	struct Sender
	{
		char						SenderName[256];
		unsigned int				width, height;
		ci::gl::TextureRef			texture;
		bool						active;
	};
	/*struct ShadaFbo
	{
		ci::gl::Fbo					fbo;
		int							shadaIndex;
		bool						active;
	};*/
	struct Shada
	{
		gl::GlslProgRef				shader;
		string						name;
		bool						active;
		int							microseconds;
	};
	/*struct WarpInput
	{
		int							leftIndex;
		int							leftMode;		// 0 for input texture, 1 for shader
		int							rightIndex;
		int							rightMode;		// 0 for input texture, 1 for shader
		float						controlValues[18];		// from 0 left to 1 right
		bool						hasTexture;		// has already a texture? if not the first one is put on left and right
		bool						active;
	};*/
	struct WarpFbo
	{
		ci::gl::Fbo					fbo;
		int							textureIndex;
		int							textureMode;	// 0 for input texture, 1 for shader
		bool						active;
	};

	class ParameterBag
	{
	public:
		ParameterBag();
		static ParameterBagRef create();

		//! maximum number of fbos, shaders, textures
		static const int			MAX = 14;
		static const int			TEXTUREMODEINPUT = 0;
		static const int			TEXTUREMODESHADER = 1;

		bool						save();
		bool						restore();
		void						reset();

		std::string mParamString;
		//fonts
		ci::Font mLabelFont, mSmallLabelFont, mIconFont, mHeaderFont, mBodyFont, mFooterFont;

		// params
		int							mMode, mPreviousMode, mNewMode;
		int							mPreviewWidth, mPreviewHeight, mPreviewFboWidth, mPreviewFboHeight;
		float						mAspectRatio;
		int							mMainWindowX, mMainWindowY, mMarginSmall, mMainWindowWidth, mMainWindowHeight, mFboWidth, mFboHeight;
		float						mCodeEditorWidth, mCodeEditorHeight;
		// MIDI
		bool						mMIDIEnabled;
		// OSC
		bool						mOSCEnabled;
		std::string					mOSCDestinationHost;
		int							mOSCDestinationPort;
		std::string					mOSCDestinationHost2;
		int							mOSCDestinationPort2;
		int							mOSCReceiverPort;
		// log to console
		std::string					mMsg;
		bool						newMsg;
		std::string					InfoMsg;
		bool						mIsOSCSender;
		// render windows
		int							mRenderWidth;
		int							mRenderHeight;
		Vec2f						mRenderXY, mLeftRenderXY, mRightRenderXY, mPreviewRenderXY, mWarp1RenderXY, mWarp2RenderXY;
		Vec2f						mRenderPosXY;
		Vec2f						mPreviewFragXY;
		Vec2f						mCamPosXY;
		Vec2f						mRenderResoXY;
		bool						mAutoLayout;
		bool						mCustomLayout;
		bool						mStandalone;

		// code editor
		Vec2f						mRenderCodeEditorXY;
		bool						mLiveCode;
		bool						mShowUI;
		bool						mCursorVisible;
		bool						isUIDirty;
		bool						mMIDIOpenAllInputPorts;
		int							mCount;
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
		float						iSpeedMultiplier;        // speed multiplier
		float						iChannelTime[4];
		Vec3f						iResolution;        // viewport resolution (in pixels)
		Vec3f						iChannelResolution[4];	// channel resolution (in pixels)
		Vec4f						iMouse;             // mouse pixel coords. xy: current (if MLB down), zw: click
		bool						iFade;
		bool						iRepeat;
		bool						iLight;
		bool						iLightAuto;
		int							iBlendMode;
		float						iRedMultiplier;
		float						iGreenMultiplier;
		float						iBlueMultiplier;
		const int					maxBlendMode = 28;
		const float					maxExposure = 3.0;

		string						mAssetsPath;
		bool						iFlipHorizontally;
		bool						iFlipVertically;
		bool						iShowFps;
		bool						iDebug;
		float						iFps;
		string						sFps;
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
		bool						mRenderThumbs;
		int							currentSelectedIndex;

		// modes, should be the same in App
		static const int			MODE_MIX = 0;
		static const int			MODE_WARP = 1;
		static const int			MODE_AUDIO = 2;
		static const int			MODE_SPHERE = 3;
		static const int			MODE_MESH = 4;
		static const int			MODE_LIVE = 5;
		static const int			MODE_ABP = 6;
		static const int			MODE_VERTEXSPHERE = 7;
		static const int			MODE_KINECT = 8;
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
		bool						mIsPlaying;
		//float						mAudioMultFactor; mParameterBag->controlValues[13]
		float						iFreqs[4];
		int							iBeat;
		int							mFftSize;
		int							mWindowSize;
		float						iSeed;

		// z EyePointZ
		float						defaultEyePointZ;
		float						minEyePointZ;
		float						maxEyePointZ;
		bool						tEyePointZ;
		bool						autoEyePointZ;
		// colors
		bool						tFR, tFG, tFB, tFA, tBR, tBG, tBB, tBA;

		ci::Anim<float>				mStateTransition;

		Vec2i						mRenderResolution;        // render resolution (replaces iResolution which increments magically)
		// 
		bool						mSendToOutput;
		bool						autoInvert;
		// shader
		string						mShaderToLoad;

		// spout
		int							mFboResolution;
		int							mOutputVideoResolution;
		Vec2f						mOutputResolution;
		// OSC/MIDI/JSON controlled UI and params
		map<int, float>				controlValues;
		// indexes for textures
		map<int, int>				iChannels;
		int							selectedChannel;
		// fbo indexes for warping
		//map<int, int>				iWarpFboChannels;
		//! warp fbos
		//! warpInputs: vector of warp input textures/shader fbo texture
		//vector<WarpInput>			warpInputs;
		//void						setCrossfade(int value) { warpInputs[mParameterBag->selectedWarp].controlValues[18] = value; };

		vector<WarpFbo>				mWarpFbos;
		int							selectedWarp;
		int							mWarpCount;
		bool						mOptimizeUI;//mDirectRender, 
		int							mUIRefresh;
		int							mCurrentPreviewFboIndex;
		int							mSphereFboIndex, mMeshFboIndex, mLiveFboIndex, mMixFboIndex, mAudioFboIndex;
		int							mLeftFboIndex, mRightFboIndex, mVertexSphereFboIndex, mWarp1FboIndex, mWarp2FboIndex, mABPFboIndex;
		int							mLeftFragIndex, mRightFragIndex, mPreviewFragIndex, mPreviousFragIndex, mWarp1FragIndex, mWarp2FragIndex, mLiveFragIndex;
		float						iZoomLeft, iZoomRight;
		// meshes
		int							mMeshIndex;
		// VertexSphere
		int							mVertexSphereTextureIndex;
		// camera
		CameraPersp					mCamera;
		MayaCamUI					mMayaCam;
		Vec2f						mCamEyePointXY;
		float						mCamEyePointZ;
		// web sockets
		bool						mAreWebSocketsEnabledAtStartup;
		bool						mIsWebSocketsServer;
		uint16_t					mWebSocketsPort;
		string						mWebSocketsHost;
		//abp
		float						mBend;
		float						liveMeter;
		// info to backup in XML
		string						mInfo;
		// ableton liveOSC
		string						mTrackName;
	private:
		const string settingsFileName = "Settings.xml";
	};

}
