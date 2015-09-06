#pragma once

#include "cinder/app/AppNative.h"
#include "Resources.h"
#include "ParameterBag.h"
// midi
#include "MidiIn.h"
#include "MidiMessage.h"
#include "MidiConstants.h"
// MessageRouter
#include "MessageRouter.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta 
{

	typedef std::shared_ptr<class MIDI> MIDIRef;
	struct midiInput
	{
		string			portName;
		bool			isConnected;
	};

	class MIDI 
	{

	public:
		MIDI(ParameterBagRef aParameterBag, MessageRouterRef aMessageRouter);
		static	MIDIRef create(ParameterBagRef aParameterBag, MessageRouterRef aMessageRouter);
		void						setup();
		int							getMidiInPortsCount() { return mMidiInputs.size(); };
		string						getMidiInPortName(int i) { return (i<mMidiInputs.size()) ? mMidiInputs[i].portName : "No midi in ports"; };
		bool						isMidiInConnected(int i) { return (i<mMidiInputs.size()) ? mMidiInputs[i].isConnected : false; };
		void						openMidiInPort(int i);
		void						closeMidiInPort(int i);
		void						shutdown();

	private:
		// parameters
		ParameterBagRef				mParameterBag;
		// MessageRouter
		MessageRouterRef			mMessageRouter;
		// midi
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

	};
}