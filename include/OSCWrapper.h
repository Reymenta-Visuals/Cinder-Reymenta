#pragma once

#include "cinder/app/App.h"
#include "Osc.h"
#include "Resources.h"
#include "ParameterBag.h"
// shaders
#include "Shaders.h"
// textures
#include "Textures.h"
	
using namespace ci;
using namespace ci::app;
using namespace std;
using namespace asio;
using namespace asio::ip;
	
static const int			MAX = 16;

namespace Reymenta 
{

	typedef std::shared_ptr<class OSC> OSCRef;
	// stores the pointer to the ReceiverUdp or ReceiverTcp instance
#if USE_UDP
	typedef std::shared_ptr<class osc::ReceiverUdp> ReceiverRef;
#else
	typedef std::shared_ptr<class osc::ReceiverTcp> ReceiverRef;
#endif
	// stores the pointer to the SenderUdp instance
	typedef std::shared_ptr<class osc::SenderUdp> SenderRef;
	class OSC 
	{

	public:
		OSC(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTextures);
		static	OSCRef create(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTextures);

		void	update();

		void	setupSender();
		void	sendOSCIntMessage(string controlType, int iarg0 = 0, int iarg1 = 0, int iarg2 = 0, int iarg3 = 0, int iarg4 = 0, int iarg5 = 0);
		void	sendOSCFloatMessage(string controlType, int iarg0 = 0, float farg1 = 0.0, float farg2 = 0.0, float farg3 = 0.0, float farg4 = 0.0, float farg5 = 0.0);
		void	sendOSCStringMessage(string controlType, int iarg0 = 0, string sarg1 = "", string sarg2 = "", string sarg3 = "", string sarg4 = "", string sarg5 = "");

		ivec4	skeleton[20];
		string						getTracks() { return tracks[0]; };//TODO
	private:

		// parameters
		ParameterBagRef				mParameterBag;
		//! Shaders
		ShadersRef					mShaders;
		// textures
		TexturesRef					mTextures;

		ReceiverRef					mOSCReceiver;
		SenderRef					mOSCSender;

		int							iargs[MAX];
		float						fargs[MAX];
		string						sargs[MAX];
		// ableton liveOSC
		string						tracks[MAX];

	};
}