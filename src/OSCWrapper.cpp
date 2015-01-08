#include "OSCWrapper.h"

using namespace Reymenta;

OSC::OSC(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTextures)
{
	mParameterBag = aParameterBag;
	mShaders = aShadersRef;
	mTextures = aTextures;
	mParameterBag = aParameterBag;
	for (int i = 0; i < 20; i++)
	{
		skeleton[i] = ivec4(0);
	}
	// ableton liveOSC
	for (int a = 0; a < MAX; a++)
	{
		tracks[a] = "default.glsl";
	}
	// OSC sender
	mOSCSender.setup(mParameterBag->mOSCDestinationHost, mParameterBag->mOSCDestinationPort);
	// OSC receiver
	mOSCReceiver.setup(mParameterBag->mOSCReceiverPort);
}

OSCRef OSC::create(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTextures)
{
	return shared_ptr<OSC>(new OSC(aParameterBag, aShadersRef, aTextures));
}

void OSC::setupSender()
{
	// OSC sender
	mOSCSender.setup(mParameterBag->mOSCDestinationHost, mParameterBag->mOSCDestinationPort);
}

void OSC::update()
{
	/*	while( receiver.hasWaitingMessages() ) {
	osc::Message m;
	receiver.getNextMessage( &m );

	console() << "New message received" << std::endl;
	console() << "Address: " << m.getAddress() << std::endl;
	console() << "Num Arg: " << m.getNumArgs() << std::endl;
	// check for mouse moved message
	if(m.getAddress() == "/mouse/position"){
	// both the arguments are int32's
	ivec2 pos = ivec2( m.getArgAsInt32(0), m.getArgAsInt32(1));
	vec2 mouseNorm = vec2( pos ) / getWindowSize();
	vec2 mouseVel = vec2( pos - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, true, true );
	pMouse = pos;
	if ( m.getArgAsInt32(2) == 1 )
	{
	mMouseDown = true;
	}
	else
	{
	mMouseDown = false;
	}
	if ( mMouseDown )
	{
	mArcball.mouseDown( pos );
	mCurrentMouseDown = mInitialMouseDown = pos;
	}
	}
	// check for mouse button message
	else if(m.getAddress() == "/mouse/button"){
	// the single argument is a string
	ivec2 pos = ivec2( m.getArgAsInt32(0), m.getArgAsInt32(1));
	mArcball.mouseDown( pos );
	mCurrentMouseDown = mInitialMouseDown = pos;
	if ( m.getArgAsInt32(2) == 1 )
	{
	mMouseDown = true;
	}
	else
	{
	mMouseDown = false;
	}
	}
	else if(m.getAddress() == "/fluid/drawfluid"){
	drawFluid = !drawFluid;
	}
	else if(m.getAddress() == "/fluid/drawfluidtex"){
	drawFluidTex = !drawFluidTex;
	}
	else if(m.getAddress() == "/fluid/drawparticles"){
	drawParticles = ! drawParticles;
	}
	else if(m.getAddress() == "/fluid/randomizecolor"){
	fluidSolver.randomizeColor();
	}
	else if(m.getAddress() == "/window/position"){
	// window position
	setWindowPos(m.getArgAsInt32(0), m.getArgAsInt32(1));
	}
	else if(m.getAddress() == "/window/setfullscreen"){
	// fullscreen
	//setFullScreen( ! isFullScreen() );
	}
	else if(m.getAddress() == "/quit"){
	quitProgram();
	}
	else{
	// unrecognized message
	//cout << "not recognized:" << m.getAddress() << endl;

	}

	}*/
	// osc
	while (mOSCReceiver.hasWaitingMessages())
	{
		osc::Message message;
		mOSCReceiver.getNextMessage(&message);
		if (mParameterBag->mIsOSCSender) mOSCSender.sendMessage(message);
		for (int a = 0; a < MAX; a++)
		{
			iargs[a] = 0;
			fargs[a] = 0.0;
			sargs[a] = "";
		}
		int skeletonIndex = 0;
		int jointIndex = 0;
		string oscAddress = message.getAddress();

		int numArgs = message.getNumArgs();
		// get arguments
		for (int i = 0; i < message.getNumArgs(); i++)
		{
			if (i < MAX)
			{

				if (message.getArgType(i) == osc::TYPE_INT32) {
					try
					{
						iargs[i] = message.getArgAsInt32(i);
						sargs[i] = toString(iargs[i]);
					}
					catch (...) {
						cout << "Exception reading argument as int32" << std::endl;
					}
				}
				if (message.getArgType(i) == osc::TYPE_FLOAT) {
					try
					{
						fargs[i] = message.getArgAsFloat(i);
						sargs[i] = toString(fargs[i]);
					}
					catch (...) {
						cout << "Exception reading argument as float" << std::endl;
					}
				}
				if (message.getArgType(i) == osc::TYPE_STRING) {
					try
					{
						sargs[i] = message.getArgAsString(i);
					}
					catch (...) {
						cout << "Exception reading argument as string" << std::endl;
					}
				}
			}
		}

		if (oscAddress == "/cc")
		{
			mParameterBag->controlValues[iargs[0]] = fargs[1];
			if (iargs[0] == 41) mTextures->playMovie();
			if (iargs[0] == 42) mTextures->stopMovie();
		}
		else if (oscAddress == "/fs")
		{
			mShaders->loadFragmentShaderString(sargs[1], sargs[2]);
			mTextures->warpInputs[0].rightIndex = 0;
			mTextures->warpInputs[0].rightMode = 1;
			mTextures->warpInputs[0].iCrossfade = 1.0;
		}
		else if (oscAddress == "/fspath")
		{
			mShaders->loadPixelFragmentShader(sargs[1]);
		}
		else if (oscAddress == "/live/beat")
		{
			mParameterBag->mBeat = iargs[0];
			if (mParameterBag->mBeat == 476)
			{
				// stop movie and remove it
				mTextures->stopMovie(true);
			}
		}
		else if (oscAddress == "/live/tempo")
		{
			mParameterBag->mTempo = fargs[0];
			if (mParameterBag->mTempo == 88.0)
			{
				mTextures->loadFileFromAssets(tracks[0]);
			}

		}
		else if (oscAddress == "/live/play")
		{
			mParameterBag->mIsPlaying = (iargs[0] == 2);
		}
		else if (oscAddress == "/live/track/meter")
		{
			mParameterBag->maxVolume = fargs[2];
		}
		else if (oscAddress == "/live/name/trackblock")
		{
			// ableton liveOSC
			for (int a = 0; a < MAX; a++)
			{
				tracks[a] = sargs[a];
			}
		}
		else if (oscAddress == "/reymenta/tracklist")
		{
			// from midi2OSC
			for (int a = 0; a < MAX; a++)
			{
				if (sargs[a] == "")
				{
					tracks[a] = "default.glsl";
				}
				else
				{
					tracks[a] = sargs[a];
				}
			}
		}
		else if (oscAddress == "/texture")
		{
			mTextures->setInput(iargs[0], iargs[1], iargs[2]);
			/*
					else if (oscAddress == "/createwarps")
		{
			mWarpings->createWarps(iargs[0]);
		}
		else if (oscAddress == "/select")
		{
			mWarpings->setSelectedWarp(iargs[0]);
		}
		else if (oscAddress == "/channel")
		{
			mWarpings->setChannelForSelectedWarp(iargs[0]);
		}
		else if (oscAddress == "/crossfade")
		{
			mWarpings->setCrossfadeForSelectedWarp(fargs[0]);
		}

			*/
		}
		else if (oscAddress == "/sumMovement")
		{
			float sumMovement = fargs[0];
			//exposure
			mParameterBag->controlValues[14] = sumMovement;
			//greyScale
			if (sumMovement < 0.1)
			{
				mParameterBag->iGreyScale = 1.0f;
			}
			else
			{
				mParameterBag->iGreyScale = 0.0f;
			}
		}
		else if (oscAddress == "/handsHeadHeight")
		{
			float handsHeadHeight = fargs[0];
			if (handsHeadHeight > 0.3)
			{
				// glitch
				mParameterBag->controlValues[45] = 1.0f;
			}
			else
			{
				// glitch
				mParameterBag->controlValues[45] = 0.0f;
			}
			// background red
			mParameterBag->controlValues[5] = handsHeadHeight*3.0;
		}
		else if (oscAddress == "/centerXY")
		{
			float x = fargs[0];
			float y = fargs[1];
			// background green
			mParameterBag->controlValues[6] = y;
			// green
			mParameterBag->controlValues[2] = x;
		}

		else if (oscAddress == "/joint")
		{
			skeletonIndex = iargs[0];
			jointIndex = iargs[1];
			if (jointIndex < 20)
			{
				skeleton[jointIndex] = ivec4(iargs[2], iargs[3], iargs[4], iargs[5]);
			}
		}
		else
		{
			//console() << "OSC message received: " << oscAddress << std::endl;
			// is it a layer msg?
			int layer = 0;
			unsigned layerFound = oscAddress.find("layer");
			if (layerFound == 1)
			{
				unsigned clipFound = oscAddress.find("/clip");
				if (clipFound == 7) // layer must be < 10
				{
					cout << "clipFound " << clipFound;
					layer = atoi(oscAddress.substr(6, 1).c_str());
					int clip = atoi(oscAddress.substr(12, 1).c_str());
					string fileName = toString((layer * 10) + clip) + ".fragjson";
					fs::path fragFile = getAssetPath("") / "shaders" / "fragjson" / fileName;
					if (fs::exists(fragFile))
					{
						//mShaders->loadFragJson(fragFile.string());
					}
				}
				else
				{
					if (clipFound == 8)
					{
						layer = atoi(oscAddress.substr(6, 2).c_str());
					}
				}
				// connect or preview
				unsigned connectFound = oscAddress.find("connect");
				if (connectFound != string::npos) cout << "connectFound " << connectFound;
			}
			//if ( layerFound != string::npos ) cout << "layerFound " << layerFound;

			unsigned found = oscAddress.find_last_of("/");
			int name = atoi(oscAddress.substr(found + 1).c_str());
			stringstream ss;
			ss << message.getRemoteIp() << " adr:" << oscAddress << " 0: " << sargs[0] << " 1: " << sargs[1] << " 2: " << sargs[2] << " 3: " << sargs[3] << " 4: " << sargs[4] << " 5: " << sargs[5] << std::endl;
			mParameterBag->OSCMsg = ss.str();
			mParameterBag->newOSCMsg = true;
		}
	}
}

void OSC::sendOSCIntMessage(string controlType, int iarg0, int iarg1, int iarg2, int iarg3, int iarg4, int iarg5)
{
	osc::Message m;
	m.setAddress(controlType);
	m.addIntArg(iarg0);
	m.addIntArg(iarg1);
	m.addIntArg(iarg2);
	m.addIntArg(iarg3);
	m.addIntArg(iarg4);
	m.addIntArg(iarg5);
	mOSCSender.sendMessage(m);
}
void OSC::sendOSCStringMessage(string controlType, int iarg0, string sarg1, string sarg2, string sarg3, string sarg4, string sarg5)
{
	osc::Message m;
	m.setAddress(controlType);
	m.addIntArg(iarg0);
	if (sarg1 != "") m.addStringArg(sarg1);
	if (sarg2 != "") m.addStringArg(sarg2);
	if (sarg3 != "") m.addStringArg(sarg3);
	if (sarg4 != "") m.addStringArg(sarg4);
	if (sarg5 != "") m.addStringArg(sarg5);
	mOSCSender.sendMessage(m);
}
void OSC::sendOSCFloatMessage(string controlType, int iarg0, float farg1, float farg2, float farg3, float farg4, float farg5)
{
	osc::Message m;
	m.setAddress(controlType);
	m.addIntArg(iarg0);
	m.addFloatArg(farg1);
	m.addFloatArg(farg2);
	m.addFloatArg(farg3);
	m.addFloatArg(farg4);
	m.addFloatArg(farg5);
	mOSCSender.sendMessage(m);
}