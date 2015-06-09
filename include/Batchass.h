#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/ImageIo.h"
#include "Logger.h"
#include "cinder/Utilities.h"

// parameters
#include "ParameterBag.h"
// textures
#include "Textures.h"// shaders
#include "Shaders.h"
// logger
#include "Logger.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta
{
	// stores the pointer to the Reymenta instance
	typedef std::shared_ptr<class Batchass> BatchassRef;

	class Batchass {
	public:
		Batchass(ParameterBagRef aParameterBag);
		static BatchassRef	create(ParameterBagRef aParameterBag)
		{
			return shared_ptr<Batchass>(new Batchass(aParameterBag));
		}
		void						setup();
		void						getWindowsResolution();
		float						formatFloat(float f);
		// Shaders
		ShadersRef					mShaders;
		// Textures
		TexturesRef					mTextures;

	private:
		// parameters
		ParameterBagRef				mParameterBag;
		// Logger
		LoggerRef					log;
		//! fboFormat
		gl::Fbo::Format				fboFormat;
		void						createWarpFbos();

	};
}

