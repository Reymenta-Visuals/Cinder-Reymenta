#include "VDKeyboard.h"

using namespace videodromm;

VDKeyboardRef VDKeyboard::create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms)
{
	return std::shared_ptr<VDKeyboard>(new VDKeyboard(aVDSettings, aVDAnimation, aVDUniforms));
}

VDKeyboard::VDKeyboard(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms) {
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDUniforms = aVDUniforms;
	CI_LOG_V("VDKeyboard constructor");
}

void VDKeyboard::setupKeyboard(VDMediatorObservableRef aVDMediator) {
	mVDMediator = aVDMediator;
}

bool VDKeyboard::handleKeyDown(KeyEvent& event) {

	bool handled = true;
	float newValue;
#if defined( CINDER_COCOA )
	bool isModDown = event.isMetaDown();
#else // windows
	bool isModDown = event.isControlDown();
#endif
	bool isShiftDown = event.isShiftDown();
	bool isAltDown = event.isAltDown();
	float delta = 0.0f;
	if (isShiftDown) { delta = 0.05f; };
	if (isModDown) { delta = -0.05f; };
	CI_LOG_V("VDKeyboard keydown: " + toString(event.getCode()) + " ctrl: " + toString(isModDown) + " shift: " + toString(isShiftDown) + " alt: " + toString(isAltDown));
	// pass this key event to the warp editor first

	switch (event.getCode()) {
	case KeyEvent::KEY_m:
		// blendmodes
		if (isShiftDown) {
			mVDMediator->setUniformValue(mVDUniforms->IBLENDMODE, math<float>::max(mVDUniforms->getUniformValue(mVDUniforms->IBLENDMODE) + 1.0f, 0.0f));
		}
		else if (isModDown) {
			mVDMediator->setUniformValue(mVDUniforms->IBLENDMODE, math<float>::max(mVDUniforms->getUniformValue(mVDUniforms->IBLENDMODE) - 1.0f, 0.0f));
		}
		break;
	case KeyEvent::KEY_x:
		// trixels
		mVDMediator->setUniformValue(mVDUniforms->ITRIXELS, math<float>::max(mVDUniforms->getUniformValue(mVDUniforms->ITRIXELS) + delta, 0.0f));
		break;
	case KeyEvent::KEY_r:
		if (isAltDown) {
			mVDMediator->setUniformValue(mVDUniforms->IBACKGROUNDCOLORX, math<float>::max(mVDUniforms->getUniformValue(mVDUniforms->IBACKGROUNDCOLORX) + delta, 0.0f));
		}
		else {
			mVDMediator->setUniformValue(mVDUniforms->ICOLORX, math<float>::max(mVDUniforms->getUniformValue(mVDUniforms->ICOLORX) + delta, 0.0f));
		}
		break;
	case KeyEvent::KEY_g:
		if (isAltDown) {
			mVDMediator->setUniformValue(mVDUniforms->IBACKGROUNDCOLORY, math<float>::max(mVDUniforms->getUniformValue(mVDUniforms->IBACKGROUNDCOLORY) + delta, 0.0f));
		}
		else {
			mVDMediator->setUniformValue(mVDUniforms->ICOLORY, math<float>::max(mVDUniforms->getUniformValue(mVDUniforms->ICOLORY) + delta, 0.0f));
		}
		break;
	case KeyEvent::KEY_b:
		if (isAltDown) {
			mVDMediator->setUniformValue(mVDUniforms->IBACKGROUNDCOLORZ, math<float>::max(mVDUniforms->getUniformValue(mVDUniforms->IBACKGROUNDCOLORZ) + delta, 0.0f));
		}
		else {
			mVDMediator->setUniformValue(mVDUniforms->ICOLORZ, math<float>::max(mVDUniforms->getUniformValue(mVDUniforms->ICOLORZ) + delta, 0.0f));
		}
		break;
	case KeyEvent::KEY_a:
		mVDMediator->setUniformValue(mVDUniforms->IALPHA, math<float>::max(mVDUniforms->getUniformValue(mVDUniforms->IALPHA) + delta, 0.0f));
		break;
	case KeyEvent::KEY_u:
		// chromatic
		// TODO find why can't put value >0.9 or 0.85!
		newValue = math<float>::max(mVDUniforms->getUniformValue(mVDUniforms->ICHROMATIC) + delta, 0.0f);
		if (newValue > 1.0f) newValue = 0.0f;
		mVDMediator->setUniformValue(mVDUniforms->ICHROMATIC, newValue);
		break;
	case KeyEvent::KEY_p:
		// pixelate
		mVDMediator->setUniformValue(mVDUniforms->IPIXELATE, math<float>::max(mVDUniforms->getUniformValue(mVDUniforms->IPIXELATE) + delta, 0.0f));
		break;
	case KeyEvent::KEY_y:
		// glitch
		mVDMediator->setUniformValue(mVDUniforms->IGLITCH, true);
		break;
	case KeyEvent::KEY_i:
		// invert
		mVDMediator->setUniformValue(mVDUniforms->IINVERT, true);
		break;
	case KeyEvent::KEY_o:
		// toggle
		mVDMediator->setUniformValue(mVDUniforms->ITOGGLE, true);
		break;
	case KeyEvent::KEY_z:
		// zoom
		mVDMediator->setUniformValue(mVDUniforms->IZOOM, math<float>::max(mVDUniforms->getUniformValue(mVDUniforms->IZOOM) + delta, 0.0f));
		break;
		/* removed temp for Sky Project case KeyEvent::KEY_LEFT:
			//mVDTextures->rewindMovie();
			if (mVDAnimation->getUniformValue(21) > 0.1f) mVDSocketio->changeFloatValue(21, mVDAnimation->getUniformValue(21) - 0.1f);
			break;
		case KeyEvent::KEY_RIGHT:
			//mVDTextures->fastforwardMovie();
			if (mVDAnimation->getUniformValue(21) < 1.0f) mVDSocketio->changeFloatValue(21, mVDAnimation->getUniformValue(21) + 0.1f);
			break;*/
	case KeyEvent::KEY_PAGEDOWN:
	case KeyEvent::KEY_RIGHT:
		// crossfade right
		if (mVDUniforms->getUniformValue(mVDUniforms->IXFADE) < 1.0f) {
			mVDMediator->setUniformValue(mVDUniforms->IXFADE, mVDUniforms->getUniformValue(mVDUniforms->IXFADE) + 0.01f);
		};
		break;
	case KeyEvent::KEY_PAGEUP:
	case KeyEvent::KEY_LEFT:
		// crossfade left
		if (mVDUniforms->getUniformValue(mVDUniforms->IXFADE) > 0.0f) {
			mVDMediator->setUniformValue(mVDUniforms->IXFADE, mVDUniforms->getUniformValue(mVDUniforms->IXFADE) - 0.01f);
		}
		break;
	case KeyEvent::KEY_UP:
		// imgseq next
		//incrementSequencePosition();
		break;
	case KeyEvent::KEY_DOWN:
		// imgseq next
		//decrementSequencePosition();
		break;
	case KeyEvent::KEY_v:
		//if (isModDown) fboFlipV(0);// TODO other indexes mVDSettings->mFlipV = !mVDSettings->mFlipV; useless?
		break;
	case KeyEvent::KEY_F1:
		mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::POST);
		break;
	case KeyEvent::KEY_F2:
		mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::MIXETTE);
		break;
	case KeyEvent::KEY_F3:
		mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FX);
		break;
	case KeyEvent::KEY_F4:
		mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FBO0);
		break;
	case KeyEvent::KEY_F5:
		mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FBO1);
		break;
	case KeyEvent::KEY_F6:
		mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FBO2);
		break;
	case KeyEvent::KEY_F7:
		mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FBO3);
		break;
	case KeyEvent::KEY_F8:
		mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FBO4);
		break;
	case KeyEvent::KEY_F9:
		mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FBO5);
		break;
	case KeyEvent::KEY_F10: // pauses app in debug mode
		mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::WARP);
		break;
	default:
		CI_LOG_V("session keydown: " + toString(event.getCode()));
		handled = false;
		break;
	}

	CI_LOG_V((handled ? "VDKeyboard keydown handled " : "VDKeyboard keydown not handled "));
	event.setHandled(handled);
	return event.isHandled();
}
bool VDKeyboard::handleKeyUp(KeyEvent& event) {
	bool handled = true;
#if defined( CINDER_COCOA )
	bool isModDown = event.isMetaDown();
#else // windows
	bool isModDown = event.isControlDown();
#endif

	// Animation did not handle the key, so handle it here
	switch (event.getCode()) {
	case KeyEvent::KEY_y:
		// glitch
		mVDMediator->setUniformValue(mVDUniforms->IGLITCH, false);
		break;
	case KeyEvent::KEY_t:
		// trixels
		mVDMediator->setUniformValue(mVDUniforms->ITRIXELS, 0.0f);
		break;
	case KeyEvent::KEY_i:
		// invert
		mVDMediator->setUniformValue(mVDUniforms->IINVERT, false);
		break;
	case KeyEvent::KEY_u:
		// chromatic
		mVDMediator->setUniformValue(mVDUniforms->ICHROMATIC, 0.0f);
		break;
	case KeyEvent::KEY_p:
		// pixelate
		mVDMediator->setUniformValue(mVDUniforms->IPIXELATE, 1.0f);
		break;
	case KeyEvent::KEY_o:
		// toggle
		mVDMediator->setUniformValue(mVDUniforms->ITOGGLE, false);
		break;
	case KeyEvent::KEY_z:
		// zoom
		mVDMediator->setUniformValue(mVDUniforms->IZOOM, 1.0f);
		break;
	default:
		CI_LOG_V("VDKeyboard keyup: " + toString(event.getCode()));
		handled = false;
		break;
	}

	CI_LOG_V((handled ? "VDKeyboard keyup handled " : "VDKeyboard keyup not handled "));
	event.setHandled(handled);
	return event.isHandled();
}

