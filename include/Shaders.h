/*
Copyright (c) 2014, Paul Houx - All rights reserved.
This code is intended for use with the Cinder C++ library: http://libcinder.org

Redistribution and use in source and binary forms, with or without modification, are permitted provided that
the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and
the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Utilities.h"
#include "cinder/Timeline.h"

#include "Resources.h"
// log
#include "Logan.h"

// parameters
#include "ParameterBag.h"
// thread safe circular buffer
#include "cinder/ConcurrentCircularBuffer.h"
// Watchdog
#include "Watchdog.h"

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
		int								setGLSLString(string pixelFrag, string name);
		int								setGLSLStringAtIndex(string pixelFrag, string name, int index);
		int								setGLSLPixelShaderAtIndex(gl::GlslProgRef pixelFrag, string name, int index);
		bool							loadTextFile(string aFilePath);
		void							loadCurrentFrag();

		string							getFragError();

		bool							isValidFrag() { return validFrag; };
		bool							isValidVert() { return validVert; };

		gl::GlslProgRef					getMixShader() { return mMixShader; };
		gl::GlslProgRef					getWarpShader() { return mWarpShader; };
		gl::GlslProgRef					getLiveShader() { return mLiveShader; };
		void							setupLiveShader();
		string							loadLiveShader(string frag);
		Shada							getShader(int aIndex) { return mFragmentShaders[aIndex]; };
		int								getCount() { return mFragmentShaders.size(); };

		int								loadPixelFragmentShader(string aFilePath);
		void							loadFragmentShader(boost::filesystem::path aFilePath);
		void							removePixelFragmentShaderAtIndex(int index);
		int								loadPixelFragmentShaderAtIndex(string aFilePath, int index);
		string							getFileName(string aFilePath);
		string							getNewFragFileName(string aFilePath);
		string							getFragStringFromFile(string fileName);
		void							renderPreviewShader();

		//! Shuts down the loader thread and its associated OpenGL context.
		void							shutdownLoader();
		void							createThumbsFromDir(string filePath);
		void							setShaderMicroSeconds(int index, int micro);
		void							random();
		void							addRequest(boost::filesystem::path aFilePath, int index);
		//! Shader that will perform the transition to the next shader.
		gl::GlslProgRef					getShaderTransition() { return mShaderTransition; };
		//! Currently active shader.
		gl::GlslProgRef					getShaderCurrent() { return mShaderCurrent; };
		//! Shader that has just been loaded and we are transitioning to.
		gl::GlslProgRef					getShaderNext() { return mShaderNext; };
		// Transition is done. Swap shaders.
		void							swapShaders();
		// get the include shader
		std::string						getShaderInclude() { return shaderInclude; };

	private:
		// Logger
		LoganRef						log;

		string							mFragFile;
		string							mFragFileName;

		bool							validFrag;
		bool							validVert;
		bool							liveError;
		bool							isLiveShaderSetup;
		fs::path						liveFragFile;
		// new
		int								mCurrentRenderShader;
		int								mCurrentPreviewShader;
		int								mShaderIndex;
		//! shaders
		vector<Shada>					mFragmentShaders;

		string							fileName, previousFileName, currentFileName, mixFileName;
		string							mError;
		// parameters
		ParameterBagRef					mParameterBag;
		// current frag string
		string							currentFrag;
		//! find index for insert/update in mFragmentShaders
		int								findFragmentShaderIndex(int index, string name);
		// thread
		//! Initializes the loader thread and the shared OpenGL context.
		bool							setupLoader();
		void loader(gl::ContextRef ctx);
		//! We will use this structure to pass data from one thread to another.
		struct LoaderData {
			LoaderData() {}
			LoaderData(const fs::path& path, gl::GlslProgRef shader)
				: path(path), shader(shader)
			{
			}

			//! This constructor allows implicit conversion from path to LoaderData.
			LoaderData(const fs::path& path)
				: path(path)
			{
			}

			fs::path path;
			gl::GlslProgRef shader;
		};
		//! Shader that will perform the transition to the next shader.
		gl::GlslProgRef mShaderTransition;
		//! Currently active shader.
		gl::GlslProgRef mShaderCurrent;
		//! Shader that has just been loaded and we are transitioning to.
		gl::GlslProgRef mShaderNext;

		//! The main thread will push data to this buffer, to be picked up by the loading thread.
		ConcurrentCircularBuffer<LoaderData>* mRequests;
		//! The loading thread will push data to this buffer, to be picked up by the main thread.
		ConcurrentCircularBuffer<LoaderData>* mResponses;
		//! Our loading thread, sharing an OpenGL context with the main thread.
		std::shared_ptr<std::thread>          mThread;
		//! Signals if the loading thread should abort.
		bool                                  mThreadAbort;
		//! Keep track of the current path.
		fs::path        mPathCurrent;
		//! Keep track of the next path.
		fs::path        mPathNext;

		// default vertex shader
		std::string						mPassthruVextexShader;
		// include shader lines
		std::string						shaderInclude;
		// mix shader
		gl::GlslProgRef					mMixShader;
		// live coding shader
		gl::GlslProgRef					mLiveShader;
		//! warp shader
		gl::GlslProgRef					mWarpShader;
		//! PassThru shader
		gl::GlslProgRef					mPassThruShader;

	};
}
/*
for (auto &shader : mFragmentShaders)
{
auto map = shader.prog->getActiveUniformTypes();
if (map.find("iResolution") != map.end())
{
shader.prog->uniform("iResolution", vec3(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 0.0f));
//shader.prog->uniform("iResolution", vec3(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight, 0.0f));
//}
	}
*/