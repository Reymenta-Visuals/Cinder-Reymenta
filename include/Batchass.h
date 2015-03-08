#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/ImageIo.h"
#include "Logger.h"
#include "cinder/Utilities.h"

// parameters
#include "ParameterBag.h"
// shaders
#include "Shaders.h"
// textures
#include "Textures.h"
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
		// TODO move to private when shadamixa upgraded
		void						createWarpFbos();
		void						getWindowsResolution();
		float						formatFloat(float f);
		void						shutdownLoader();
	private:
		// parameters
		ParameterBagRef				mParameterBag;
		// Shaders
		ShadersRef					mShaders;
		// Textures
		TexturesRef					mTextures;
		// Logger
		LoggerRef					log;
	};
}

