/*
	VDFboShader
	Handles the frame buffer objects to draw into, from shaders, images, video.
*/
// TODO avoid duplicate get(Float/Int/Bool)UniformValueBy(Name/Index) which exist in VDAnimation too
// TODO recycle using object pool

#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Xml.h"
//#include "cinder/Json.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "cinder/Timeline.h"
#include "cinder/ImageIo.h"
#include "cinder/CameraUi.h"

// textures
//#include "VDTexture.h"
//! Uniforms
#include "VDUniforms.h"
//! Animation
#include "VDAnimation.h"
// Params
#include "VDParams.h"
// video
// #include "ciWMFVideoPlayer.h"
// Spout
#include "CiSpoutIn.h"

#include <atomic>
#include <vector>

using namespace ci;
using namespace ci::app;

namespace videodromm
{
	// stores the pointer to the VDFboShader instance
	typedef std::shared_ptr<class VDFboShader> 		VDFboShaderRef;
	typedef std::vector<VDFboShaderRef>				VDFboShaderList;
	// for profiling
	typedef std::chrono::high_resolution_clock		Clock;
	struct VDTextureStruct
	{
		std::string						name;
		unsigned int					ms;
		gl::TextureRef					texture;
		bool							isValid;
	};
	class VDFboShader {
	public:
		VDFboShader(VDUniformsRef aVDUniforms, VDAnimationRef aVDAnimation, unsigned int aFboIndex, const std::string& aAssetsPath);
		~VDFboShader(void);
		static VDFboShaderRef create(VDUniformsRef aVDUniforms, VDAnimationRef aVDAnimation, unsigned int aFboIndex, const std::string& aAssetsPath)
		{
			return std::make_shared<VDFboShader>(aVDUniforms, aVDAnimation, aFboIndex, aAssetsPath);
		}

		ci::gl::Texture2dRef					getTexture(); //TODO 20200610; = 0
		ci::gl::Texture2dRef					getRenderedTexture();
		void									saveThumbnail(bool overwrite = true);
		bool									isValid();
		std::string								getShaderName();
		std::vector<ci::gl::GlslProg::Uniform>	getUniforms();
		//new 
		bool									setFragmentShaderString(const std::string& aFragmentShaderString, const std::string& aName = "");
		float									getUniformValueByLocation(unsigned int aLocationIndex) {
			return mUniformValueByLocation[aLocationIndex];
		};
		void									setUniformValueByLocation(unsigned int aLocationIndex, float aValue) {
			mUniformValueByLocation[aLocationIndex] = aValue;
		};

		/* 20220101 hydra check
		void									setInputTextureRefByIndex(unsigned int aTexIndex, ci::gl::Texture2dRef aTextureRef) {
			mInputTextureList[aTexIndex].texture = aTextureRef;
			mInputTextureList[aTexIndex].name = "setInputTextureRefByIndex " + toString(aTexIndex);
		};*/
		void									setFboTextureAudioMode() {
			mCurrentFilename = mTextureName = mVDAnimation->getAudioTextureName();// "audio";
			mTextureMode = VDTextureMode::AUDIO;
			mInputTextureList[0].texture = mVDAnimation->getAudioTexture();
			mInputTextureList[0].name = mVDAnimation->getAudioTextureName();
			mInputTextureList[0].ms = 0;
			mInputTextureList[0].isValid = true;
		}
		ci::gl::Texture2dRef					getFboInputTextureListItem(unsigned int aTexIndex) {
			return mInputTextureList[aTexIndex].texture;
		};
		std::string								getTextureName(unsigned int aTexIndex = 0) {
			/* 20220101 hydra checkif (mIsHydraTex) {
				return "fbotex" + mInputTextureIndex;
			}
			else {*/
			return mInputTextureList[aTexIndex].name;
			//}
		};
		unsigned int							getFboMs(unsigned int aTexIndex = 0) {
			return mInputTextureList[aTexIndex].ms;
		};
		unsigned int							getFboMsTotal() {
			return msTotal;
		};
		// full path (dnd)
		void									loadImageFile(const std::string& aFile, unsigned int aCurrentIndex = 0);
		unsigned int							getInputTexturesCount() {
			return mInputTextureList.size();
		}
		std::string								getFboMsg() {
			return mFboMsg;
		};
		std::string								getFboError() {
			return mFboError;
		};
		std::string								getFboStatus() {
			return mFboStatus;
		};
		bool									loadFragmentShaderFromFile(const string& aFileOrPath, bool isAudio = false);
		/*void									setInputTextureIndex(unsigned int aTexIndex = 0) {
			mInputTextureIndex = getValidTexIndex(aTexIndex);
		};
		unsigned int							getInputTextureIndex() {
			return mInputTextureIndex;
		};*/
		bool									isHydraTex() {
			return mIsHydraTex;
		}
		ci::gl::Texture2dRef					getInputTexture(unsigned int aTexIndex = 0) {
			//return (mtexMode == 8 && aTexIndex < getInputTexturesCount()) ? mInputTextureList[aTexIndex] : mInputTextureRef;
			//CI_LOG_E("getInputTexture " << aTexIndex << " size " << mInputTextureList.size());

			return mInputTextureList[getValidTexIndex(aTexIndex)].texture;
		};
		bool									isValidInputTexture(unsigned int aTexIndex) {
			return mInputTextureList[getValidTexIndex(aTexIndex)].isValid;
		};
		int										getInputTextureWidth() {
			return mInputTextureList[0].texture->getWidth();
		};
		int										getInputTextureHeight() {
			return mInputTextureList[0].texture->getHeight();
		};
		void									selectSenderPanel() {
			mSpoutIn.getSpoutReceiver().SelectSenderPanel();
		}
		int										getInputTextureMode() {
			return mTextureMode;
		}
		bool handleMouseDown(MouseEvent event)
		{
			bool handled = false;
			mx = event.getPos().x;
			my = event.getPos().y;
			//mCamUi.mouseDown(event);
			event.setHandled(handled);
			return event.isHandled();
		}
		bool handleMouseDrag(MouseEvent event)
		{
			//mCamUi.mouseDrag(event);
			return true;
		}

	private:
		// Params
		VDParamsRef						mVDParams;
		// Animation
		const VDAnimationRef			mVDAnimation;
		// uniforms
		VDUniformsRef					mVDUniforms;

		//! Input textures
		std::string						mCurrentFilename;
		std::map<unsigned int, VDTextureStruct>		mInputTextureList;
		unsigned int					mCacheImageIndex = 0;
		SpoutIn							mSpoutIn;
		//unsigned int					mInputTextureIndex;
		unsigned int createInputTexture();
		bool							mLoadTopDown = false;
		// 20211115
		int								mCurrentImageSequenceIndex = 0;
		// 20211107
		string							mTextureName = "";
		//string							mStatus = "";
		string							mTypestr = "";
		string							mExt = "jpg";
		int								mTextureMode = VDTextureMode::UNKNOWN;
		int								mTextureCount = 1;
		bool							mPreloadTextures = false;
		unsigned int					msTotal = 0;
		int								dotIndex = std::string::npos;
		int								colonIndex = std::string::npos;
		// hydra
		bool							mIsHydraTex = false;

		fs::path						texFileOrPath;
		bool							fileExists;
		//! shader
		gl::GlslProgRef					mShader;
		std::vector<ci::gl::GlslProg::Uniform> mUniforms;
		std::map<unsigned int, float>	mUniformValueByLocation;
		std::string						mShaderName = "";
		std::string						mName = "";
		std::string						mShaderFileName = "";
		fs::path						mFragFilePath = "";
		bool							loadFragmentStringFromFile();
		std::string						mShaderFragmentString;

		std::string						mFileNameWithExtension;

		// include shader lines
		std::string						shaderInclude;
		bool							mValid = false;
		//! Fbo
		gl::FboRef						mFbo;

		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;
		bool							isReady;
		ci::gl::Texture2dRef			mRenderedTexture;
		ci::gl::Texture2dRef			getFboTexture();
		void							loadNextTexture(unsigned int aCurrentIndex);
		// messages
		static const int				mFboMsgLength = 150;
		std::string						mFboMsg;
		std::string						mFboError = "";
		std::string						mFboStatus = "";
		std::string						mAssetsPath = "";
		unsigned int					mFboIndex = 0;
		// video
		// ciWMFVideoPlayer				mVideo;
		float							mVideoPos = 0.0f;
		float							mVideoDuration = 0.0f;
		bool							mIsVideoLoaded = false;
		// mouse
		float mx = 0.0f;
		float my = 0.0f;
		unsigned int					getValidTexIndex(unsigned int aTexIndex) {
			return math<unsigned int>::min(aTexIndex, (unsigned int)mInputTextureList.size() - 1);
		}
	};
}
