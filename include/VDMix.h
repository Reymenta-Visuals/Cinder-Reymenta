#pragma once
/*
	VDMix
	Handles mixing shaders.
	Maintains a Fbo list
	Outputs severals Fbos depending on the context: mix 2 shaders or several with weights
*/

#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
//#include "cinder/Json.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "cinder/Timeline.h"

// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// Uniforms
#include "VDUniforms.h"
// Fbos
#include "VDFboShader.h"
// Params
#include "VDParams.h"

// Syphon
#if defined( CINDER_MAC )
#include "cinderSyphon.h"
#endif

#include <atomic>
#include <vector>

using namespace ci;
using namespace ci::app;

using namespace videodromm;

namespace videodromm
{
	// stores the pointer to the VDMix instance
	typedef std::shared_ptr<class VDMix> 	VDMixRef;

	class VDMix {
	public:
		VDMix(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms);
		static VDMixRef					create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms)
		{
			return std::shared_ptr<VDMix>(new VDMix(aVDSettings, aVDAnimation, aVDUniforms));
		}

		// fbolist
		unsigned int					getFboShaderListSize() {
			return (unsigned int)mFboShaderList.size();
		};
		bool							isFboValid(unsigned int aFboIndex) {
			bool valid = false;
			if (mFboShaderList.size() > 0) {
				valid = mFboShaderList[getValidFboIndex(aFboIndex)]->isValid();
			}
			return valid;

		};
		std::string						getFboMsg(unsigned int aFboIndex) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getFboMsg();
		};
		std::string						getFboError(unsigned int aFboIndex) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getFboError();
		};
		std::string						getFboStatus(unsigned int aFboIndex) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getFboStatus();
		};
		bool							isValidInputTexture(unsigned int aTexIndex = 0) {
			return mFboShaderList[getValidFboIndex(mSelectedFbo)]->isValidInputTexture(aTexIndex);
		};
		unsigned int					getFboMs(unsigned int aTexIndex = 0) {
			return mFboShaderList[getValidFboIndex(mSelectedFbo)]->getFboMs(aTexIndex);
		};
		unsigned int					getFboMsTotal(unsigned int aFboIndex = 0) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getFboMsTotal();
		};

		std::string						getAssetsPath() {
			return mAssetsPath;
		};
		unsigned int findAvailableIndex(unsigned int aFboShaderIndex);
		bool							setFragmentShaderString(const string& aFragmentShaderString, const std::string& aName = "", unsigned int aFboShaderIndex = 0);

		int								loadFragmentShader(const std::string& aFilePath, unsigned int aFboShaderIndex);
		std::vector<ci::gl::GlslProg::Uniform>			getUniforms(unsigned int aFboIndex = 0) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getUniforms();
		}

		void							setSelectedFbo(unsigned int aFboIndex = 0) {
			mSelectedFbo = getValidFboIndex(aFboIndex);
		}
		unsigned int					getSelectedFbo() {
			return mSelectedFbo;
		};
		/*void							setFboInputTexture(unsigned int aFboIndex = 0, unsigned int aTexIndex = 0) {
			if (mFboShaderList.size() > 0) {
				mFboShaderList[getValidFboIndex(aFboIndex)]->setInputTextureIndex(aTexIndex);
				// TODO 20211227 check if useless now mFboShaderList[getValidFboIndex(aFboIndex)]->setInputTextureRef(mTextureList[texIndex]->getTexture());
			}
		}*/

		ci::gl::Texture2dRef			getFboInputTexture(unsigned int aTexIndex = 0) {
			return mFboShaderList[getValidFboIndex(mSelectedFbo)]->getInputTexture(aTexIndex);
		}
		std::string						getInputTextureName(unsigned int aTexIndex = 0) {
			return mFboShaderList[getValidFboIndex(mSelectedFbo)]->getTextureName(aTexIndex);
		}
		std::string						getFboTextureName(unsigned int aFboIndex) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getTextureName(); // useless or duplic
		};

		int								getFboInputTextureWidth(unsigned int aFboIndex) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getInputTextureWidth();
		};
		int								getFboInputTextureHeight(unsigned int aFboIndex) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getInputTextureHeight();
		};

		void							setFboTextureAudioMode(unsigned int aFboIndex) {
			mFboShaderList[getValidFboIndex(aFboIndex)]->setFboTextureAudioMode();
		}
		void							saveThumbnail(unsigned int aFboIndex) {
			mFboShaderList[getValidFboIndex(aFboIndex)]->saveThumbnail();
		}
		ci::gl::Texture2dRef			getFboInputTextureListItem(unsigned int aFboIndex, unsigned int aTexIndex) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getFboInputTextureListItem(aTexIndex);
		}
		/*unsigned int					getFboInputTextureIndex(unsigned int aFboIndex) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getInputTextureIndex();
		}*/
		unsigned int					getInputTexturesCount(unsigned int aFboIndex = 0) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getInputTexturesCount();
		}

		std::string						getFboShaderName(unsigned int aFboIndex) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getShaderName();
		};
		void							loadImageFile(const std::string& aFile, unsigned int aFboIndex = 0);
		void							loadVideoFile(const std::string& aFile, unsigned int aFboIndex = 0);

		std::vector<ci::gl::GlslProg::Uniform>	getFboShaderUniforms(unsigned int aFboShaderIndex);
		float							getUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex);
		void							setUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex, float aValue);

		unsigned int					createFboShaderTexture(unsigned int aFboIndex = 0, const std::string& aFolder = "");
		void							clearFboShaderList() {
			mFboShaderList.clear();
		}
		ci::gl::TextureRef				getFboRenderedTexture(unsigned int aFboIndex) {
			if (mFboShaderList.size() == 0) return mDefaultTexture;
			/* 20220101 hydra check

			if (aFboIndex > mFboShaderList.size() - 1) aFboIndex = 0;

			if (mFboShaderList[aFboIndex]->isHydraTex()) {
				// fbo as inputTexture
				for (unsigned int i = 0; i < 4; i++)
				{
					mFboShaderList[aFboIndex]->setInputTextureRefByIndex(i, mFboShaderList[getValidFboIndex(aFboIndex)]->getTexture());
				}
			}
			else {
				// 20211227 useless? mFboShaderList[aFboIndex]->setInputTextureRef(mFboShaderList[aFboIndex]->getTexture());
			}
			return mFboShaderList[aFboIndex]->getRenderedTexture();
			*/
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getRenderedTexture();

		}
		ci::gl::TextureRef				getFboTexture(unsigned int aFboIndex) {
			if (mFboShaderList.size() == 0) return mDefaultTexture;
			if (aFboIndex > mFboShaderList.size() - 1) aFboIndex = 0;
			return mFboShaderList[aFboIndex]->getTexture();

		}






		bool handleMouseDown(MouseEvent event)
		{
			bool handled = false;
			if (mFboShaderList.size() > 0) {
				for (unsigned int i = 0; i < mFboShaderList.size() - 1; i++)
				{
					if (mFboShaderList[i]->handleMouseDown(event)) handled = true;// event.getPos()
				}
			}
			event.setHandled(handled);
			return event.isHandled();
		}
		bool handleMouseDrag(MouseEvent event)
		{
			if (mFboShaderList.size() == 0) return false;
			for (unsigned int i = 0; i < mFboShaderList.size() - 1; i++)
			{
				mFboShaderList[i]->handleMouseDrag(event);
			}
			return true;
		}
		ci::gl::TextureRef				getMixetteTexture(unsigned int aFboIndex);
		ci::gl::TextureRef				getRenderedMixetteTexture(unsigned int aFboIndex) { return mMixetteTexture; };
		void							selectSenderPanel() {
			if (mFboShaderList.size() == 0) return;
			mFboShaderList[0]->selectSenderPanel();
			CI_LOG_E("selectSenderPanel " << (unsigned int)mFboShaderList.size());
			for (size_t i{ 0 }; i < mFboShaderList.size() - 1; i++)
			{
				if (mFboShaderList[i]->getInputTextureMode() == VDTextureMode::SHARED) {
					mFboShaderList[i]->selectSenderPanel();
				}

			}
		};
		void							restore(const fs::path& aFilePath);
	private:
		// Params
		VDParamsRef						mVDParams;
		// Animation
		VDAnimationRef					mVDAnimation;
		// Settings
		VDSettingsRef					mVDSettings;
		// Uniforms
		VDUniformsRef					mVDUniforms;

		//! FboShaders
		VDFboShaderRef					mMixFboShader;
		VDFboShaderRef					mFboShader;
		// maintain a list of fbos specific to this mix
		VDFboShaderList					mFboShaderList;
		unsigned int					mSelectedFbo = 0;
		// textures
		bool							save();
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;
		//! mixette
		gl::FboRef						mMixetteFbo;
		gl::GlslProgRef					mGlslMixette;
		ci::gl::Texture2dRef			mMixetteTexture;
		std::string						mError;
		const unsigned int				MAXSHADERS = 8;
		std::string						mAssetsPath = "";
		fs::path						mixPath;
		unsigned int					mCurrentSecond = 0;
		unsigned int					mCurrentIndex = 0;
		unsigned int					getValidFboIndex(unsigned int aFboIndex);
		ci::gl::Texture2dRef			mDefaultTexture; //in case no fbos
	};
}
