/*
	VDMediator
	Mediator observable
*/
// TODO 

#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"

// Uniforms
#include "VDUniforms.h"
// FboShader
#include "VDFboShader.h"
// Keyboard
#include "VDKeyboard.h"
// Osc
// #include "VDOscReceiver.h"
// Midi
#include "VDMidi.h"
// Mix
#include "VDMix.h"

#include <memory>
#include <vector>

#define OSC_DEFAULT_PORT 2346
#define WS_DEFAULT_HOST "127.0.0.1"
#define WS_DEFAULT_PORT 8088

using namespace ci;
using namespace ci::app;

namespace videodromm {

	//class VDOscReceiver;
	//typedef std::shared_ptr<VDOscReceiver> VDOscReceiverRef;

	class VDMidi;
	typedef std::shared_ptr<VDMidi> VDMidiRef;

	class VDWebsocket;
	typedef std::shared_ptr<VDWebsocket> VDWebsocketRef;

	class VDKeyboard;
	typedef std::shared_ptr<VDKeyboard> VDKeyboardRef;


	class VDUniformObserver;
	typedef std::shared_ptr<class VDUniformObserver> VDUniformObserverRef;

	class VDUniformObserver : public std::enable_shared_from_this<VDUniformObserver> {
	public:
		virtual VDUniformObserverRef setUniformValue(int aIndex, float aValue) = 0;
		/*VDUniformObserverRef getInstance() const {return std::shared_from_this();}*/
	private:
	};
	class VDMediatorObservable;
	typedef std::shared_ptr<class VDMediatorObservable> VDMediatorObservableRef;
	class VDMediatorObservable : public std::enable_shared_from_this<VDMediatorObservable> {
	public:
		static VDMediatorObservableRef		createVDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix);
		VDMediatorObservableRef				addObserver(VDUniformObserverRef o);
		// osc
		VDMediatorObservableRef				setupOSCReceiver();
		int									getOSCReceiverPort();
		void								setOSCReceiverPort(int aReceiverPort);
		void								setOSCMsg(const std::string& aMsg);
		std::string							getOSCMsg();
		// midi
		VDMediatorObservableRef				setupMidi();
		void								midiOutSendNoteOn(int i, int channel, int pitch, int velocity);
		int									getMidiInPortsCount();
		string								getMidiInPortName(int i);
		bool								isMidiInConnected(int i);
		std::string							getMidiMsg();
		void								openMidiInPort(int i);
		void								closeMidiInPort(int i);
		int									getMidiOutPortsCount();
		string								getMidiOutPortName(int i);
		bool								isMidiOutConnected(int i);
		void								openMidiOutPort(int i);
		void								closeMidiOutPort(int i);
		void								setPreferredMidiInputDevice(const std::string& aDevice);
		std::string							getPreferredMidiInputDevice();
		// ws
		VDMediatorObservableRef				setupWSClient();
		int									getWSClientPort();
		void								setWSClientPort(int aPort);
		void								setWSMsg(const std::string& aMsg);
		std::string							getWSMsg();
		void								wsConnect();
		void								wsPing();
		void								update();

		VDMediatorObservableRef				setupKeyboard();
		float								getUniformValue(unsigned int aIndex);
		std::string							getUniformName(unsigned int aIndex);
		VDMediatorObservableRef				setUniformValue(int aIndex, float aValue);
		//VDMediatorObservableRef				setUniformValueByName(std::string aName, float aValue);
		//VDMediatorObservableRef				updateShaderText(int aIndex, float aValue);
		void								setAnim(int aIndex, float aValue);
		bool								handleKeyDown(KeyEvent& event);
		bool								handleKeyUp(KeyEvent& event);

		int									getErrorCode() { return mVDUniforms->getErrorCode(); }
		bool								setFragmentShaderString(const std::string& aFragmentShaderString, const std::string& aName = "", unsigned int aFboShaderIndex = 0);
	private:
		std::vector<VDUniformObserverRef>	mObservers;
		// Settings
		VDSettingsRef						mVDSettings;
		// Animation
		VDAnimationRef						mVDAnimation;
		// Uniforms
		VDUniformsRef						mVDUniforms;
		// OSC
		//VDOscReceiverRef					mVDOscReceiver;
		// Midi
		VDMidiRef							mVDMidi;
		std::string							mPreferredMidiInputDevice = "none";
		// Websockets
		VDWebsocketRef						mVDWebsocket;
		// Mix
		VDMixRef							mVDMix;
		//! OSC
		//bool								validateJson( const ci::Json& tree );
		int									mOSCReceiverPort;
		const std::string					mOSCJsonFileName = "oscreceiver.json";
		//void								loadOSCReceiverFromJsonFile(const fs::path& jsonFile);
		//ci::Json							saveOSCReceiverToJson() const;
		//! WS
		std::string							mWSHost;
		int									mWSPort;
		const std::string					mWSJsonFileName = "wsclient.json";
		//void								loadWSFromJsonFile(const fs::path& jsonFile);
		//ci::Json							saveWSToJson() const;
		bool								mWSInstanced = false;
		//std::string							mShaderLeft;
		//std::string							mShaderRight;
		//void								setFragmentShaderString(unsigned int aFboShaderIndex, const std::string& aFragmentShaderString, const std::string& aName);
		// Keyboard
		VDKeyboardRef						mVDKeyboard;
		//VDMediatorObservable() {}
		VDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix);


	};

}
/*void update(void (*callback) (VDUniformObserverRef, map<string, any::type>)) {
	for (VDUniformObserverRef observer : mObservers) {
		callback(observer, params);
	}
};*/
/*VDMediatorObservableRef mVDMediator = VDMediatorObservable::create()->addObserver(VDSocketIOObserverBuilder->connect(ip, port))->addObserver(new UIDisplay());

mVDMediator->setUniformValue(a, b);
mVDMediator->update([](observer, { "a": 1, "b" : 2 }) -> {
	observer->setUniformValue(a, b);
});

VDProxy::connect(ip, port)

*/
