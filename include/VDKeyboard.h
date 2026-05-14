/*
	VDKeyboard

*/

#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
// #include "cinder/Json.h"

// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// Uniforms
#include "VDUniforms.h"
// Mediator
#include "VDMediator.h"

using namespace ci;
using namespace ci::app;

namespace videodromm
{
	class VDMediatorObservable;
	typedef std::shared_ptr<VDMediatorObservable> VDMediatorObservableRef;

	class VDKeyboard;
	// stores the pointer to the VDKeyboard instance
	typedef std::shared_ptr<VDKeyboard> VDKeyboardRef;
	class VDKeyboard {
	public:
		VDKeyboard(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms);
		static VDKeyboardRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms);

		void								setupKeyboard(VDMediatorObservableRef aVDMediator);
		bool								handleKeyDown(KeyEvent& event);
		bool								handleKeyUp(KeyEvent& event);
	private:
		// Settings
		VDSettingsRef						mVDSettings;
		// Animation
		VDAnimationRef						mVDAnimation;
		// Uniforms
		VDUniformsRef						mVDUniforms;
		//Mediator
		VDMediatorObservableRef				mVDMediator;

	};
}

