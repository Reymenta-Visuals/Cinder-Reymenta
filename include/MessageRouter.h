#pragma once

#include "cinder/app/AppNative.h"
#include "Resources.h"
#include "ParameterBag.h"
// MIDI
#include "MIDIWrapper.h"
// OSC
#include "OSCWrapper.h"
// WebSockets
#include "WebSocketsWrapper.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta 
{

	typedef std::shared_ptr<class MessageRouter> MessageRouterRef;

	class MessageRouter
	{

	public:
		MessageRouter(ParameterBagRef aParameterBag);
		static	MessageRouterRef create(ParameterBagRef aParameterBag);
		// messages
		void						sendJSON(string params);
		void						updateParams(int iarg0, float farg1);
		void						setup(WebSocketsRef aWebSockets, OSCRef aOSC, MIDIRef aMIDI);

	private:
		// parameters
		ParameterBagRef				mParameterBag;
		// MIDI
		MIDIRef						mMIDI;
		// osc
		OSCRef						mOSC;
		// WebSockets
		WebSocketsRef				mWebSockets;

	};
}