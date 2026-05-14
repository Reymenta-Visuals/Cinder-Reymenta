/*
	VDSessionFacade

*/
// TODO 

#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
// Logger
#ifdef _DEBUG
#include "VDLog.h"
#endif 

// Settings
#include "VDSettings.h"
//
#include "VDSession.h"
#include "VDAnimation.h"
#include "VDMediator.h"
//#include "VDOscObserver.h"
#include "VDUniforms.h"
#include "VDUIObserver.h"
// VDRouterBuilder
//#include "VDRouterBuilder.h"

using namespace ci;
using namespace ci::app;

namespace videodromm
{
	typedef std::shared_ptr<class VDSession> VDSessionRef;
	typedef std::shared_ptr<class VDSessionFacade> VDSessionFacadeRef;

	class VDSessionFacade : public std::enable_shared_from_this<VDSessionFacade> {
	public:
		static VDSessionFacadeRef createVDSession(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix);

		VDSessionFacadeRef		setUniformValue(unsigned int aCtrl, float aValue);
		VDSessionFacadeRef		addUIObserver(VDSettingsRef aVDSettings, VDUniformsRef aVDUniforms);
		VDSessionFacadeRef		getWindowsResolution();
		VDSessionFacadeRef		setupSession();
		VDSessionFacadeRef		setupOSCReceiver();
		VDSessionFacadeRef		setupMidi();
		//VDSessionFacadeRef		setupWSClient();
		//VDSessionFacadeRef		setupHttpClient();
		//VDSessionFacadeRef		loadShaderFromHttp(const std::string& url, unsigned int aFboIndex);
		VDSessionFacadeRef		setupKeyboard();
		//VDSessionFacadeRef		addOSCObserver(const std::string& host, unsigned int port);
		VDSessionFacadeRef		addSocketIOObserver(const std::string& host, unsigned int port);
		VDSessionFacadeRef		setAnim(unsigned int aCtrl, unsigned int aAnim);
		VDSessionFacadeRef		toggleValue(unsigned int aCtrl);
		VDSessionFacadeRef		tapTempo();
		VDSessionFacadeRef		toggleUseTimeWithTempo();
		VDSessionFacadeRef		useTimeWithTempo();
		VDSessionFacadeRef		toggleUseLineIn();
		//VDSessionFacadeRef		loadFromJsonFile(const fs::path& jsonFile);
		VDSessionFacadeRef		update();
		// begin terminal operations
		unsigned int			getAnim(unsigned int aCtrl);
		bool					getUseTimeWithTempo();
		// OSC
		bool					isOscSenderConnected();
		bool					isOscReceiverConnected();
		int						getOSCReceiverPort();
		void					setOSCReceiverPort(int aReceiverPort);
		void					setOSCMsg(const std::string& aMsg);
		std::string				getOSCMsg();
		// midi
		void					midiOutSendNoteOn(int i, int channel, int pitch, int velocity) { mVDMediator->midiOutSendNoteOn(i, channel, pitch, velocity); };
		int						getMidiInPortsCount() { return mVDMediator->getMidiInPortsCount(); };
		string					getMidiInPortName(int i) { return mVDMediator->getMidiInPortName(i); };
		bool					isMidiInConnected(int i) { return mVDMediator->isMidiInConnected(i); };
		void					openMidiInPort(int i) { mVDMediator->openMidiInPort(i); };
		void					closeMidiInPort(int i) { mVDMediator->closeMidiInPort(i); };
		int						getMidiOutPortsCount() { return mVDMediator->getMidiOutPortsCount(); };
		string					getMidiOutPortName(int i) { return mVDMediator->getMidiOutPortName(i); };
		bool					isMidiOutConnected(int i) { return mVDMediator->isMidiOutConnected(i); };
		void					openMidiOutPort(int i) { mVDMediator->openMidiOutPort(i); };
		void					closeMidiOutPort(int i) { mVDMediator->closeMidiOutPort(i); };
		std::string				getMidiMsg() { return mVDMediator->getMidiMsg(); }
		bool					isMidiSetup();
		// websockets
		bool					isWSClientConnected();
		int						getWSClientPort();
		VDSessionFacadeRef		wsConnect();
		void					wsPing();
		void					setWSClientPort(int aPort);
		void					setWSMsg(const std::string& aMsg);
		std::string				getWSMsg();

		ci::gl::TextureRef		buildRenderedMixetteTexture(unsigned int aIndex);
		ci::gl::TextureRef		buildFboTexture(unsigned int aIndex);
		ci::gl::TextureRef		buildFboRenderedTexture(unsigned int aFboIndex);
		ci::gl::TextureRef		getFboShaderTexture(unsigned int aIndex);
		std::string				getFboShaderName(unsigned int aIndex);
		unsigned int			getFboShaderListSize(); 
		bool					isFboValid(unsigned int aFboIndex) {
			return mVDSession->isFboValid(aFboIndex);
		};
		std::string				getFboMsg(unsigned int aFboIndex) {
			return mVDSession->getFboMsg(aFboIndex);
		};
		std::string				getFboError(unsigned int aFboIndex) {
			return mVDSession->getFboError(aFboIndex);
		};	
		std::string				getFboStatus(unsigned int aFboIndex) {
			return mVDSession->getFboStatus(aFboIndex);
		};
		unsigned int			getFboMs(unsigned int aTexIndex = 0) {
			return mVDSession->getFboMs(aTexIndex);
		};
		bool					isValidInputTexture(unsigned int aTexIndex) {
			return mVDSession->isValidInputTexture(aTexIndex);
		};
		unsigned int			getFboMsTotal(unsigned int aFboIndex) {
			return mVDSession->getFboMsTotal(aFboIndex);
		};
		
		std::vector<ci::gl::GlslProg::Uniform> getFboShaderUniforms(unsigned int aFboShaderIndex);
		//float					getFboShaderUniformValue();
		float					getUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex);
		void					setUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex, float aValue);
		ci::gl::TextureRef		buildPostFboTexture();
		ci::gl::TextureRef		buildFxFboTexture();
		ci::gl::TextureRef		buildWarpFboTexture();
		ci::gl::TextureRef		buildRenderedWarpFboTexture();
		unsigned int			getWarpAFboIndex(unsigned int aWarpIndex);
		unsigned int			getWarpBFboIndex(unsigned int aWarpIndex);
		float					getMinUniformValue(unsigned int aIndex);
		float					getMaxUniformValue(unsigned int aIndex);
		float					getDefaultUniformValue(unsigned int aIndex);
		int						getFboTextureWidth(unsigned int aFboIndex);
		int						getFboTextureHeight(unsigned int aFboIndex);
		unsigned int			getWarpCount();
		void					createWarp();
		void					saveWarps() {
			mVDSession->saveWarps();
		};
		std::string				getWarpName(unsigned int aWarpIndex);// or trycatch
		int						getWarpWidth(unsigned int aWarpIndex);
		int						getWarpHeight(unsigned int aWarpIndex);
		std::string				getFboInputTextureName(unsigned int aFboIndex = 0);
		ci::gl::Texture2dRef	getFboInputTexture(unsigned int aTexIndex = 0);
		void					setFboTextureAudioMode(unsigned int aFboIndex);
		void					saveThumbnail(unsigned int aFboIndex = 0);
		void					setSelectedFbo(unsigned int aFboIndex = 0);
		unsigned int			getSelectedFbo();
		//unsigned int			getFboInputTextureIndex(unsigned int aFboIndex = 0);

		std::string				getFboName(unsigned int aFboIndex);
		std::string				getTrackName() { return mTrackName; };
		// audio
		ci::gl::TextureRef		getAudioTexture() { return mVDSession->getAudioTexture(); };
		bool					getUseAudio() { return mVDSession->getUseAudio(); };
		bool					getUseLineIn() { return mVDSession->getUseLineIn(); };
		void					setUseLineIn(bool useLineIn = true) { mVDSession->setUseLineIn(useLineIn); };
		
		bool					isAudioBuffered() { return mVDSession->isAudioBuffered(); };
		void					toggleAudioBuffered() { mVDSession->toggleAudioBuffered(); };
		bool					getUseWaveMonitor() { return mVDSession->getUseWaveMonitor(); };
		void					toggleUseWaveMonitor() { mVDSession->toggleUseWaveMonitor(); };
		bool					getUseRandom() { return mVDSession->getUseRandom(); };
		void					toggleUseRandom() { mVDSession->toggleUseRandom(); };
		/*void					setFboInputTexture(unsigned int aFboIndex = 0, unsigned int aTexIndex = 0) {
			mVDSession->setFboInputTexture(aFboIndex, aTexIndex);
		}*/
		unsigned int			getInputTexturesCount(unsigned int aFboIndex = 0) {
			return mVDSession->getInputTexturesCount(aFboIndex);
		}
		std::string				getInputTextureName(unsigned int aTexIndex) {
			return mVDSession->getInputTextureName(aTexIndex);
		}
		int						getFFTWindowSize();
		float*					getFreqs();
		float					getFreq(unsigned int aFreqIndex) { return mVDSession->getFreq(aFreqIndex); };
		int						getFreqIndex(unsigned int aFreqIndex) { return mVDSession->getFreqIndex(aFreqIndex); };
		void					setFreqIndex(unsigned int aFreqIndex, unsigned int aFreq) { mVDSession->setFreqIndex(aFreqIndex, aFreq); };

		bool					showUI();
		VDSessionFacadeRef		toggleUI();
		int						getErrorCode() { return mVDMediator->getErrorCode(); }

		std::vector<ci::gl::GlslProg::Uniform> getUniforms(unsigned int aFboIndex = 0);
		ci::gl::Texture2dRef	buildFboInputTexture(unsigned int aFboIndex = 0);
		ci::gl::Texture2dRef	getFboInputTextureListItem(unsigned int aFboIndex = 0, unsigned int aTexIndex = 0);
		void					resetAnim() { mVDSession->resetAnim(); }
		std::string				getModeName(unsigned int aMode);
		unsigned int			getModesCount();
		int						getUniformIndexForName(const std::string& aName);
		float					getUniformValue(unsigned int aCtrl);
		std::string				getUniformName(unsigned int aIndex);
		// end terminal operations 
		// begin events
		bool					handleMouseMove(MouseEvent event);
		bool					handleMouseDown(MouseEvent event);
		bool					handleMouseDrag(MouseEvent event);
		bool					handleMouseUp(MouseEvent event);
		void					fileDrop(FileDropEvent event);
		bool					handleKeyDown(KeyEvent& event);
		bool					handleKeyUp(KeyEvent& event);

		// end events
		VDSessionRef			getInstance() const;
		bool					loadFolder(const string& aFolder) {
			mTrackName = aFolder;
			return mVDSession->loadFolder(aFolder);
		}

	private:
		VDSessionFacade(VDSessionRef session, VDMediatorObservableRef mediator) : mVDSession(session), mVDMediator(mediator), mOscSenderConnected(false), mIsMidiSetup(false), mOscReceiverConnected(false), mWSClientConnected(false) { }
		VDSessionRef						mVDSession;
		VDMediatorObservableRef				mVDMediator;
		bool								mOscSenderConnected = false;
		bool								mOscReceiverConnected = false;
		bool								mWSClientConnected = false;
		bool								mIsMidiSetup = false;
		std::string							mTrackName = "";
		// session
		fs::path							sessionPath;
		const std::string					sessionFileName = "session.json";
		void								save();
		void								restore();
	};


}
