#include "MessageRouter.h"

using namespace Reymenta;

MessageRouter::MessageRouter(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;

}

MessageRouterRef MessageRouter::create(ParameterBagRef aParameterBag)
{
	return shared_ptr<MessageRouter>(new MessageRouter(aParameterBag));
}
void MessageRouter::setup(WebSocketsRef aWebSockets, OSCRef aOSC, MIDIRef aMIDI) {
	mWebSockets = aWebSockets;
	mOSC = aOSC;
	mMIDI = aMIDI;
}

void MessageRouter::sendJSON(string params) {
	mWebSockets->write(params);
	if (mParameterBag->mOSCEnabled) {

	}
}

void MessageRouter::updateParams(int iarg0, float farg1)
{
	if (farg1 > 0.1)
	{
		//avoid to send twice
		if (iarg0 == 54) mOSC->sendOSCIntMessage("/live/play", 0);			// play
		if (iarg0 == 53) mOSC->sendOSCIntMessage("/live/stop", 0);			// stop
		if (iarg0 == 52) mOSC->sendOSCIntMessage("/live/next/cue", 0);		// next cue
		if (iarg0 == 51) mOSC->sendOSCIntMessage("/live/prev/cue", 0);		// previous cue
	}
	if (iarg0 > 0 && iarg0 < 9)
	{
		// sliders 
		mParameterBag->controlValues[iarg0] = farg1;
	}
	if (iarg0 > 10 && iarg0 < 19)
	{
		// rotary 
		mParameterBag->controlValues[iarg0] = farg1;
		// audio multfactor
		if (iarg0 == 13) mParameterBag->controlValues[iarg0] = (farg1 + 0.01) * 10;
		// exposure
		if (iarg0 == 14) mParameterBag->controlValues[iarg0] = (farg1 + 0.01) * maxExposure;
	}
	// buttons
	if (iarg0 > 20 && iarg0 < 29)
	{
		// select index
		mParameterBag->selectedWarp = iarg0 - 21;
	}
	if (iarg0 > 30 && iarg0 < 39)
	{
		// select input
		mParameterBag->mWarpFbos[mParameterBag->selectedWarp].textureIndex = iarg0 - 31;
		// activate
		mParameterBag->mWarpFbos[mParameterBag->selectedWarp].active = !mParameterBag->mWarpFbos[mParameterBag->selectedWarp].active;
	}
	if (iarg0 > 40 && iarg0 < 49)
	{
		// low row 
		mParameterBag->controlValues[iarg0] = farg1;
	}
	if (iarg0 == 61 && farg1 > 0.1)
	{
		// left arrow
		mParameterBag->iBlendMode--;
		if (mParameterBag->iBlendMode < 0) mParameterBag->iBlendMode = mParameterBag->maxBlendMode;
	}
	if (iarg0 == 62 && farg1 > 0.1)
	{
		// left arrow
		mParameterBag->iBlendMode++;
		if (mParameterBag->iBlendMode > mParameterBag->maxBlendMode) mParameterBag->iBlendMode = 0;
	}

}