#pragma once

#include "cinder/app/AppNative.h"
#include "OscListener.h"
#include "OSCSender.h"
#include "Resources.h"
#include "ParameterBag.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta 
{

	typedef std::shared_ptr<class OSC> OSCRef;

	class OSC 
	{

	public:
		OSC( ParameterBagRef aParameterBag );
		static	OSCRef create( ParameterBagRef aParameterBag );

		void	update();

		void	setupSender();
		void	sendOSCIntMessage(string controlType, int iarg0 = 0, int iarg1 = 0, int iarg2 = 0, int iarg3 = 0, int iarg4 = 0, int iarg5 = 0);
		void	sendOSCFloatMessage(string controlType, int iarg0 = 0, float farg1 = 0.0, float farg2 = 0.0, float farg3 = 0.0, float farg4 = 0.0, float farg5 = 0.0);
		void	updateAndSendOSCFloatMessage(string controlType, int iarg0 = 0, float farg1 = 0.0, float farg2 = 0.0, float farg3 = 0.0, float farg4 = 0.0, float farg5 = 0.0);
		void	sendOSCStringMessage(string controlType, int iarg0 = 0, string sarg1 = "", string sarg2 = "", string sarg3 = "", string sarg4 = "", string sarg5 = "");

		Vec4i	skeleton[20];
	private:

		// parameters
		ParameterBagRef				mParameterBag;

		osc::Listener 				mOSCReceiver;
		osc::Sender					mOSCSender;
		osc::Sender					mOSCSender2;
		void						updateParams(int iarg0, float farg1);

		static const int			MAX = 16;

		int							iargs[MAX];
		float						fargs[MAX];
		string						sargs[MAX];
		// ableton liveOSC
		string						tracks[MAX];

	};
}