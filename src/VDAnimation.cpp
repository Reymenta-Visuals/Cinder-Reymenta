#include "VDAnimation.h"

using namespace videodromm;

VDAnimation::VDAnimation(VDSettingsRef aVDSettings, VDUniformsRef aVDUniforms) {
	mVDSettings = aVDSettings;
	mVDUniforms = aVDUniforms;
	mBlendRender = false;
	//audio
	mAudioBuffered = false;
	mUseAudio = true;
	mUseLineIn = false;
	mAudioFormat = gl::Texture2d::Format().swizzleMask(GL_RED, GL_RED, GL_RED, GL_ONE).internalFormat(GL_RED);
	mAudioTexture = ci::gl::Texture::create(64, 2, mAudioFormat);
	mLineInInitialized = false;
	mWaveInitialized = false;
	mAudioName = "not initialized";
	for (int i{ 0 }; i < 7; i++)
	{
		freqIndexes[i] = i * 7;
	}
	for (int i{ 0 }; i < mFFTWindowSize; i++)
	{
		iFreqs[i] = 0.0f;
	}

	currentScene = 0;

	previousTime = 0.0f;
	counter = 0;
	// tempo
	mUseTimeWithTempo = false;
	// init timer
	mTimer.start();
	startTime = currentTime = mTimer.getSeconds();

	mVDUniforms->setVec3UniformValueByIndex(mVDUniforms->IRESOLUTION, vec3(mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONX), mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONY), 1.0));
}

//! uniform to json
/* ci::Json VDAnimation::uniformToJson( int i )
{
	std::stringstream svec4;
	ci::Json		  json;
	//string s = controlIndexes[i];

	ci::Json u = ci::Json::array();
	u.push_back("uniform");
	// common
	int uniformType = mVDUniforms->getUniformType(i);//  shaderUniforms[i].uniformType;
	u.push_back(ci::Json{ {"type", uniformType} });
	u.push_back(ci::Json{ {"name", mVDUniforms->getUniformName(i)} });
	u.push_back(ci::Json{ {"index", i} });
	// type specific
	switch (uniformType) {
	case 0:
		//float
		u.push_back(ci::Json{ {"value", mVDUniforms->getDefaultUniformValue(i)} });
		u.push_back(ci::Json{ {"min", mVDUniforms->getMinUniformValue(i)} });
		u.push_back(ci::Json{ {"max", mVDUniforms->getMaxUniformValue(i)} });
		break;
	case 1:
		// sampler2d
		u.push_back(ci::Json{ {"textureindex", mVDUniforms->getUniformTextureIndex(i)} });
		break;
	case 4:
		// vec4
		//svec4 << toString(shaderUniforms[i].vec4Value.x) << "," << toString(shaderUniforms[i].vec4Value.y);
		//svec4 << "," << toString(shaderUniforms[i].vec4Value.z) << "," << toString(shaderUniforms[i].vec4Value.w);
		u.push_back(ci::Json{ {"value", mVDUniforms->getDefaultUniformValue(i)} });
		break;
	case 5:
		// int
		u.push_back(ci::Json{ {"value", mVDUniforms->getDefaultUniformValue(i)} });
		break;
	case 6:
		// boolean
		u.push_back(ci::Json{ {"value", mVDUniforms->getDefaultUniformValue(i)} });
		break;
	default:
		break;
	}

	json.push_back( u );
	return json;
}*/

/*
void VDAnimation::setExposure(float aExposure) {
	mExposure = aExposure;
}
void VDAnimation::setAutoBeatAnimation(bool aAutoBeatAnimation) {
	mAutoBeatAnimation = aAutoBeatAnimation;
}*/
bool VDAnimation::handleKeyDown(KeyEvent& event)
{
	//float newValue;
	/*bool handled = true;
	switch (event.getCode()) {
	case KeyEvent::KEY_s:
		// save animation
		save();
		break;
	//case KeyEvent::KEY_u:
	//	// save badtv keyframe
	//	mBadTV[getElapsedFrames() - 10] = 1.0f;
	//	//iBadTvRunning = true;
	//	// duration = 0.2
	//	shaderUniforms["iBadTv"].floatValue = 5.0f;
	//	//timeline().apply(&mVDSettings->iBadTv, 60.0f, 0.0f, 0.2f, EaseInCubic());
	//	break;
	case KeyEvent::KEY_d:
		// save end keyframe
		setEndFrame(getElapsedFrames() - 10);
		break;

		//case KeyEvent::KEY_x:
	case KeyEvent::KEY_y:
		mVDSettings->iXorY = !mVDSettings->iXorY;
		break;

	default:
		handled = false;
	} */

	event.setHandled(false);

	return event.isHandled();
}
bool VDAnimation::handleKeyUp(KeyEvent& event)
{
	bool handled = true;
	switch (event.getCode()) {
	case KeyEvent::KEY_u:
		// save badtv keyframe
		// not used for now mBadTV[getElapsedFrames()] = 0.001f;
		//shaderUniforms["iBadTv"].floatValue = 0.0f;
		break;

	default:
		handled = false;
	}
	event.setHandled(handled);

	return event.isHandled();
}
void  VDAnimation::initLineIn() {
#if (defined( CINDER_MSW ) || defined( CINDER_MAC ))
	bool audioDeviceFound = false;

	if (!mLineInInitialized) {
		if (getUseLineIn()) {
			// linein
			preventLineInCrash(); // at next launch
			try
			{
				// inputs
				inputDevices = ci::audio::Device::getInputDevices();
				outputDevices = ci::audio::Device::getOutputDevices();
				std::string preferredAudioDeviceKey = "";
				/* ci::Json	doc;
				ci::Json	audioinputs = ci::Json::array();
				audioinputs.push_back("audioinputs"); */
				for (ci::audio::DeviceRef in : inputDevices) {
					std::string currentInputName = in->getName();
					//ci::Json inputObj = ci::Json::object();
					//inputObj["key"] = in->getKey();
					//inputObj["name"] = currentInputName;
					//audioinputs.push_back(inputObj);

					std::size_t nameIndex = currentInputName.find(mPreferredAudioInputDevice);
					if (nameIndex != std::string::npos) {
						audioDeviceFound = true;
						preferredAudioDeviceKey = in->getKey();
						mVDSettings->setMsg("Inputs\nPreferred: " + currentInputName + "\n");
					}
					else {
						//mVDSettings->setMsg("Inputs\n: " + currentInputName + "\n");
					}
				}
				/* doc.push_back( audioinputs );
				// outputs
				ci::Json audiooutputs = ci::Json::array();
				audiooutputs.push_back("audiooutputs");*/
				for (ci::audio::DeviceRef out : outputDevices) {
					std::string currentOutputName = out->getName();
					//audiooutputs.push_back( ci::Json( out->getKey(), currentOutputName ) );

					std::size_t nameIndex = currentOutputName.find(mPreferredAudioOutputDevice);
					if (nameIndex != std::string::npos) {
						mVDSettings->setErrorMsg("Outputs\nPreferred: " + currentOutputName + "\n");
					}
					else {
						//mVDSettings->setErrorMsg("Outputs\n" + currentOutputName + "\n");
					}

					//ci::Json outputObj = ci::Json::object();
					//outputObj["key"] = out->getKey();
					//outputObj["name"] = currentOutputName;
					//audiooutputs.push_back(outputObj);
				}
				/* doc.push_back( audiooutputs );
				//doc.write(writeFile(getAssetPath("") / "audio.json"), Json::WriteOptions());
				*/
				if (audioDeviceFound) {
					auto device = ci::audio::Device::findDeviceByKey(preferredAudioDeviceKey);
					CI_LOG_W("trying to open mic/line in, if no line follows in the log, the app crashed so put UseLineIn to false in the VDSettings.xml file");
					mLineIn = ctx->createInputDeviceNode(device); //crashes if linein is present but disabled, doesn't go to catch block
					mAudioName = mPreferredAudioInputDevice;
				}
				else {
					mLineIn = ctx->createInputDeviceNode();
					mAudioName = mLineIn->getDevice()->getName();
				}

				CI_LOG_V("mic/line in opened");
				saveLineIn();

				auto scopeLineInFmt = audio::MonitorSpectralNode::Format().fftSize(mFFTWindowSize * 2).windowSize(mFFTWindowSize);// CHECK is * 2 needed
				mMonitorLineInSpectralNode = ctx->makeNode(new audio::MonitorSpectralNode(scopeLineInFmt));
				mLineIn >> mMonitorLineInSpectralNode;
				mLineIn->enable();
				mLineInInitialized = true;
			}
			catch (const std::exception& ex)
			{
				CI_LOG_V("mic/line in crashed");
				//mVDSettings->mMsg = "mic/line in crashed";
				mVDSettings->setErrorMsg(ex.what());
			}
		}
	}

#endif
}

ci::gl::TextureRef VDAnimation::getAudioTexture() {
	mAudioFormat = gl::Texture2d::Format().swizzleMask(GL_RED, GL_RED, GL_RED, GL_ONE).internalFormat(GL_RED);
	//auto ctx = audio::Context::master();

	if (!mWaveInitialized) {
		if (getUseAudio()) {

			preventWaveMonitorCrash(); // at next launch
			try
			{
				//initialize wave monitor
				auto scopeWaveFmt = audio::MonitorSpectralNode::Format().fftSize(mFFTWindowSize * 2).windowSize(mFFTWindowSize);// CHECK is * 2 needed
				mMonitorWaveSpectralNode = ctx->makeNode(new audio::MonitorSpectralNode(scopeWaveFmt));
				ctx->enable();
				mAudioName = "wave";
				saveWaveMonitor();
				mWaveInitialized = true;
			}
			catch (const std::exception& ex)
			{
				CI_LOG_V("wave monitor crashed");
				mVDSettings->setMsg("wave monitor crashed");
				mVDSettings->setErrorMsg(ex.what());
			}
		}
	}
#if (defined( CINDER_MSW ) || defined( CINDER_MAC ))
	if (getUseLineIn()) {
		mMagSpectrum = mMonitorLineInSpectralNode->getMagSpectrum();
	}
	else {
#endif
		if (getUseAudio()) {
			if (isAudioBuffered()) {
				if (mBufferPlayerNode) {
					mMagSpectrum = mMonitorWaveSpectralNode->getMagSpectrum();
				}
			}
			else {
				if (mSamplePlayerNode) {
					mMagSpectrum = mMonitorWaveSpectralNode->getMagSpectrum();
					mPosition = (int)mSamplePlayerNode->getReadPosition();
				}
			}
		}
#if (defined( CINDER_MSW ) || defined( CINDER_MAC ))
	}
#endif
	if (!mMagSpectrum.empty()) {
		mVDUniforms->setUniformValue(mVDUniforms->IMAXVOLUME, 0.0f);
		//maxVolume = 0.0f;//mIntensity
		size_t mDataSize = mMagSpectrum.size();
		if (mDataSize > 0 && mDataSize < mFFTWindowSize + 1) {
			float db;
			unsigned char signal[mFFTWindowSize];
			for (size_t i{ 0 }; i < mDataSize; i++) {
				float f = mMagSpectrum[i];
				db = audio::linearToDecibel(f);
				f = db * mVDUniforms->getUniformValue(mVDUniforms->IAUDIOX);
				if (f > mVDUniforms->getUniformValue(mVDUniforms->IMAXVOLUME))
				{
					mVDUniforms->setUniformValue(mVDUniforms->IMAXVOLUME, f);
				}
				iFreqs[i] = f;
				// update iFreq uniforms 
				if (i == getFreqIndex(0)) mVDUniforms->setUniformValue(mVDUniforms->IFREQ0, f);
				if (i == getFreqIndex(1)) mVDUniforms->setUniformValue(mVDUniforms->IFREQ1, f);
				if (i == getFreqIndex(2)) mVDUniforms->setUniformValue(mVDUniforms->IFREQ2, f);
				if (i == getFreqIndex(3)) mVDUniforms->setUniformValue(mVDUniforms->IFREQ3, f);

				if (i < mFFTWindowSize) {
					int ger = (int)f;
					signal[i] = static_cast<unsigned char>(ger);
				}
			}
			// store it as a 512x2 texture
			mAudioTexture = gl::Texture::create(signal, GL_RED, 32, 1, mAudioFormat);
		}
	}
	else {
		unsigned char signal[mFFTWindowSize];
		float f = 0.0f;
		int ger = 0;
		for (size_t i{ 0 }; i < mFFTWindowSize; i++) {
			if (mUseRandom) {
				// generate random values
				f = (float)(i + 23);
			}
			else {
				// get freqs from Speckthor in VDRouter.cpp
				f = iFreqs[i];
			}
			ger = (int)f;
			signal[i] = static_cast<unsigned char>(ger);

			if (f > mVDUniforms->getUniformValue(mVDUniforms->IMAXVOLUME))
			{
				mVDUniforms->setUniformValue(mVDUniforms->IMAXVOLUME, f);
			}
			// update iFreq uniforms 
			if (i == getFreqIndex(0)) mVDUniforms->setUniformValue(mVDUniforms->IFREQ0, f);
			if (i == getFreqIndex(1)) mVDUniforms->setUniformValue(mVDUniforms->IFREQ1, f);
			if (i == getFreqIndex(2)) mVDUniforms->setUniformValue(mVDUniforms->IFREQ2, f);
			if (i == getFreqIndex(3)) mVDUniforms->setUniformValue(mVDUniforms->IFREQ3, f);
		}

		// store it as a 512x2 texture
		mAudioTexture = gl::Texture::create(signal, GL_RED, 32, 1, mAudioFormat);
		mAudioName = "audio";
	}

	return mAudioTexture;
};
void VDAnimation::resetAnim() {
	for (unsigned int anim{ 1 }; anim < 29; anim++)
	{
		mVDUniforms->setAnim(anim, mVDSettings->ANIM_NONE);
		mVDUniforms->setUniformValue(anim, mVDUniforms->getDefaultUniformValue(anim));
	}
}

void VDAnimation::update() {

	if (mBadTV[getElapsedFrames()] == 0) {
		// TODO check shaderUniforms["iBadTv"].floatValue = 0.0f;
	}
	else {
		// duration = 0.2
		//timeline().apply(&mVDSettings->iBadTv, 60.0f, 0.0f, 0.2f, EaseInCubic());
		mVDUniforms->setUniformValue(mVDUniforms->IBADTV, 5.0f);
	}

	mVDSettings->iChannelTime[0] = (float)getElapsedSeconds();
	mVDSettings->iChannelTime[1] = (float)(getElapsedSeconds() - 1.0);
	mVDSettings->iChannelTime[2] = (float)(getElapsedSeconds() - 2.0);
	mVDSettings->iChannelTime[3] = (float)(getElapsedSeconds() - 3.0);
	// ITIME
	/*if (mUseTimeWithTempo)
	{
		// Ableton Link from openframeworks SocketIO
		mVDUniforms->setUniformValue(mVDUniforms->ITIME,
			mVDUniforms->getUniformValue(mVDUniforms->ITIME)  *
			mVDUniforms->getUniformValue(mVDUniforms->ISPEED) *
			mVDUniforms->getUniformValue(mVDUniforms->ITIMEFACTOR));
		// sos
		// IBARBEAT = IBAR * 4 + IBEAT
		float current = mVDUniforms->getUniformValue(mVDUniforms->IBARBEAT); // 20210101 was int
		if (current == 426.0f || current == 428.0f || current == 442.0f) {
			mLastBar = 0.0f;
		} //38 to set iStart
		if (mLastBar != mVDUniforms->getUniformValue(mVDUniforms->IBAR)) {
			mLastBar = mVDUniforms->getUniformValue(mVDUniforms->IBAR);
			if (mLastBar < 419.0f && mLastBar > 424.0f) { mVDSettings->iStart = mVDUniforms->getUniformValue(mVDUniforms->ITIME); }
		}
	}
	else
	{*/
	mVDUniforms->setUniformValue(mVDUniforms->ITIME,
		((float)getElapsedSeconds() - mVDUniforms->getUniformValue(mVDUniforms->ISTART)) *
		mVDUniforms->getUniformValue(mVDUniforms->ISPEED) *
		mVDUniforms->getUniformValue(mVDUniforms->ITIMEFACTOR));
	// sos
	// IBARBEAT = IBAR * 4 + IBEAT
	float current = mVDUniforms->getUniformValue(mVDUniforms->IBARBEAT); // 20210101 was int
	if (current == 426.0f || current == 428.0f || current == 442.0f) {
		mLastBar = 0.0f;
	} //38 to set iStart
	if (mLastBar != mVDUniforms->getUniformValue(mVDUniforms->IBAR)) {
		mLastBar = mVDUniforms->getUniformValue(mVDUniforms->IBAR);
		if (mLastBar < 419.0f && mLastBar > 424.0f) { mVDSettings->iStart = mVDUniforms->getUniformValue(mVDUniforms->ITIME); }
	}
	//}
	// iResolution
	mVDUniforms->setVec3UniformValueByIndex(mVDUniforms->IRESOLUTION, vec3(mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONX), mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONY), 1.0));
	mVDUniforms->setVec2UniformValueByIndex(mVDUniforms->RESOLUTION, vec2(mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONX), mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONY)));
	mVDUniforms->setVec2UniformValueByIndex(mVDUniforms->RENDERSIZE, vec2(mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONX), mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONY)));
	mVDUniforms->setVec2UniformValueByIndex(mVDUniforms->IRENDERXY, vec2(mVDUniforms->getUniformValue(mVDUniforms->IRENDERXYX), mVDUniforms->getUniformValue(mVDUniforms->IRENDERXYY)));

	// iDate
	time_t now = time(0);
	struct tm* t = gmtime(&now);
	mVDUniforms->setVec4UniformValueByIndex(mVDUniforms->IDATE,
		vec4(float(t->tm_year + 1900), float(t->tm_mon + 1), float(t->tm_mday), float(t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec)));
	mVDUniforms->setUniformValue(mVDUniforms->IDATEX, float(t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec));
	mVDUniforms->setUniformValue(mVDUniforms->IDATEY, float(t->tm_hour + 2));
	mVDUniforms->setUniformValue(mVDUniforms->IDATEZ, float(t->tm_sec));
	mVDUniforms->setUniformValue(mVDUniforms->IDATEW, float((t->tm_hour + 2) * 3600 + t->tm_min * 60 + t->tm_sec));
	//CI_LOG_E("sec:" + toString(t->tm_sec) + ", IDATEY:" + toString(mVDUniforms->getUniformValue(mVDUniforms->IDATEY)) + ", IDATEZ:" + toString(mVDUniforms->getUniformValue(mVDUniforms->IDATEZ)) + ", IDATEW:" + toString(mVDUniforms->getUniformValue(mVDUniforms->IDATEW)));


#pragma region animation

	currentTime = mTimer.getSeconds();
	// TODO check bounds
	//if (mAutoBeatAnimation) mVDSettings->liveMeter = maxVolume * 2;

	int time = (int)((currentTime - startTime) * 1000000.0);

	int elapsed = (int)(mVDUniforms->getUniformValue(mVDUniforms->IDELTATIME) * 1000000.0);
	int elapsedBeatPerBar = (int)(mVDUniforms->getUniformValue(mVDUniforms->IDELTATIME) /
		(mVDUniforms->getUniformValue(mVDUniforms->IBEATSPERBAR) + 1) * 1000000.0);
	/*if (elapsedBeatPerBar > 0)
	{
		double moduloBeatPerBar = (time % elapsedBeatPerBar) / 1000000.0;
		iTempoTimeBeatPerBar = (float)moduloBeatPerBar;
		if (iTempoTimeBeatPerBar < previousTimeBeatPerBar)
		{
			if (shaderUniforms["iPhase"].intValue > shaderUniforms["iBeatsPerBar"].intValue ) shaderUniforms["iPhase"].intValue = 0;
			shaderUniforms["iPhase"].intValue++;
		}
		previousTimeBeatPerBar = iTempoTimeBeatPerBar;
	} */
	if (elapsed > 0)
	{
		double modulo = (time % elapsed) / 1000000.0;
		mVDUniforms->setUniformValue(mVDUniforms->ITEMPOTIME, (float)abs(modulo));

		/* not used shaderUniforms["iDeltaTime"].floatValue = (float)abs(modulo);
		if (shaderUniforms["iTempoTime"].floatValue < previousTime)
		{
			//iBar++;
			//if (mAutoBeatAnimation) mVDSettings->iPhase++;
		}*/
		previousTime = mVDUniforms->getUniformValue(mVDUniforms->ITEMPOTIME);
		// TODO (modulo < 0.1) ? tempoMvg->setNameColor(ColorA::white()) : tempoMvg->setNameColor(UIController::DEFAULT_NAME_COLOR);
		float targetValue = 1.0f;
		for (unsigned int anim{ 1 }; anim < 29; anim++)
		{
			switch (mVDUniforms->getUniformAnim(anim)) {
			case 1: // ANIM_TIME
				mVDUniforms->setUniformValue(anim, (modulo < 0.1f) ? mVDUniforms->getMaxUniformValue(anim) : mVDUniforms->getMinUniformValue(anim));
				break;
			case 2: // ANIM_AUTO
				mVDUniforms->setUniformValue(anim, lmap<float>(mVDUniforms->getUniformValue(mVDUniforms->ITEMPOTIME), 0.00001f,
					mVDUniforms->getUniformValue(mVDUniforms->IDELTATIME), mVDUniforms->getMinUniformValue(anim), mVDUniforms->getMaxUniformValue(anim)));
				break;
			case 3: // ANIM_BASS
				mVDUniforms->setUniformValue(anim, (mVDUniforms->getDefaultUniformValue(anim) + 0.01f) * mVDUniforms->getUniformValue(mVDUniforms->IFREQ0) / 25.0f);
				break;
			case 4: // ANIM_MID
				mVDUniforms->setUniformValue(anim, (mVDUniforms->getDefaultUniformValue(anim) + 0.01f) * mVDUniforms->getUniformValue(mVDUniforms->IFREQ1) / 5.0f);
				break;
			case 5: // ANIM_SMOOTH
				targetValue = mVDUniforms->getTargetUniformValue(anim);
				if (abs(targetValue - mVDUniforms->getUniformValue(anim)) <= 0.006f) {//mVDUniforms->getUniformValue(mVDUniforms->ISMOOTH)) {
					resetUniformAnim(anim);
				}
				else {
					if (mVDUniforms->getUniformValue(anim) > targetValue) {
						mVDUniforms->setUniformValue(anim, (mVDUniforms->getUniformValue(anim) - mVDUniforms->getUniformValue(mVDUniforms->ISMOOTH) / 27.0f));
					}
					else if (mVDUniforms->getUniformValue(anim) < targetValue) {
						mVDUniforms->setUniformValue(anim, (mVDUniforms->getUniformValue(anim) + mVDUniforms->getUniformValue(mVDUniforms->ISMOOTH) / 27.0f));
					}
					else {
						resetUniformAnim(anim);
					}
				}
				// ANIM_TREBLE mVDUniforms->setUniformValue(anim, (mVDUniforms->getDefaultUniformValue(anim) + 0.01f) * mVDUniforms->getUniformValue(mVDUniforms->IFREQ2) / 2.0f);
				break;
			default:
				// no animation
				break;
			}
		}

		// foreground color vec3 update
		mVDUniforms->setVec3UniformValueByIndex(mVDUniforms->ICOLOR, vec3(mVDUniforms->getUniformValue(mVDUniforms->ICOLORX), mVDUniforms->getUniformValue(mVDUniforms->ICOLORY), mVDUniforms->getUniformValue(mVDUniforms->ICOLORZ)));

		// background color vec3 update
		mVDUniforms->setVec3UniformValueByIndex(mVDUniforms->IBACKGROUNDCOLOR, vec3(mVDUniforms->getUniformValue(mVDUniforms->IBACKGROUNDCOLORX), mVDUniforms->getUniformValue(mVDUniforms->IBACKGROUNDCOLORY), mVDUniforms->getUniformValue(mVDUniforms->IBACKGROUNDCOLORZ)));

		// mouse vec4 update
		mVDUniforms->setVec4UniformValueByIndex(mVDUniforms->IMOUSE, vec4(mVDUniforms->getUniformValue(mVDUniforms->IMOUSEX), mVDUniforms->getUniformValue(mVDUniforms->IMOUSEY), mVDUniforms->getUniformValue(mVDUniforms->IMOUSEZ), mVDUniforms->getUniformValue(mVDUniforms->IMOUSEW)));

		// TODO migrate:
		if (mVDSettings->autoInvert)
		{
			mVDUniforms->setUniformValue(mVDUniforms->IINVERT, (modulo < 0.1) ? 1.0f : 0.0f);
		}
	}
#pragma endregion animation
}
void VDAnimation::resetUniformAnim(unsigned int anim) {
	mVDUniforms->setUniformValue(anim, mVDUniforms->getDefaultUniformValue(anim));
	mVDUniforms->setAnim(anim, mVDSettings->ANIM_NONE);
}
bool VDAnimation::toggleValue(unsigned int aIndex) {
	bool rtn = mVDUniforms->getUniformValue(aIndex);
	rtn = !rtn;
	mVDUniforms->setUniformValue(aIndex, rtn);
	//shaderUniforms[aIndex].boolValue = !shaderUniforms[aIndex].boolValue;
	return rtn;
}
// tempo
void VDAnimation::tapTempo()
{
	startTime = currentTime = mTimer.getSeconds();

	mTimer.stop();
	mTimer.start();

	// check for out of time values - less than 50% or more than 150% of from last "TAP and whole time buffer is going to be discarded....
	if (counter > 2 && (buffer.back() * 1.5 < currentTime || buffer.back() * 0.5 > currentTime))
	{
		buffer.clear();
		counter = 0;
		averageTime = 0;
	}
	if (counter >= 1)
	{
		buffer.push_back(currentTime);
		calculateTempo();
	}
	counter++;
}
void VDAnimation::calculateTempo()
{
	// NORMAL AVERAGE
	double tAverage = 0;
	for (int i{ 0 }; i < buffer.size(); i++)
	{
		tAverage += buffer[i];
	}
	averageTime = (double)(tAverage / buffer.size());
	mVDUniforms->setUniformValue(mVDUniforms->IDELTATIME, (float)averageTime);
	mVDUniforms->setUniformValue(mVDUniforms->IBPM, (float)(60.0 / averageTime));
}

void VDAnimation::preventLineInCrash() {
	setUseLineIn(false);
	//mVDSettings->save();
}
void VDAnimation::preventWaveMonitorCrash() {
	setUseWaveMonitor(false);
	//mVDSettings->save();
}
void VDAnimation::saveLineIn() {
	setUseLineIn(true);
	//mVDSettings->save();
}
void VDAnimation::saveWaveMonitor() {
	setUseWaveMonitor(true);
	//mVDSettings->save();
}