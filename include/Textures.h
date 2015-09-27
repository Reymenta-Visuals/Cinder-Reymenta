#pragma once

#include <string>
#include <vector>

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
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
// quicktime
#include "cinder/qtime/QuickTime.h"

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
	// structure for loaded image files
	struct sequence {
		string						filePath;
		char						folder[32];
		string						prefix;
		string						ext;
		int							numberOfDigits;
		int							nextIndexFrameToTry;
		int							currentLoadedFrame;
		int							index;
		int							framesLoaded;
		int							playheadPosition;
		bool						loadingPaused;
		bool						loadingFilesComplete;
		bool						playing;
		float						speed;
		vector<ci::gl::TextureRef>		sequenceTextures;
	};
	class Textures {
	public:
		Textures(ParameterBagRef aParameterBag, ShadersRef aShadersRef);
		virtual					~Textures();
		static TexturesRef	create(ParameterBagRef aParameterBag, ShadersRef aShadersRef)
		{
			return shared_ptr<Textures>(new Textures(aParameterBag, aShadersRef));
		}
		//ci::gl::Texture			getFboThumb(int index);
		ci::gl::TextureRef			getWarpTexture(int index);
		WarpFbo						getWarpFbo(int index) { return mParameterBag->mWarpFbos[min((int)(mParameterBag->mWarpFbos.size() - 1), index)]; };
		void						renderWarpFbos();
		// shaders
		void						renderShadaThumbFbo();
		int							currentShadaThumbIndex;
		// textures
		void						setTexture(int index, string fileName);
		ci::gl::TextureRef			getTexture(int index);
		int							getTextureCount() { return sTextures.size(); };
		void						flipTexture(int index);
		void						setTexture(int index, ci::gl::TextureRef texture);
		char*						getTextureName(int index);
		void						setTextureName(int index, char* name);
		// from audio
		void						setAudioTexture(unsigned char *signal);
		// fbos
		ci::gl::TextureRef			getFboTexture(int index);
		GLuint						getFboTextureId(int index);
		ci::gl::FboRef				getFbo(int index);
		int							getFboCount() { return mFbos.size(); };
		int							getThumbsFboCount() { return mThumbFbos.size(); };
		void						flipFboV(int index);
		void						flipFboH(int index);
		char*						getFboName(int index);

		// shaders
		GLuint						getShaderThumbTextureId(int index);
		// spout
		void						setSenderTextureSize(int index, int width, int height);
		int							createSpoutTexture(char name[256], unsigned int width, unsigned int height);
		// image sequence
		void						createFromDir(string filePath, int index);
		void						playSequence(int textureIndex);
		void						stopSequence(int textureIndex);
		void						pauseSequence(int textureIndex);
		void						toggleLoadingFromDisk(int textureIndex);
		void						stopLoading();
		int							getPlayheadPosition(int textureIndex);
		void						setPlayheadPosition(int textureIndex, int position);

		float						getSpeed(int textureIndex);
		void						setSpeed(int textureIndex, float speed);
		void						reverseSequence(int textureIndex);
		bool						isLoadingFromDisk(int textureIndex);
		bool						isSequence(int textureIndex);
		int							getMaxFrames(int textureIndex);
		// image
		void						loadImageFile(int index, string aFile);
		// quicktime video
		void						loadMovieFile(int index, string aFile);
		void						update();
		void						draw();
		//void						createWarpInput();
		char*						getPreviewTime() { return previewTime; };
	private:
		// Logger
		LoggerRef					log;
		char						buf[32];

		//static const int			mTexturesCount = 12; //match MAX from mParameterBag

		//ci::gl::Texture				previousTexture;
		//ci::gl::Texture				currentTexture;
		unsigned char				dTexture[1024];
		vector<ci::gl::TextureRef>	sTextures;
		vector<Texta>				textas;
		char						spoutSenderName[256];
		string						fileName;
		// fbo
		vector<FrameBuffa>			mFbos;
		//! thumb fbos
		vector<FrameBuffa>			mThumbFbos;

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
		// quicktime movie
		//qtime::MovieGlRef			mMovie;
		int							mMovieIndex;
		// image sequence
		void						updateSequence(int sequenceIndex);
		void						createFromPathList(vector<string> paths);
		void						createFromTextureList(vector<ci::gl::TextureRef> textureList);
		ci::gl::TextureRef			getCurrentSequenceTexture(int sequenceIndex);
		//bool						complete;
		//bool						looping;
		int							playheadFrameInc;
		vector<sequence>			sequences;
		void						loadNextImageFromDisk(int currentSeq);
	};
}