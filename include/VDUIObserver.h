/*
	VDUIObserver
	Mediator UI Observer
*/
// TODO 

#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
// Settings
#include "VDSettings.h"
// Uniforms
#include "VDUniforms.h"
// Mediator
#include "VDMediator.h"

#include <memory>
#include <vector>

using namespace ci;
using namespace ci::app;

namespace videodromm {
	
	class VDUIObserver : public VDUniformObserver {
	public:
		static VDUniformObserverRef connect(VDSettingsRef aVDSettings, VDUniformsRef aVDUniforms) {
			VDUIObserver* o = new VDUIObserver(aVDSettings, aVDUniforms);
			
			VDUniformObserverRef obs(o);

			return obs;
		}
		VDUniformObserverRef setUniformValue(int aIndex, float aValue) {
			mVDUniforms->setUniformValue(aIndex, aValue);
			return shared_from_this();
		}
		
		
		bool mIsConnected = false;
		~VDUIObserver() {  };
	private:
		VDUIObserver(VDSettingsRef aVDSettings, VDUniformsRef aVDUniforms) {
			CI_LOG_V("VDUIObserver ctor");
			mVDSettings = aVDSettings;
			mVDUniforms = aVDUniforms;
		}
		// Settings
		VDSettingsRef	mVDSettings;
		// Uniforms
		VDUniformsRef	mVDUniforms;
	};
}
