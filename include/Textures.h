#pragma once

#include <string>
#include <vector>

#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "Logger.h"
#include "cinder/Utilities.h"
#include "cinder/Filesystem.h"
#include "cinder/Capture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Camera.h"
// parameters
#include "ParameterBag.h"
// shaders
#include "Shaders.h"
// to use UI element for update background image
//#include "UIElement.h"

// gstreamer
// #include "_2RealGStreamerWrapper.h"
// WMFVideo
//#include "../../../src/ciWMFVideoPlayer.h"

using namespace ci;
using namespace ci::app;
using namespace std;
//using namespace _2RealGStreamerWrapper;

namespace Reymenta
{
	// stores the pointer to the Textures instance
	typedef std::shared_ptr<class Textures> TexturesRef;
	typedef std::chrono::high_resolution_clock Clock;

	class Textures {
	public:		
		Textures(ParameterBagRef aParameterBag, ShadersRef aShadersRef);
		virtual					~Textures();
		static TexturesRef	create(ParameterBagRef aParameterBag, ShadersRef aShadersRef)
		{
			return shared_ptr<Textures>(new Textures(aParameterBag, aShadersRef));
		}
		//ci::gl::Texture				getFboThumb(int index);
		ci::gl::Texture				getWarpTexture(int index);
		WarpFbo						getWarpFbo(int index) { return mParameterBag->mWarpFbos[min((int)(mParameterBag->mWarpFbos.size()-1), index)]; };
		void						renderWarpFbos();
		// shaders
		void						renderShadaThumbFbo();
		int							currentShadaThumbIndex;
		// textures
		void						setTexture( int index, string fileName );
		ci::gl::Texture				getTexture(int index);
		int							getTextureCount() { return sTextures.size(); };
		void						flipTexture(int index);
		void						setTexture(int index, ci::gl::Texture texture);
		// from audio
		void						setAudioTexture( unsigned char *signal );
		// fbos
		ci::gl::Texture				getFboTexture(int index);
		GLuint						getFboTextureId(int index);
		ci::gl::Fbo					getFbo(int index);
		int							getFboCount() { return mFbos.size(); };
		void						flipMixFbo(bool flip);
		void						flipFbo(int index);
		void						flipMixFboH(bool flip);
		// shaders
		GLuint						getShaderThumbTextureId(int index);
		// spout
		void						setSenderTextureSize(int index, int width, int height);
		int							createSpoutTexture(char name[256], unsigned int width, unsigned int height);

		// image
		void						loadImageFile( int index, string aFile );

		void						update();
		void						draw();
		//void						createWarpInput();
		char*						getPreviewTime() { return previewTime; };
	private:
		// Logger
		LoggerRef					log;	
		static const int			mTexturesCount = 12; //match MAX from mParameterBag

		ci::gl::Texture				previousTexture;
		ci::gl::Texture				currentTexture;
		unsigned char				dTexture[1024];
		vector<ci::gl::Texture>		sTextures;
		char						spoutSenderName[256];
		string						fileName;
		// fbo
		vector<gl::Fbo>				mFbos;
		vector<gl::Fbo>				mThumbFbos;
		//! mixes fbos
		vector<gl::Fbo>				mMixesFbos;
		//! shader fbos
		//vector<ShadaFbo>			mShadaFbos;
		// shaders
		gl::GlslProgRef				aShader;
		// parameters
		ParameterBagRef				mParameterBag;
		// Shaders
		ShadersRef					mShaders;
		// profiling
		char						previewTime[256];

	};
}