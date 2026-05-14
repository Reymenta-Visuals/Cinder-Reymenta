/*
	VDMidi
*/

#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
//#include "cinder/Json.h"

// Uniforms
#include "VDUniforms.h"
// Mediator
#include "VDMediator.h"
// Midi
#include "MidiIn.h"

using namespace ci;
using namespace ci::app;

namespace videodromm
{
	class VDMediatorObservable;
	typedef std::shared_ptr<VDMediatorObservable> VDMediatorObservableRef;

	class VDMidi;
	// stores the pointer to the VDMidi instance
	typedef std::shared_ptr<VDMidi> VDMidiRef;
	struct midiInput
	{
		std::string			portName;
		bool			isConnected;
	};
	struct midiOutput
	{
		std::string			portName;
		bool			isConnected;
	};
	class VDMidi {
	public:
		VDMidi(VDUniformsRef aVDUniforms);
		static VDMidiRef			create(VDUniformsRef aVDUniforms);
		void						setupMidi(VDMediatorObservableRef aVDMediator);
		void						setMidiMsg(const std::string& aMsg);
		std::string					getMidiMsg();
		// MIDI
		void						saveMidiPorts();
		int							getMidiInPortsCount() { return mMidiInputs.size(); };
		std::string					getMidiInPortName(unsigned int i) { return (i < mMidiInputs.size()) ? mMidiInputs[i].portName : "No midi in ports"; };
		bool						isMidiInConnected(unsigned int i) { return (i < mMidiInputs.size()) ? mMidiInputs[i].isConnected : false; };
		unsigned int				getMidiOutPortsCount() { return (unsigned int)mMidiOutputs.size(); };
		std::string					getMidiOutPortName(unsigned int i) { return (i < mMidiOutputs.size()) ? mMidiOutputs[i].portName : "No midi out ports"; };
		bool						isMidiOutConnected(unsigned int i) { return (i < mMidiOutputs.size()) ? mMidiOutputs[i].isConnected : false; };
		void						midiOutSendNoteOn(int i, int channel, int pitch, int velocity);

		void						openMidiInPort(unsigned int i);
		void						closeMidiInPort(int i);
		void						openMidiOutPort(int i);
		void						closeMidiOutPort(int i);
		~VDMidi(void);
	private:
		// Uniforms
		VDUniformsRef				mVDUniforms;
		//Mediator
		VDMediatorObservableRef		mVDMediator;

		// MIDI
		std::vector<midiInput>		mMidiInputs;
		// midi inputs: couldn't make a vector
		midi::Input					mMidiIn0;
		midi::Input					mMidiIn1;
		midi::Input					mMidiIn2;
		midi::Input					mMidiIn3;
		void						midiSetup();
		void						midiListener(midi::Message msg);
		std::string					mMidiMsg;
		// midi output
		midi::MidiOut				mMidiOut0;
		midi::MidiOut				mMidiOut1;
		midi::MidiOut				mMidiOut2;
		midi::MidiOut				mMidiOut3;
		std::vector<midiOutput>		mMidiOutputs;
		std::string					midiControlType;
		int							midiControl;
		int							midiPitch;
		int							midiVelocity;
		float						midiNormalizedValue;
		int							midiValue;
		int							midiChannel;
		// midimix solo mode
		bool						midiSticky;
		bool						midiStickyPrevValue;
		int							midiStickyPrevIndex;
		// toggles 
		bool						midiWeights = true; // nano mix weights

	};
}

