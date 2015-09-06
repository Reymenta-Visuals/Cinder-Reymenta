#pragma once

#include "cinder/app/AppNative.h"
#include "Resources.h"
#include "ParameterBag.h"
#include "cinder/Json.h"
// MIDI
#include "MidiIn.h"
#include "MidiMessage.h"
#include "MidiConstants.h"

// OSC
#include "OscListener.h"
#include "OSCSender.h"

// WebSockets
#include "WebSocketClient.h"
#include "WebSocketServer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta 
{

	typedef std::shared_ptr<class MessageRouter> MessageRouterRef;
	typedef std::shared_ptr<class MIDI> MIDIRef;
	struct midiInput
	{
		string			portName;
		bool			isConnected;
	};
	class MessageRouter
	{

	public:
		MessageRouter(ParameterBagRef aParameterBag);
		static	MessageRouterRef create(ParameterBagRef aParameterBag);
		void						update();
		// messages
		void						sendJSON(string params);
		void						updateParams(int iarg0, float farg1);
		// MIDI
		void						midiSetup();
		int							getMidiInPortsCount() { return mMidiInputs.size(); };
		string						getMidiInPortName(int i) { return (i<mMidiInputs.size()) ? mMidiInputs[i].portName : "No midi in ports"; };
		bool						isMidiInConnected(int i) { return (i<mMidiInputs.size()) ? mMidiInputs[i].isConnected : false; };
		void						openMidiInPort(int i);
		void						closeMidiInPort(int i);
		void						shutdown();
		// OSC
		void						setupOSCSender();
		void						sendOSCIntMessage(string controlType, int iarg0 = 0, int iarg1 = 0, int iarg2 = 0, int iarg3 = 0, int iarg4 = 0, int iarg5 = 0);
		void						sendOSCColorMessage(string controlType, float val);
		void						sendOSCFloatMessage(string controlType, int iarg0 = 0, float farg1 = 0.0, float farg2 = 0.0, float farg3 = 0.0, float farg4 = 0.0, float farg5 = 0.0);
		void						updateAndSendOSCFloatMessage(string controlType, int iarg0 = 0, float farg1 = 0.0, float farg2 = 0.0, float farg3 = 0.0, float farg4 = 0.0, float farg5 = 0.0);
		void						sendOSCStringMessage(string controlType, int iarg0 = 0, string sarg1 = "", string sarg2 = "", string sarg3 = "", string sarg4 = "", string sarg5 = "");
		Vec4i						skeleton[20];

		// WebSockets
		void						wsWrite(std::string msg);
		void						wsConnect();
		void						wsPing();
		bool						isWsClientConnected() { return clientConnected; };
		void						colorWrite();

	private:
		// parameters
		ParameterBagRef				mParameterBag;
		// MIDI
		vector<midiInput>			mMidiInputs;
		void						midiListener(midi::Message msg);
		string						midiControlType;
		int							midiControl;
		int							midiPitch;
		int							midiVelocity;
		float						midiNormalizedValue;
		int							midiValue;
		int							midiChannel;
		// midi inputs: couldn't make a vector
		midi::Input					mMidiIn0;
		midi::Input					mMidiIn1;
		midi::Input					mMidiIn2;
		midi::Input					mMidiIn3;		
		// WebSockets
		// Web socket client
		void						wsClientDisconnect();
		WebSocketClient				mClient;
		void						wsClientConnect();
		void						onWsConnect();
		void						onWsDisconnect();
		void						onWsError(std::string err);
		void						onWsInterrupt();
		void						onWsPing(std::string msg);
		void						onWsRead(std::string msg);
		bool						clientConnected;
		// Web socket  server
		WebSocketServer				mServer;
		void						serverConnect();
		void						serverDisconnect();
		double						mPingTime;
		// osc
		osc::Listener 				mOSCReceiver;
		osc::Sender					mOSCSender;
		osc::Sender					mOSCSender2;
		static const int			MAX = 16;
		int							iargs[MAX];
		float						fargs[MAX];
		string						sargs[MAX];
		// ableton liveOSC
		string						tracks[MAX];
	};
}