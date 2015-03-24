#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Utilities.h"
#include "cinder/Timeline.h"

#include "Resources.h"
// log
#include "Logger.h"

// parameters
#include "ParameterBag.h"
// thread safe circular buffer
#include "cinder/ConcurrentCircularBuffer.h"
// Watchdog
#include "Watchdog.h"
// VisualStudio does'nt seem to support initializer_list
// yet so let's use boost::assign::list_of instead
#if defined( CINDER_MSW )
#include "boost/assign/list_of.hpp"
using namespace boost::assign;
#endif

#pragma warning(push)
#pragma warning(disable: 4996) // _CRT_SECURE_NO_WARNINGS

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta
{
	// stores the pointer to the Shaders instance
	typedef std::shared_ptr<class Shaders> ShadersRef;

	class Shaders {
	public:
		Shaders(ParameterBagRef aParameterBag);
		virtual					~Shaders();
		void update();
		static ShadersRef	create(ParameterBagRef aParameterBag)
		{
			return shared_ptr<Shaders>(new Shaders(aParameterBag));
		}

		string							getFragFileName() { return mFragFileName; };
		string							getFragFullPath() { return mFragFile; };

		void							doTransition();
		bool							setFragString(string pixelFrag);
		int								setGLSLString(string pixelFrag);
		int								setGLSLStringAtIndex(string pixelFrag, int index);
		string							getShaderName(int aIndex);
		bool							loadTextFile(string aFilePath);
		void							loadCurrentFrag();

		string							getFragError();

		bool							isValidFrag() { return validFrag; };
		bool							isValidVert() { return validVert; };

		gl::GlslProgRef					getMixShader() { return mMixShader; };
		gl::GlslProgRef					getWarpShader() { return mWarpShader; };
		gl::GlslProgRef					getLiveShader() { return mLiveShader; };
		gl::GlslProgRef					getShader(int aIndex) { return mFragmentShaders[aIndex]; };

		int								loadPixelFragmentShader(string aFilePath);
		void							loadFragmentShader(boost::filesystem::path aFilePath);
		int								loadPixelFragmentShaderAtIndex(string aFilePath, int index);
		string							getFileName(string aFilePath);
		string							getNewFragFileName(string aFilePath);
		string							getFragStringFromFile(string fileName);
		void							renderPreviewShader();
		int								getCount() { return mFragmentShaders.size(); };

		void shutdownLoader();
	private:
		// Logger
		LoggerRef					log;


		string						mFragFile;
		string						mFragFileName;

		bool						validFrag;
		bool						validVert;
		bool						liveError;
		// new
		int							mCurrentRenderShader;
		int							mCurrentPreviewShader;
		vector<gl::GlslProgRef>		mFragmentShaders;
		vector<string>				mFragmentShadersNames;

		string						fileName, previousFileName, currentFileName, mixFileName;
		string						mError;
		// parameters
		ParameterBagRef				mParameterBag;
		// current frag string
		string						currentFrag;
		// thread
		void setupLoader();
		void loader();
		struct LoaderData
		{
			LoaderData() {}
			//LoaderData(const fs::path& path, gl::GlslProgRef shader): path(path), shader(shader) {}

			//! This constructor allows implicit conversion from path to LoaderData.
			LoaderData(const fs::path& path)
				: path(path) {}

			fs::path path;
			std::string shadertext;
			//gl::GlslProgRef shader;
		};
		//! The main thread will push data to this buffer, to be picked up by the loading thread.
		ConcurrentCircularBuffer<LoaderData>* mRequests;
		//! The loading thread will push data to this buffer, to be picked up by the main thread.
		ConcurrentCircularBuffer<LoaderData>* mResponses;
		//! Our loading thread, sharing a OpenGL context with the main thread.
		std::shared_ptr<std::thread>          mThread;
		//! Signals if the loading thread should abort.
		bool                                  mThreadAbort;
		// default vertex shader
		std::string vs;
		// include shader lines
		std::string inc;
		// mix shader
		gl::GlslProgRef mMixShader;
		// live coding shader
		gl::GlslProgRef mLiveShader;
		//! warp shader
		gl::GlslProgRef				mWarpShader;

	};
}