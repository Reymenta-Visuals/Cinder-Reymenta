#include "MIDIWrapper.h"

using namespace Reymenta;

MIDI::MIDI(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;

}

MIDIRef MIDI::create(ParameterBagRef aParameterBag)
{
	return shared_ptr<MIDI>(new MIDI(aParameterBag));
}
void MIDI::shutdown() {
	mMidiIn0.closePort();
	mMidiIn1.closePort();
	mMidiIn2.closePort();
}
void MIDI::setupMidi()
{
	stringstream ss;
	ss << "setupMidi: ";

	if (mMidiIn0.getNumPorts() > 0)
	{
		mMidiIn0.listPorts();
		for (int i = 0; i < mMidiIn0.getNumPorts(); i++)
		{

			midiInput mIn;
			mIn.portName = mMidiIn0.mPortNames[i];
			mMidiInputs.push_back(mIn);
			if (mParameterBag->mMIDIOpenAllInputPorts)
			{
				if (i == 0)
				{
					mMidiIn0.openPort(i);
					mMidiIn0.midiSignal.connect(boost::bind(&MIDI::midiListener, this, boost::arg<1>::arg()));
				}
				if (i == 1)
				{
					mMidiIn1.openPort(i);
					mMidiIn1.midiSignal.connect(boost::bind(&MIDI::midiListener, this, boost::arg<1>::arg()));
				}
				if (i == 2)				{
					mMidiIn2.openPort(i);
					mMidiIn2.midiSignal.connect(boost::bind(&MIDI::midiListener, this, boost::arg<1>::arg()));
				}
				mMidiInputs[i].isConnected = true;
				ss << "Opening MIDI port " << i << " " << mMidiInputs[i].portName;
			}
			else
			{
				mMidiInputs[i].isConnected = false;
				ss << "Available MIDI port " << i << " " << mMidiIn0.mPortNames[i];
			}
		}
	}
	else
	{
		ss << "No MIDI Ports found!!!!" << std::endl;
	}
	ss << std::endl;

	mParameterBag->newMsg = true;
	mParameterBag->mMsg = ss.str();
	midiControlType = "none";
	midiControl = midiPitch = midiVelocity = midiNormalizedValue = midiValue = midiChannel = 0;
}
void MIDI::openMidiInPort(int i) {
	stringstream ss;
	if (i == 0)
	{
		mMidiIn0.openPort(i);
		mMidiIn0.midiSignal.connect(boost::bind(&MIDI::midiListener, this, boost::arg<1>::arg()));
	}
	if (i == 1)
	{
		mMidiIn1.openPort(i);
		mMidiIn1.midiSignal.connect(boost::bind(&MIDI::midiListener, this, boost::arg<1>::arg()));
	}
	if (i == 2)
	{
		mMidiIn2.openPort(i);
		mMidiIn2.midiSignal.connect(boost::bind(&MIDI::midiListener, this, boost::arg<1>::arg()));
	}
	mMidiInputs[i].isConnected = true;
	ss << "Opening MIDI port " << i << " " << mMidiInputs[i].portName << std::endl;
	mParameterBag->mMsg = ss.str();
	mParameterBag->newMsg = true;
}
void MIDI::closeMidiInPort(int i) {
	if (i == 0)
	{
		mMidiIn0.closePort();
	}
	if (i == 1)
	{
		mMidiIn1.closePort();
	}
	if (i == 2)
	{
		mMidiIn2.closePort();
	}
	mMidiInputs[i].isConnected = false;
}

void MIDI::midiListener(midi::Message msg)
{
	stringstream ss;
	midiChannel = msg.channel;
	switch (msg.status)
	{
	case MIDI_CONTROL_CHANGE:
		midiControlType = "/cc";
		midiControl = msg.control;
		midiValue = msg.value;
		midiNormalizedValue = lmap<float>(midiValue, 0.0, 127.0, 0.0, 1.0);
		if (mParameterBag->mOSCEnabled)
		{
			mOSC->updateAndSendOSCFloatMessage(midiControlType, midiControl, midiNormalizedValue, midiChannel);
		}
		mBatchass->updateParams(midiControl, midiNormalizedValue);

		//mWebSockets->write("{\"params\" :[{" + controlType);
		break;
	case MIDI_NOTE_ON:
		midiControlType = "/on";
		midiPitch = msg.pitch;
		midiVelocity = msg.velocity;
		midiNormalizedValue = lmap<float>(midiVelocity, 0.0, 127.0, 0.0, 1.0);
		break;
	case MIDI_NOTE_OFF:
		midiControlType = "/off";
		midiPitch = msg.pitch;
		midiVelocity = msg.velocity;
		midiNormalizedValue = lmap<float>(midiVelocity, 0.0, 127.0, 0.0, 1.0);
		break;
	default:
		break;
	}
	ss << "MIDI Chn: " << midiChannel << " type: " << midiControlType << " CC: " << midiControl << " Pitch: " << midiPitch << " Vel: " << midiVelocity << " Val: " << midiValue << " NVal: " << midiNormalizedValue << std::endl;
	mParameterBag->mMsg = ss.str();
	mParameterBag->newMsg = true;
}