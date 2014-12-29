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

	class Textures {
	public:		
		Textures(ParameterBagRef aParameterBag, ShadersRef aShadersRef);
		virtual					~Textures();
		static TexturesRef	create(ParameterBagRef aParameterBag, ShadersRef aShadersRef)
		{
			return shared_ptr<Textures>(new Textures(aParameterBag, aShadersRef));
		}
		void						setTexture( int index, string fileName );
		ci::gl::Texture				getTexture(int index);
		ci::gl::Texture				getFboTexture(int index);
		
		ci::gl::Fbo					getFbo(int index);
		int							getTextureCount() { return sTextures.size(); };
		int							getFboCount() { return mFbos.size(); };
		void						flipMixFbo(bool flip);
		// from audio
		void						setAudioTexture( unsigned char *signal );
		// image
		void						loadImageFile( int index, string aFile );

		void						update();
		void						draw();

	private:
		// Logger
		LoggerRef					log;	
		static const int			mTexturesCount = 8;

		ci::gl::Texture				previousTexture;
		ci::gl::Texture				currentTexture;
		unsigned char				dTexture[1024];
		vector<ci::gl::Texture>		sTextures;

		// fbo
		vector<gl::Fbo>				mFbos;
		// shaders
		gl::GlslProgRef				aShader;
		// parameters
		ParameterBagRef				mParameterBag;
		// Shaders
		ShadersRef					mShaders;
	};
}