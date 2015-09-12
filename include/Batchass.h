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
#if !defined( NOWARPING )
// warps
#include "WarpWrapper.h"
#endif
// logger
#include "Logger.h"
// MessageRouter
#include "MessageRouter.h"

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
		void						shutdown();
		// TODO move to private when shadamixa upgraded
		void						createWarpFbos();
		int							getWindowsResolution();
		float						formatFloat(float f);
		void						shutdownLoader();
		void						log(string msg);
		ShadersRef					getShadersRef() { return mShaders; };
		TexturesRef					getTexturesRef() { return mTextures; };
		WarpWrapperRef				getWarpsRef() { return mWarpings; };
		bool						isEditModeEnabled() { return mWarpings->isEditModeEnabled(); };
		// animation
		// exposure
		float						defaultExposure;
		float						minExposure;
		bool						tExposure;
		bool						autoExposure;
		//zoom
		float						defaultZoom;
		float						minZoom;
		float						maxZoom;
		bool						tZoom;
		bool						autoZoom;
		//chromatic
		float						defaultChromatic;
		float						minChromatic;
		float						maxChromatic;
		bool						tChromatic;
		bool						autoChromatic;
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
		void						resetChromatic();
		void						tempoChromatic();
		void						resetRatio();
		void						tempoRatio();
		void						resetZPos();
		void						tempoZPos();
		void						resetRotationSpeed();
		void						tempoRotationSpeed();
		void						lockChromatic() { autoChromatic = !autoChromatic; };
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
		void						assignFboToWarp(int index, int fbo);
		void						assignTextureToChannel(int selectedTexture_index, int selectedChannel);
		// MIDI
		void						midiSetup();
		int							midiInCount() { return mMessageRouter->getMidiInPortsCount(); };
		string						midiInPortName(int i) { return mMessageRouter->getMidiInPortName(i); };
		bool						midiInConnected(int i) { return mMessageRouter->isMidiInConnected(i); };
		void						midiInOpenPort(int i) { mMessageRouter->openMidiInPort(i); };
		void						midiInClosePort(int i) { mMessageRouter->closeMidiInPort(i); };
		// messages
		void						sendJSON(string params);
		void						colorWrite();
		void						sendOSCIntMessage(string controlType, int iarg0 = 0, int iarg1 = 0, int iarg2 = 0, int iarg3 = 0, int iarg4 = 0, int iarg5 = 0);
		void						selectShader(bool left, int index);
		void						wsConnect();
		void						wsPing();

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
		// MessageRouter
		MessageRouterRef			mMessageRouter;
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
