#pragma once

#include "cinder/app/App.h"
#include "OscListener.h"
#include "OSCSender.h"
#include "Resources.h"
#include "ParameterBag.h"

using namespace ci;
using namespace ci::app;
using namespace std;
// shaders
#include "Shaders.h"
// textures
#include "Textures.h"
		static const int			MAX = 16;

namespace Reymenta 
{

	typedef std::shared_ptr<class OSC> OSCRef;

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
		// ableton liveOSC
		string						tracks[MAX];
	private:

		// parameters
		ParameterBagRef				mParameterBag;
		//! Shaders
		ShadersRef					mShaders;
		// textures
		TexturesRef					mTextures;

		osc::Listener 				mOSCReceiver;
		osc::Sender					mOSCSender;

		int							iargs[MAX];
		float						fargs[MAX];
		string						sargs[MAX];

	};
}