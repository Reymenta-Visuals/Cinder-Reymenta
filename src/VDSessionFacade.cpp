#include "VDSessionFacade.h"

using namespace videodromm;

VDSessionFacadeRef VDSessionFacade::createVDSession(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix)
{
	VDMediatorObservableRef mediator =
		VDMediatorObservable::createVDMediatorObservable(aVDSettings, aVDAnimation, aVDUniforms, aVDMix);
	// OK ->addObserver(VDSocketIOObserver::connect(aVDSettings->mSocketIOHost, aVDSettings->mSocketIOPort))
	// OK ->addObserver(VDOscObserver::connect(aVDSettings->mOSCDestinationHost, aVDSettings->mOSCDestinationPort));
	// OK ->addObserver(VDUIObserver::connect(aVDSettings, aVDAnimation));// ->addObserver(new UIDisplay());	
		// check to see if session.json file exists and restore if it does	
	mediator->setupKeyboard();
#ifdef _DEBUG
	VDLog::create();
#endif 

	return VDSessionFacadeRef(new VDSessionFacade(VDSessionRef(new VDSession(aVDSettings, aVDAnimation, aVDUniforms, aVDMix)), mediator));
}
VDSessionFacadeRef VDSessionFacade::setUniformValue(unsigned int aCtrl, float aValue) {
	mVDMediator->setUniformValue(aCtrl, aValue);
	return shared_from_this();
}
VDSessionFacadeRef VDSessionFacade::getWindowsResolution() {
	mVDSession->getWindowsResolution();
	return shared_from_this();
}

VDSessionFacadeRef VDSessionFacade::addUIObserver(VDSettingsRef aVDSettings, VDUniformsRef aVDUniforms) {
	mVDMediator->addObserver(VDUIObserver::connect(aVDSettings, aVDUniforms));
	return shared_from_this();
}
VDSessionFacadeRef VDSessionFacade::setupOSCReceiver() {
	if (!mOscReceiverConnected) {
		mOscReceiverConnected = true;
		mVDMediator->setupOSCReceiver();
	}
	return shared_from_this();
}
VDSessionFacadeRef VDSessionFacade::setupMidi() {
	if (!mIsMidiSetup) {
		mIsMidiSetup = true;
		mVDMediator->setupMidi();
	}
	return shared_from_this();
}
VDSessionFacadeRef VDSessionFacade::setupSession() {
	sessionPath = getAssetPath("") / sessionFileName;
	if (fs::exists(sessionPath))
	{
		restore();
	}
	else
	{
		// Create json file if it doesn't already exist.
		std::ofstream oStream(sessionPath.string());
		oStream.close();
		save();
	}
	return shared_from_this();
}
//VDSessionFacadeRef VDSessionFacade::setupWSClient() {
//	if (!mWSClientConnected ) {
//		mWSClientConnected = true;
//		mVDMediator->setupWSClient();
//	}
//	return shared_from_this();
//}
//VDSessionFacadeRef VDSessionFacade::setupHttpClient() {
//	mVDSession->setupHttpClient();
//	return shared_from_this();
//}
//VDSessionFacadeRef VDSessionFacade::loadShaderFromHttp(const std::string& url, unsigned int aFboIndex) {
//	mVDSession->loadShaderFromHttp(url, aFboIndex);
//	return shared_from_this();
//}

VDSessionFacadeRef VDSessionFacade::setupKeyboard() {
	mVDMediator->setupKeyboard();
	return shared_from_this();
}
//VDSessionFacadeRef VDSessionFacade::addOSCObserver(const std::string& host, unsigned int port) {
//	if (!mOscSenderConnected) {
//		mOscSenderConnected = true;
//		mVDMediator->addObserver(VDOscObserver::connect(host, port));
//	}
//	return shared_from_this();
//}
VDSessionFacadeRef VDSessionFacade::addSocketIOObserver(const std::string& host, unsigned int port) {
	//mVDMediator->addObserver(VDSocketIOObserver::connect(host, port));
	return shared_from_this();
}
VDSessionFacadeRef VDSessionFacade::setAnim(unsigned int aCtrl, unsigned int aAnim) {
	mVDSession->setAnim(aCtrl, aAnim);
	return shared_from_this();
}
unsigned int VDSessionFacade::getAnim(unsigned int aCtrl) { 
	return mVDSession->getAnim(aCtrl);
}
VDSessionFacadeRef VDSessionFacade::toggleValue(unsigned int aCtrl) {
	mVDSession->toggleValue(aCtrl);
	return shared_from_this();
}
VDSessionFacadeRef VDSessionFacade::toggleUseLineIn() { 
	mVDSession->toggleUseLineIn(); 
	return shared_from_this();
};
VDSessionFacadeRef VDSessionFacade::tapTempo() {
	mVDSession->tapTempo();
	return shared_from_this();
};
VDSessionFacadeRef VDSessionFacade::toggleUseTimeWithTempo() {
	mVDSession->toggleUseTimeWithTempo();
	return shared_from_this();
};
VDSessionFacadeRef VDSessionFacade::useTimeWithTempo() {
	mVDSession->useTimeWithTempo();
	return shared_from_this();
};

VDSessionFacadeRef VDSessionFacade::update() {
	mVDSession->update();
	mVDMediator->update();
	getAudioTexture(); // 20240518 force audio fft for fx.glsl even if no fboshader uses it (no performance drop)
	return shared_from_this();
}
// begin terminal operations
bool VDSessionFacade::getUseTimeWithTempo() {
	return mVDSession->getUseTimeWithTempo();
};
// midi
bool VDSessionFacade::isMidiSetup() {
	return mIsMidiSetup;
};
// osc
bool VDSessionFacade::isOscSenderConnected() {
	return mOscSenderConnected;
};
bool VDSessionFacade::isOscReceiverConnected() {
	return mOscReceiverConnected;
};
int VDSessionFacade::getOSCReceiverPort() {
	return mVDMediator->getOSCReceiverPort();
};
void VDSessionFacade::setOSCReceiverPort(int aReceiverPort) {
	mVDMediator->setOSCReceiverPort(aReceiverPort);
};
void VDSessionFacade::setOSCMsg(const std::string& aMsg) {
	if (mOscReceiverConnected) {
		mVDMediator->setOSCMsg(aMsg);
	}
};
std::string VDSessionFacade::getOSCMsg() {
	return (mOscReceiverConnected) ? mVDMediator->getOSCMsg() : "";
};

// Websockets begin
bool					VDSessionFacade::isWSClientConnected() {
	return mWSClientConnected;
};
int						VDSessionFacade::getWSClientPort() {
	return mVDMediator->getWSClientPort();
};
void VDSessionFacade::wsPing() {
	mVDMediator->wsPing();
}
void VDSessionFacade::setWSClientPort(int aPort) {
	mVDMediator->setWSClientPort(aPort);
};
VDSessionFacadeRef VDSessionFacade::wsConnect() {
	mVDMediator->wsConnect();
	return shared_from_this();
}
void					VDSessionFacade::setWSMsg(const std::string& aMsg) {
	if (mWSClientConnected) {
		mVDMediator->setWSMsg(aMsg);
	}
};

std::string VDSessionFacade::getWSMsg() {
	return (mWSClientConnected) ? mVDMediator->getWSMsg() : "";
};
// Websockets end

ci::gl::TextureRef VDSessionFacade::buildRenderedMixetteTexture(unsigned int aIndex) {
	return mVDSession->getRenderedMixetteTexture(aIndex);
}
ci::gl::TextureRef VDSessionFacade::buildFboTexture(unsigned int aIndex) {
	return mVDSession->getFboTexture(aIndex);;
}
ci::gl::TextureRef VDSessionFacade::getFboShaderTexture(unsigned int aIndex) {
	return mVDSession->getFboShaderTexture(aIndex);
}
std::string VDSessionFacade::getFboShaderName(unsigned int aIndex) {
	return mVDSession->getFboShaderName(aIndex);
}
unsigned int VDSessionFacade::getFboShaderListSize() {
	return mVDSession->getFboShaderListSize();
}
std::vector<ci::gl::GlslProg::Uniform> VDSessionFacade::getFboShaderUniforms(unsigned int aFboShaderIndex) {
	return mVDSession->getFboShaderUniforms(aFboShaderIndex);
}

float VDSessionFacade::getUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex) {
	return mVDSession->getUniformValueByLocation(aFboShaderIndex, aLocationIndex);
};
void VDSessionFacade::setUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex, float aValue) {
	mVDSession->setUniformValueByLocation(aFboShaderIndex, aLocationIndex, aValue);
};

ci::gl::TextureRef VDSessionFacade::buildFboRenderedTexture(unsigned int aFboIndex) {
	return mVDSession->getFboRenderedTexture(aFboIndex);
}
ci::gl::TextureRef VDSessionFacade::buildPostFboTexture() {
	return mVDSession->getPostFboTexture();
}
ci::gl::TextureRef VDSessionFacade::buildFxFboTexture() {
	return mVDSession->getFxFboTexture();
}
ci::gl::TextureRef VDSessionFacade::buildWarpFboTexture() {
	return mVDSession->getWarpFboTexture();
}
ci::gl::TextureRef VDSessionFacade::buildRenderedWarpFboTexture() {
	return mVDSession->getRenderedWarpFboTexture();
}
unsigned int VDSessionFacade::getWarpAFboIndex(unsigned int aWarpIndex) {
	return mVDSession->getWarpAFboIndex(aWarpIndex);
}
unsigned int VDSessionFacade::getWarpBFboIndex(unsigned int aWarpIndex) {
	return mVDSession->getWarpBFboIndex(aWarpIndex);
}
float VDSessionFacade::getMinUniformValue(unsigned int aIndex) {
	return mVDSession->getMinUniformValue(aIndex);
}
float VDSessionFacade::getMaxUniformValue(unsigned int aIndex) {
	return mVDSession->getMinUniformValue(aIndex);
}
float VDSessionFacade::getDefaultUniformValue(unsigned int aIndex) {
	return mVDSession->getDefaultUniformValue(aIndex);
}
/* 20201229*/
int VDSessionFacade::getFboTextureWidth(unsigned int aFboIndex) {
	return mVDSession->getFboTextureWidth(aFboIndex);
};
int VDSessionFacade::getFboTextureHeight(unsigned int aFboIndex) {
	return mVDSession->getFboTextureHeight(aFboIndex);
}
std::string VDSessionFacade::getFboInputTextureName(unsigned int aFboIndex) {
	return mVDSession->getFboInputTextureName(aFboIndex);
}
ci::gl::Texture2dRef VDSessionFacade::getFboInputTexture(unsigned int aTexIndex) {
	return mVDSession->getFboInputTexture(aTexIndex);
}
void VDSessionFacade::setFboTextureAudioMode(unsigned int aFboIndex) {
	return mVDSession->setFboTextureAudioMode(aFboIndex);
}
void VDSessionFacade::saveThumbnail(unsigned int aFboIndex) {
	mVDSession->saveThumbnail(aFboIndex);
}
void VDSessionFacade::setSelectedFbo(unsigned int aFboIndex) {
	mVDSession->setSelectedFbo(aFboIndex);
}
unsigned int VDSessionFacade::getSelectedFbo() { 
	return mVDSession->getSelectedFbo();
};
/*unsigned int VDSessionFacade::getFboInputTextureIndex(unsigned int aFboIndex) {
	return mVDSession->getFboInputTextureIndex(aFboIndex);
}*/
std::string VDSessionFacade::getFboName(unsigned int aFboIndex) {
	return mVDSession->getFboShaderName(aFboIndex);
}
std::vector<ci::gl::GlslProg::Uniform> VDSessionFacade::getUniforms(unsigned int aFboIndex) {
	return mVDSession->getUniforms(aFboIndex);
}
ci::gl::Texture2dRef VDSessionFacade::buildFboInputTexture(unsigned int aFboIndex) {
	return mVDSession->getFboInputTexture(aFboIndex);
}
ci::gl::Texture2dRef VDSessionFacade::getFboInputTextureListItem(unsigned int aFboIndex, unsigned int aTexIndex) {
	return mVDSession->getFboInputTextureListItem(aFboIndex, aTexIndex);
}
/*ci::gl::Texture2dRef VDSessionFacade::getInputTexture(unsigned int aTexIndex) {
	return mVDSession->getInputTexture(aTexIndex);
}*/

void VDSessionFacade::createWarp() {
	mVDSession->createWarp();
}
unsigned int VDSessionFacade::getWarpCount() {
	return mVDSession->getWarpCount();
};
std::string VDSessionFacade::getWarpName(unsigned int aWarpIndex) {
	return mVDSession->getWarpName(aWarpIndex);
}// or trycatch
int VDSessionFacade::getWarpWidth(unsigned int aWarpIndex) {
	return mVDSession->getWarpWidth(aWarpIndex);
}
int VDSessionFacade::getWarpHeight(unsigned int aWarpIndex) {
	return mVDSession->getWarpHeight(aWarpIndex);
}


int VDSessionFacade::getFFTWindowSize() {
	return mVDSession->getFFTWindowSize();
}
float* VDSessionFacade::getFreqs() {
	return mVDSession->getFreqs();
}


bool VDSessionFacade::showUI() {
	return mVDSession->showUI();
};
VDSessionFacadeRef VDSessionFacade::toggleUI() {
	mVDSession->toggleUI();
	return shared_from_this();
};
std::string VDSessionFacade::getModeName(unsigned int aMode) {
	return mVDSession->getModeName(aMode);
}
unsigned int VDSessionFacade::getModesCount() {
	return mVDSession->getModesCount();
};
int VDSessionFacade::getUniformIndexForName(const std::string& aName) {
	return mVDSession->getUniformIndexForName(aName);
};
float VDSessionFacade::getUniformValue(unsigned int aCtrl) {
	return mVDMediator->getUniformValue(aCtrl);
};

std::string VDSessionFacade::getUniformName(unsigned int aIndex) {
	return mVDMediator->getUniformName(aIndex);
}
/*void VDSessionFacade::setIntUniformValueByIndex(unsigned int aCtrl, int aValue) {
	mVDMediator->setUniformValue(aCtrl, aValue);
}
void VDSessionFacade::setBoolUniformValueByIndex(unsigned int aCtrl, float aValue) {
	mVDMediator->setUniformValue(aCtrl, aValue);
}*/
// end terminal operations 
// begin events
bool VDSessionFacade::handleMouseMove(MouseEvent event)
{
	return mVDSession->handleMouseMove(event);
}
bool VDSessionFacade::handleMouseDown(MouseEvent event)
{
	return mVDSession->handleMouseDown(event);
}
bool VDSessionFacade::handleMouseDrag(MouseEvent event)
{
	return mVDSession->handleMouseDrag(event);
}
bool VDSessionFacade::handleMouseUp(MouseEvent event)
{
	return mVDSession->handleMouseUp(event);
}
void VDSessionFacade::fileDrop(FileDropEvent event)
{
	mVDSession->fileDrop(event);
}
//VDSessionFacadeRef VDSessionFacade::loadFromJsonFile(const fs::path& jsonFile) {
//	mVDSession->loadFromJsonFile(jsonFile);
//	return shared_from_this();
//}
bool VDSessionFacade::handleKeyDown(KeyEvent& event) {
	bool handled = true;
	if (!mVDSession->handleKeyDown(event)) {
		if (!mVDMediator->handleKeyDown(event)) {
			handled = false;
		}
	}
	event.setHandled(handled);
	return event.isHandled();
}
bool VDSessionFacade::handleKeyUp(KeyEvent& event) {
	bool handled = true;
	if (!mVDSession->handleKeyUp(event)) {
		if (!mVDMediator->handleKeyUp(event)) {
			handled = false;
		}
	}
	event.setHandled(handled);
	return event.isHandled();
}

// end events
VDSessionRef VDSessionFacade::getInstance() const {
	return mVDSession;
}
void VDSessionFacade::save()
{
	saveWarps();
	// save in sessionPath
	/* JsonTree doc;
	JsonTree settings = JsonTree::makeArray("settings");
	settings.addChild(ci::JsonTree("apiUrl", ""));
	doc.pushBack(settings);
	doc.write(writeFile(sessionPath), JsonTree::WriteOptions());*/
}

void VDSessionFacade::restore()
{
	// save load settings
	if (!fs::exists(sessionPath)) {
		// check to see if json file exists
		return;
	}

	//try {
	//	JsonTree doc(loadFile(sessionPath));
	//	if (doc.hasChild("settings")) {
	//		JsonTree settings(doc.getChild("settings"));
	//		if (settings.hasChild("apiUrl")) mVDSession->setApiUrl(settings.getValueForKey<std::string>("apiUrl"));
	//		if (settings.hasChild("preferredAudioInput")) mVDSession->setPreferredAudioInputDevice(settings.getValueForKey<string>("preferredAudioInput"));
	//		if (settings.hasChild("preferredAudioOutput")) mVDSession->setPreferredAudioOutputDevice(settings.getValueForKey<string>("preferredAudioOutput"));
	//		if (settings.hasChild("preferredMidiInput")) mVDMediator->setPreferredMidiInputDevice(settings.getValueForKey<string>("preferredMidiInput"));

	//	}
	//}
	//catch (const JsonTree::ExcJsonParserError& exc) {
	//	CI_LOG_W(exc.what());
	//}
}