/*
 Copyright (c) 2013-2026, Bruce Lane - All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Using Cinder-Warping from Paul Houx.

 Cinder-Warping is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Cinder-Warping is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Cinder-Warping.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

// Animation
#include "VDAnimation.h"
// Session Facade
#include "VDSessionFacade.h"
// Spout
#include "CiSpoutOut.h"
// Uniforms
#include "VDUniforms.h"
// Params
#include "VDParams.h"
// Mix
#include "VDMix.h"


using namespace ci;
using namespace ci::app;
using namespace videodromm;

class _TBOX_PREFIX_App : public App {
public:
	_TBOX_PREFIX_App();
	void cleanup() override;
void update() override;
void draw() override;
void resize() override;
void mouseMove(MouseEvent event) override;
void mouseDown(MouseEvent event) override;
void mouseDrag(MouseEvent event) override;
void mouseUp(MouseEvent event) override;
void keyDown(KeyEvent event) override;
void keyUp(KeyEvent event) override;
void fileDrop(FileDropEvent event) override;
private:
	// Settings
VDSettingsRef					mVDSettings;
// Animation
VDAnimationRef					mVDAnimation;
// Session
VDSessionFacadeRef				mVDSessionFacade;
// Mix
VDMixRef						mVDMix;
// Uniforms
VDUniformsRef					mVDUniforms;
// Params
VDParamsRef						mVDParams;


bool							mFadeInDelay = true;
void							toggleCursorVisibility(bool visible);
SpoutOut 						mSpoutOut;
};


_TBOX_PREFIX_App::_TBOX_PREFIX_App() : mSpoutOut("VDRUI", app::getWindowSize())
{

	// Settings
mVDSettings = VDSettings::create("VDRUI");
// Uniform
mVDUniforms = VDUniforms::create();
// Params
mVDParams = VDParams::create();
// Animation
mVDAnimation = VDAnimation::create(mVDSettings, mVDUniforms);
// Mix
mVDMix = VDMix::create(mVDSettings, mVDAnimation, mVDUniforms);
// Session
mVDSessionFacade = VDSessionFacade::createVDSession(mVDSettings, mVDAnimation, mVDUniforms, mVDMix)
	->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::POST)
	->setupSession()
	//->setupWSClient()
	//->wsConnect()
	//->setupOSCReceiver()
	//->addOSCObserver(mVDSettings->mOSCDestinationHost, mVDSettings->mOSCDestinationPort)
	->addUIObserver(mVDSettings, mVDUniforms)
	->toggleUI()
	->setUniformValue(mVDUniforms->IBPM, 160.0f)
	->setUniformValue(mVDUniforms->IMOUSEX, 0.27710f)
	->setUniformValue(mVDUniforms->IMOUSEY, 0.5648f);

mFadeInDelay = true;

}

void _TBOX_PREFIX_App::toggleCursorVisibility(bool visible)
{
	if (visible)
	{
		showCursor();
	}
	else
	{
		hideCursor();
	}
}

void _TBOX_PREFIX_App::fileDrop(FileDropEvent event)
{
	mVDSessionFacade->fileDrop(event);
}

void _TBOX_PREFIX_App::mouseMove(MouseEvent event)
{
	if (!mVDSessionFacade->handleMouseMove(event)) {

	}
}

void _TBOX_PREFIX_App::mouseDown(MouseEvent event)
{

	if (!mVDSessionFacade->handleMouseDown(event)) {

	}
}

void _TBOX_PREFIX_App::mouseDrag(MouseEvent event)
{

	if (!mVDSessionFacade->handleMouseDrag(event)) {

	}
}

void _TBOX_PREFIX_App::mouseUp(MouseEvent event)
{

	if (!mVDSessionFacade->handleMouseUp(event)) {

	}
}

void _TBOX_PREFIX_App::keyDown(KeyEvent event)
{

	// warp editor did not handle the key, so handle it here
	if (!mVDSessionFacade->handleKeyDown(event)) {
		switch (event.getCode()) {
		case KeyEvent::KEY_F12:
			// quit the application
			quit();
			break;
		case KeyEvent::KEY_f:
			// toggle full screen
			setFullScreen(!isFullScreen());
			break;

		case KeyEvent::KEY_l:
			mVDSessionFacade->createWarp();
			break;
		}
	}
}

void _TBOX_PREFIX_App::keyUp(KeyEvent event)
{

	// let your application perform its keyUp handling here
	if (!mVDSessionFacade->handleKeyUp(event)) {
		
	}
}
void _TBOX_PREFIX_App::cleanup()
{
	CI_LOG_V("cleanup and save");

	mVDSessionFacade->saveWarps();
	mVDSettings->save();
	CI_LOG_V("quit");
}

void _TBOX_PREFIX_App::update()
{
	mVDSessionFacade->setUniformValue(mVDUniforms->IFPS, getAverageFps());
	mVDSessionFacade->update();
}


void _TBOX_PREFIX_App::resize()
{
	
}
void _TBOX_PREFIX_App::draw()
{
	// clear the window and set the drawing color to white
gl::clear();
gl::color(Color::white());
if (mFadeInDelay) {
	mVDSettings->iAlpha = 0.0f;
	if (getElapsedFrames() > 10.0) {// mVDSessionFacade->getFadeInDelay()) {
		mFadeInDelay = false;
		timeline().apply(&mVDSettings->iAlpha, 0.0f, 1.0f, 1.5f, EaseInCubic());
	}
}
else {
	gl::setMatricesWindow(mVDParams->getFboWidth(), mVDParams->getFboHeight());
	
	int m = mVDSessionFacade->getUniformValue(mVDUniforms->IDISPLAYMODE);
	if (m == VDDisplayMode::MIXETTE) {
		gl::draw(mVDSessionFacade->buildRenderedMixetteTexture(0));
		mSpoutOut.sendTexture(mVDSessionFacade->buildRenderedMixetteTexture(0));
	}
	else if (m == VDDisplayMode::POST) {
		gl::draw(mVDSessionFacade->buildPostFboTexture());
		mSpoutOut.sendTexture(mVDSessionFacade->buildPostFboTexture());
	}
	else if (m == VDDisplayMode::FX) {
		gl::draw(mVDSessionFacade->buildFxFboTexture());
		mSpoutOut.sendTexture(mVDSessionFacade->buildFxFboTexture());
	}
	else {
		if (m < mVDSessionFacade->getFboShaderListSize()) {
			gl::draw(mVDSessionFacade->getFboShaderTexture(m));
			mSpoutOut.sendTexture(mVDSessionFacade->getFboShaderTexture(m));
		}
		else {
			gl::draw(mVDSessionFacade->buildRenderedMixetteTexture(0), Area(50, 50, mVDParams->getFboWidth() / 2, mVDParams->getFboHeight() / 2));
			gl::draw(mVDSessionFacade->buildPostFboTexture(), Area(mVDParams->getFboWidth() / 2, mVDParams->getFboHeight() / 2, mVDParams->getFboWidth(), mVDParams->getFboHeight()));
		}
		//gl::draw(mVDSession->getRenderedMixetteTexture(0), Area(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
		// ok gl::draw(mVDSession->getWarpFboTexture(), Area(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));//getWindowBounds()
	}
}	

getWindow()->setTitle(toString((int)getAverageFps()) + " fps");

}
void prepareSettings(App::Settings *settings)
{
	settings->setWindowSize(1280, 720);
}
CINDER_APP(_TBOX_PREFIX_App, RendererGl(RendererGl::Options().msaa(8)),  prepareSettings)
