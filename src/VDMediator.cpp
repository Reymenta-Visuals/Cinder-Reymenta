#include "VDMediator.h"

using namespace videodromm;

VDMediatorObservableRef VDMediatorObservable::createVDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix)
{
	return VDMediatorObservableRef(new VDMediatorObservable(aVDSettings, aVDAnimation, aVDUniforms, aVDMix));
}

VDMediatorObservable::VDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix) {
	CI_LOG_V("VDMediatorObservable constructor");
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDUniforms = aVDUniforms;
	mVDMix = aVDMix;
	mOSCReceiverPort = OSC_DEFAULT_PORT;
	mWSHost = WS_DEFAULT_HOST;
	mWSPort = WS_DEFAULT_PORT;
}
VDMediatorObservableRef VDMediatorObservable::addObserver(VDUniformObserverRef o) {
	mObservers.push_back(o);
	return shared_from_this();
}
VDMediatorObservableRef VDMediatorObservable::setupOSCReceiver() {
	// Osc Receiver
	//mVDOscReceiver = VDOscReceiver::create(mVDSettings, mVDAnimation, mVDUniforms);
	//mOSCReceiverPort = OSC_DEFAULT_PORT;
	//fs::path jsonFile = getAssetPath("") / mOSCJsonFileName;
	//loadOSCReceiverFromJsonFile(jsonFile);
	//mVDOscReceiver->setupOSCReceiver(shared_from_this(), mOSCReceiverPort);
	// only save if not present saveOSCReceiverToJson();
	return shared_from_this();
}
// midi
VDMediatorObservableRef VDMediatorObservable::setupMidi() {
	// Midi in
	mVDMidi = VDMidi::create(mVDUniforms);
	mVDMidi->setupMidi(shared_from_this());
	return shared_from_this();
}
// midi
void VDMediatorObservable::midiOutSendNoteOn(int i, int channel, int pitch, int velocity) { mVDMidi->midiOutSendNoteOn(i, channel, pitch, velocity); };
int VDMediatorObservable::getMidiInPortsCount() { return mVDMidi->getMidiInPortsCount(); };
string VDMediatorObservable::getMidiInPortName(int i) { return mVDMidi->getMidiInPortName(i); };
bool VDMediatorObservable::isMidiInConnected(int i) { return mVDMidi->isMidiInConnected(i); };
void VDMediatorObservable::openMidiInPort(int i) { mVDMidi->openMidiInPort(i); };
void VDMediatorObservable::closeMidiInPort(int i) { mVDMidi->closeMidiInPort(i); };
int VDMediatorObservable::getMidiOutPortsCount() { return mVDMidi->getMidiOutPortsCount(); };
string VDMediatorObservable::getMidiOutPortName(int i) { return mVDMidi->getMidiOutPortName(i); };
bool VDMediatorObservable::isMidiOutConnected(int i) { return mVDMidi->isMidiOutConnected(i); };
void VDMediatorObservable::openMidiOutPort(int i) { mVDMidi->openMidiOutPort(i); };
void VDMediatorObservable::closeMidiOutPort(int i) { mVDMidi->closeMidiOutPort(i); };

std::string VDMediatorObservable::getMidiMsg() {
	return mVDMidi ? mVDMidi->getMidiMsg() : "Disabled";
};
void VDMediatorObservable::setPreferredMidiInputDevice(const std::string& aPreferredMidiInputDevice) {
	mPreferredMidiInputDevice = aPreferredMidiInputDevice;
}
std::string VDMediatorObservable::getPreferredMidiInputDevice() {
	return mPreferredMidiInputDevice;
}

VDMediatorObservableRef VDMediatorObservable::setupWSClient() {
	// WS Receiver
	//mVDWebsocket = VDWebsocket::create();
	mWSInstanced = true;
	mWSHost = WS_DEFAULT_HOST;
	mWSPort = WS_DEFAULT_PORT;
	fs::path jsonFile = getAssetPath("") / mWSJsonFileName;
	//loadWSFromJsonFile(jsonFile);
	//mVDWebsocket->setupWSClient(shared_from_this(), mWSHost, mWSPort);
	// only save if not present saveWSToJson();
	return shared_from_this();
}
/* void VDMediatorObservable::loadOSCReceiverFromJsonFile( const fs::path& jsonFile )
{
	if (fs::exists(jsonFile)) {
		ci::Json json( loadFile( jsonFile ) );
		if( json.contains( "receiver" ) ) {
			ci::Json u( json.getChild( "receiver" ) );
			if (validateJson(u)) {
				// (u.hasChild("port")) ? u.getValueForKey<int>("port") : OSC_DEFAULT_PORT;
				mOSCReceiverPort = u.getValueForKey<int>("port");
			}
		}
	}
	else {
		saveOSCReceiverToJson(); // create if not exists
	}
}
void VDMediatorObservable::loadWSFromJsonFile(const fs::path& jsonFile) {
	if (fs::exists(jsonFile)) {
		ci::Json json( loadFile( jsonFile ) );
		if( json.contains( "client" ) ) {
			ci::Json u( json.getChild( "client" ) );
			if (validateJson(u)) {

				mWSHost = u.getValueForKey<std::string>("host");
				mWSPort = u.getValueForKey<int>("port");
			}
		}
	}
	else {
		saveWSToJson(); // create if not exists
	}
}
// => VDJsonManager
bool VDMediatorObservable::validateJson( const ci::Json& tree )
{
	bool rtn = false;
	if( tree.contains( "port" ) ) {
		CI_LOG_W(tree.getNodeType());
		if (tree.getNodeType() == cinder::Json::NodeType::NODE_OBJECT) {//3 not 4 NODE_VALUE) {
			int p = tree.getValueForKey<int>("port");
			if (p > 0 && p < 65536) {
				rtn = true;
			}
		}
	}
	if (!rtn) {
		CI_LOG_E("json not well formatted");
	}
	return rtn;
}*/
//! to json
/*ci::Json VDMediatorObservable::saveOSCReceiverToJson() const
{
	ci::Json json;
	 Json receiver = ci::Json::makeArray( "receiver" );
	receiver.push_back( ci::Json( "port", mOSCReceiverPort ) );
	json.push_back( receiver );
	fs::path jsonFile = getAssetPath("") / mOSCJsonFileName;
	json.write(jsonFile);
	return json;
}
ci::Json VDMediatorObservable::saveWSToJson() const
{
	ci::Json json;
	Json client = ci::Json::makeArray("client");
	client.push_back( ci::Json( "host", mWSHost ) );
	client.push_back( ci::Json( "port", mWSPort ) );
	json.push_back( client );
	fs::path jsonFile = getAssetPath("") / mWSJsonFileName;
	json.write(jsonFile);
	return json;
}*/
int VDMediatorObservable::getWSClientPort() {
	return mWSPort;
}
void VDMediatorObservable::setWSClientPort(int aPort) {
	mWSPort = aPort;
}
void VDMediatorObservable::wsConnect() {
	//mVDWebsocket->wsConnect();
}
void VDMediatorObservable::update() {
	//mVDWebsocket->update();
	/*if (mVDWebsocket->hasReceivedShader()) {
		std::string receivedShader = mVDWebsocket->getReceivedShader();
		if (mVDUniforms->getUniformValue(mVDUniforms->IXFADE) < 0.5) {
			mFboShaderList[aFboShaderIndex]->setFragmentShaderString(0, receivedShader, "hydra0");
		}
		else {
			mFboShaderList[aFboShaderIndex]->setFragmentShaderString(1, receivedShader, "hydra1");
		}
		// TODO timeline().apply(&mWarps[aWarpIndex]->ABCrossfade, 0.0f, 2.0f); };
	}*/
}
bool VDMediatorObservable::setFragmentShaderString(const std::string& aFragmentShaderString, const std::string& aName, unsigned int aFboShaderIndex) {
	return mVDMix->setFragmentShaderString(aFragmentShaderString, aName, aFboShaderIndex);
}
int VDMediatorObservable::getOSCReceiverPort() {
	return mOSCReceiverPort;
};
void VDMediatorObservable::setOSCReceiverPort(int aReceiverPort) {
	mOSCReceiverPort = aReceiverPort;
};
void VDMediatorObservable::setOSCMsg(const std::string& aMsg) {
	//mVDOscReceiver->setOSCMsg(aMsg);
};
std::string VDMediatorObservable::getOSCMsg() {
	return "no osc";//mVDOscReceiver->getOSCMsg();
}

void VDMediatorObservable::setWSMsg(const std::string& aMsg) {
	//mVDWebsocket->setWSMsg(aMsg);
};
std::string VDMediatorObservable::getWSMsg() {
	return "coin";
	//mVDWebsocket->getWSMsg();
}
void VDMediatorObservable::wsPing() {
	//mVDWebsocket->wsPing();
};
VDMediatorObservableRef VDMediatorObservable::setupKeyboard() {
	// Keyboard
	mVDKeyboard = VDKeyboard::create(mVDSettings, mVDAnimation, mVDUniforms);
	mVDKeyboard->setupKeyboard(shared_from_this());
	return shared_from_this();
}
bool VDMediatorObservable::handleKeyDown(KeyEvent& event) {
	return mVDKeyboard->handleKeyDown(event);
}
bool VDMediatorObservable::handleKeyUp(KeyEvent& event) {
	return mVDKeyboard->handleKeyUp(event);
}
float VDMediatorObservable::getUniformValue(unsigned int aIndex) {
	return mVDUniforms->getUniformValue(aIndex);
}
std::string VDMediatorObservable::getUniformName(unsigned int aIndex) {
	return mVDUniforms->getUniformName(aIndex);
}
void VDMediatorObservable::setAnim(int aIndex, float aValue) {
	mVDUniforms->setAnim(aIndex, aValue);
	if (aValue == 0) {
		setUniformValue(aIndex, mVDUniforms->getDefaultUniformValue(aIndex));
	}
}
VDMediatorObservableRef VDMediatorObservable::setUniformValue(int aIndex, float aValue) {
	if (aIndex != mVDUniforms->IFPS) {
		for (auto observer : mObservers) {
			observer->setUniformValue(aIndex, aValue);
		}
		// couldn't make an observer because it's both sender and receiver
		/* 20220407 OSC routed to ws, freezes the app */
		if (mWSInstanced) {
			//mVDWebsocket->wsWrite("{\"params\" :[{ \"name\":" + toString(aIndex) + ",\"value\":" + toString(aValue) + "}]}");
		}
	}
	return shared_from_this();
};
/*VDMediatorObservableRef VDMediatorObservable::setUniformValueByName(std::string aName, float aValue) {

		for (auto observer : mObservers) {
			observer->setUniformValueByName(aName, aValue);
		}

	return shared_from_this();
};



VDMediatorObservableRef VDMediatorObservable::updateShaderText(int aIndex, float aValue) {
	for (auto observer : mObservers) {
		observer->setUniformValue(aIndex, aValue);
	}
	return shared_from_this();
};*/