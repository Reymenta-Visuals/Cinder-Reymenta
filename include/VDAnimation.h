/*
	VDAnimation
	Handles all uniform variables for shaders: index, name, type, bounds and their animation.
*/
// TODO put audio in a separate class
// TODO put timing in separate class?
// TODO implement lazy loading for audio

#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
//!  audio
#include "cinder/audio/Context.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/Utilities.h"
#include "cinder/audio/Source.h"
#include "cinder/audio/Target.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/SamplePlayerNode.h"
#include "cinder/audio/SampleRecorderNode.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/Rand.h"
//!  json
//#include "cinder/Json.h"
//!  Settings
#include "VDSettings.h"
//!  Uniforms
#include "VDUniforms.h"

using namespace ci;
using namespace ci::app;

namespace videodromm
{
	// stores the pointer to the VDAnimation instance
	typedef std::shared_ptr<class VDAnimation> VDAnimationRef;

	class VDAnimation {
	public:
		VDAnimation(VDSettingsRef aVDSettings, VDUniformsRef aVDUniforms);

		static VDAnimationRef			create(VDSettingsRef aVDSettings, VDUniformsRef aVDUniforms)
		{
			return std::shared_ptr<VDAnimation>(new VDAnimation(aVDSettings, aVDUniforms));
		}
		void							update();
		void							resetAnim();
		void							resetUniformAnim(unsigned int anim);
		const int						mBlendModes = 28;
		void							blendRenderEnable(bool render) { mBlendRender = render; };
		// tap tempo
		bool							mUseTimeWithTempo;
		void							toggleUseTimeWithTempo() { mUseTimeWithTempo = !mUseTimeWithTempo; };
		void							useTimeWithTempo() { mUseTimeWithTempo = true; };
		bool							getUseTimeWithTempo() { return mUseTimeWithTempo; };
		float							iTempoTimeBeatPerBar;
		void							tapTempo();
		// animation
		int								currentScene;
		// keyboard
		bool							handleKeyDown(KeyEvent& event);
		bool							handleKeyUp(KeyEvent& event);
		// audio
		ci::gl::TextureRef				getAudioTexture();
		std::string						getAudioTextureName() { return mAudioName; };
		bool							mLineInInitialized;
		bool							mWaveInitialized;
		audio::InputDeviceNodeRef		mLineIn;
		audio::MonitorSpectralNodeRef	mMonitorLineInSpectralNode;
		audio::MonitorSpectralNodeRef	mMonitorWaveSpectralNode;
		audio::SamplePlayerNodeRef		mSamplePlayerNode;
		audio::SourceFileRef			mSourceFile;
		audio::MonitorSpectralNodeRef	mScopeLineInFmt;
		audio::BufferPlayerNodeRef		mBufferPlayerNode;

		std::vector<float>				mMagSpectrum;

		// number of frequency bands of our spectrum
		static const int				mFFTWindowSize = 32;
		float							iFreqs[mFFTWindowSize];
		int								mPosition;
		std::string						mAudioName;
		void							preventLineInCrash(); // at next launch
		void							saveLineIn();
		void							preventWaveMonitorCrash(); // at next launch
		void							saveWaveMonitor();
		bool							getUseAudio() {
			return mUseAudio;
		};
		bool							getUseLineIn() {
			return mUseLineIn;
		};
		void							setUseLineIn(bool useLineIn = true) {
			mUseLineIn = useLineIn;
		};
		void							toggleUseLineIn() {
			mUseLineIn = !mUseLineIn;
			if (!mLineInInitialized && mUseLineIn) {
				initLineIn();
			}
		};
		void							setPreferredAudioInputDevice(const std::string& aPreferredAudioInputDevice) {
			mPreferredAudioInputDevice = aPreferredAudioInputDevice;
		};
		void							setPreferredAudioOutputDevice(const std::string& aPreferredAudioOutputDevice) {
			mPreferredAudioOutputDevice = aPreferredAudioOutputDevice;
		};
		void							initLineIn();
		void							setUseWaveMonitor(bool useWaveMonitor) {
			mUseAudio = useWaveMonitor;
		};
		bool							getUseWaveMonitor() { return mUseAudio; };
		void							toggleUseWaveMonitor() { mUseAudio = !mUseAudio; };


		// audio
		bool							isAudioBuffered() { return mAudioBuffered; };
		void							toggleAudioBuffered() {
			mAudioBuffered = !mAudioBuffered;
		};
		bool							getUseRandom() { return mUseRandom; };
		void							toggleUseRandom() { mUseRandom = !mUseRandom; };

		// shaders
		int								getUniformTypeByName(const std::string& aName) {
			return mVDUniforms->getUniformTypeByName(aName);
		}
		bool							isExistingUniform(const std::string& aName) {
			return mVDUniforms->isExistingUniform(aName);
		};

		int								getUniformIndexForName(const std::string& aName) {
			return mVDUniforms->getUniformIndexForName(aName);
			//return shaderUniforms[stringToIndex(aName)].index;
		};
		bool							toggleValue(unsigned int aIndex);

		void							setAnim(unsigned int aCtrl, unsigned int aAnim) {
			mVDUniforms->setAnim(aCtrl, aAnim);
		}
		unsigned int					getAnim(unsigned int aCtrl) {
			return mVDUniforms->getAnim(aCtrl);
		}
		unsigned int					getBlendModesCount() { return mBlendModes; };
		bool							renderBlend() { return mBlendRender; };

		// timed animation
		unsigned int					getFreqIndexSize() { return (unsigned int)freqIndexes.size(); };
		unsigned int					getFreqIndex(unsigned int aFreqIndex) { return (unsigned int)freqIndexes[aFreqIndex]; };
		void							setFreqIndex(unsigned int aFreqIndex, unsigned int aFreq) { freqIndexes[aFreqIndex] = aFreq; };
		//float							getFreq(unsigned int aFreqIndex) { return iFreqs[freqIndexes[aFreqIndex]]; };

	private:
		// Settings
		VDSettingsRef					mVDSettings;
		VDUniformsRef					mVDUniforms;
		// Audio
		ci::audio::Context* ctx = audio::Context::master();
		std::vector<ci::audio::DeviceRef> inputDevices;
		std::vector<ci::audio::DeviceRef> outputDevices;
		std::string						  mPreferredAudioInputDevice = "Réseau de microphones (Realtek(R) Audio)";
		std::string						  mPreferredAudioOutputDevice = "Haut-parleurs (Model 12)";
		bool							mUseAudio = true;
		bool							mUseRandom = false;
		bool							mUseLineIn = false;
		std::map<int, int>				freqIndexes;
		bool							mAudioBuffered = false;
		ci::gl::TextureRef				mAudioTexture;
		gl::Texture2d::Format			mAudioFormat;
		//unsigned char					dTexture[256];// MUST be < mWindowSize

		// shaders
		//ci::Json					uniformToJson(int i);

		// time
		ci::Timer						mTimer;
		std::deque <double>				buffer;
		void							calculateTempo();
		int								counter;
		double							averageTime;
		double							currentTime;
		double							startTime;
		float							previousTime;
		float							previousTimeBeatPerBar;
		float							mLastBar = 0.0f; // 20210101 was int
		std::unordered_map<int, float>	mBadTV;
		bool							mBlendRender;

	};
}