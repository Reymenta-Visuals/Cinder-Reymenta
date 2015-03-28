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
// warps
#include "WarpWrapper.h"
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
		void						update();
		// TODO move to private when shadamixa upgraded
		void						createWarpFbos();
		void						getWindowsResolution();
		float						formatFloat(float f);
		void						shutdownLoader();
		void						log(string msg);
		ShadersRef					getShadersRef() { return mShaders; };
		TexturesRef					getTexturesRef() { return mTextures; };
		WarpWrapperRef				getWarpsRef() { return mWarpings; };
		// animation
		// exposure
		float						defaultExposure;
		float						minExposure;
		float						maxExposure;
		bool						tExposure;
		bool						autoExposure;
		//zoom
		float						defaultZoom;
		float						minZoom;
		float						maxZoom;
		bool						tZoom;
		bool						autoZoom;
		//ratio
		float						defaultRatio;
		float						minRatio;
		float						maxRatio;
		bool						tRatio;
		bool						autoRatio;
		// z position
		float						defaultZPos;
		float						minZPos;
		float						maxZPos;
		bool						tZPos;
		bool						autoZPos;
		// Rotation Speed
		float						defaultRotationSpeed;
		float						minRotationSpeed;
		float						maxRotationSpeed;
		bool						tRotationSpeed;
		bool						autoRotationSpeed;
		// animation functions
		void						resetExposure();
		void						tempoExposure();
		void						resetZoom();
		void						tempoZoom();
		void						resetRatio();
		void						tempoRatio();
		void						resetZPos();
		void						tempoZPos();
		void						resetRotationSpeed();
		void						tempoRotationSpeed();
		void						lockRatio() { autoRatio = !autoRatio; };
		void						lockExposure() { autoExposure = !autoExposure; };
		void						lockZoom() { autoZoom = !autoZoom; };
		void						lockZPos() { autoZPos = !autoZPos; };
		void						lockRotationSpeed() { autoRotationSpeed = !autoRotationSpeed; };
		// tempo
		void						tapTempo();
		void						setTimeFactor(const int &aTimeFactor);
		void						changeMode(int newMode);
		void						createWarp();
	private:
		// parameters
		ParameterBagRef				mParameterBag;
		// Shaders
		ShadersRef					mShaders;
		// Textures
		TexturesRef					mTextures;
		// warps
		WarpWrapperRef				mWarpings;
		// Logger
		LoggerRef					mLog;
		// tempo
		void						calculateTempo();
		std::deque <double>			buffer;
		ci::Timer					timer;
		int							counter;
		double						averageTime;
		double						currentTime;
		double						startTime;
		float						previousTime;
		int							beatIndex; //0 to 3
	};
}
