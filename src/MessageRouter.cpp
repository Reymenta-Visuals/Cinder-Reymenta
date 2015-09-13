#include "MessageRouter.h"

using namespace Reymenta;

MessageRouter::MessageRouter(ParameterBagRef aParameterBag, TexturesRef aTexturesRef, ShadersRef aShadersRef)
{
	mParameterBag = aParameterBag;
	mTextures = aTexturesRef;
	mShaders = aShadersRef;
	for (int i = 0; i < 20; i++)
	{
		skeleton[i] = Vec4i::zero();
	}
	// OSC sender with broadcast = true
	mOSCSender.setup(mParameterBag->mOSCDestinationHost, mParameterBag->mOSCDestinationPort, true);
	mOSCSender2.setup(mParameterBag->mOSCDestinationHost2, mParameterBag->mOSCDestinationPort2, true);
	// OSC receiver
	mOSCReceiver.setup(mParameterBag->mOSCReceiverPort);
	// ws
	clientConnected = false;
	if (mParameterBag->mAreWebSocketsEnabledAtStartup) wsConnect();
	mPingTime = getElapsedSeconds();

}

MessageRouterRef MessageRouter::create(ParameterBagRef aParameterBag, TexturesRef aTexturesRef, ShadersRef aShadersRef)
{
	return shared_ptr<MessageRouter>(new MessageRouter(aParameterBag, aTexturesRef, aShadersRef));
}
void MessageRouter::shutdown() {
	mMidiIn0.closePort();
	mMidiIn1.closePort();
	mMidiIn2.closePort();
}
void MessageRouter::midiSetup()
{
	stringstream ss;
	ss << "setupMidi: ";

	if (mMidiIn0.getNumPorts() > 0)
	{
		mMidiIn0.listPorts();
		for (int i = 0; i < mMidiIn0.getNumPorts(); i++)
		{
			bool alreadyListed = false;
			for (int j = 0; j < mMidiInputs.size(); j++)
			{
				if (mMidiInputs[j].portName == mMidiIn0.mPortNames[i]) alreadyListed = true;
			}
			if (!alreadyListed) {
				midiInput mIn;
				mIn.portName = mMidiIn0.mPortNames[i];
				mMidiInputs.push_back(mIn);
				if (mParameterBag->mMIDIOpenAllInputPorts)
				{
					openMidiInPort(i);
					mMidiInputs[i].isConnected = true;
					ss << "Opening MIDI port " << i << " " << mMidiInputs[i].portName;
				}
				else
				{
					mMidiInputs[i].isConnected = false;
					ss << "Available MIDI port " << i << " " << mMidiIn0.mPortNames[i];
				}

			}
		}
	}
	else
	{
		ss << "No MIDI Ports found!!!!" << std::endl;
	}
	ss << std::endl;

	mParameterBag->newMsg = true;
	mParameterBag->mMsg = ss.str();
	midiControlType = "none";
	midiControl = midiPitch = midiVelocity = midiNormalizedValue = midiValue = midiChannel = 0;
}
void MessageRouter::openMidiInPort(int i) {
	stringstream ss;
	if (i == 0)
	{
		mMidiIn0.openPort(i);
		mMidiIn0.midiSignal.connect(boost::bind(&MessageRouter::midiListener, this, boost::arg<1>::arg()));
	}
	if (i == 1)
	{
		mMidiIn1.openPort(i);
		mMidiIn1.midiSignal.connect(boost::bind(&MessageRouter::midiListener, this, boost::arg<1>::arg()));
	}
	if (i == 2)
	{
		mMidiIn2.openPort(i);
		mMidiIn2.midiSignal.connect(boost::bind(&MessageRouter::midiListener, this, boost::arg<1>::arg()));
	}
	mMidiInputs[i].isConnected = true;
	ss << "Opening MIDI port " << i << " " << mMidiInputs[i].portName << std::endl;
	mParameterBag->mMsg = ss.str();
	mParameterBag->newMsg = true;
}
void MessageRouter::closeMidiInPort(int i) {
	if (i == 0)
	{
		mMidiIn0.closePort();
	}
	if (i == 1)
	{
		mMidiIn1.closePort();
	}
	if (i == 2)
	{
		mMidiIn2.closePort();
	}
	mMidiInputs[i].isConnected = false;
}

void MessageRouter::midiListener(midi::Message msg)
{
	stringstream ss;
	midiChannel = msg.channel;
	switch (msg.status)
	{
	case MIDI_CONTROL_CHANGE:
		midiControlType = "/cc";
		midiControl = msg.control;
		midiValue = msg.value;
		midiNormalizedValue = lmap<float>(midiValue, 0.0, 127.0, 0.0, 1.0);
		if (mParameterBag->mOSCEnabled) {
			updateAndSendOSCFloatMessage(midiControlType, midiControl, midiNormalizedValue, midiChannel);
		}
		else {
			updateParams(midiControl, midiNormalizedValue);
		}

		//mWebSockets->write("{\"params\" :[{" + controlType);
		break;
	case MIDI_NOTE_ON:
		midiControlType = "/on";
		midiPitch = msg.pitch;
		midiVelocity = msg.velocity;
		midiNormalizedValue = lmap<float>(midiVelocity, 0.0, 127.0, 0.0, 1.0);
		break;
	case MIDI_NOTE_OFF:
		midiControlType = "/off";
		midiPitch = msg.pitch;
		midiVelocity = msg.velocity;
		midiNormalizedValue = lmap<float>(midiVelocity, 0.0, 127.0, 0.0, 1.0);
		break;
	default:
		break;
	}
	ss << "MIDI Chn: " << midiChannel << " type: " << midiControlType << " CC: " << midiControl << " Pitch: " << midiPitch << " Vel: " << midiVelocity << " Val: " << midiValue << " NVal: " << midiNormalizedValue << std::endl;
	mParameterBag->mMsg = ss.str();
	mParameterBag->newMsg = true;
}
void MessageRouter::sendJSON(string params) {
	wsWrite(params);
	if (mParameterBag->mOSCEnabled) {

	}
}

void MessageRouter::updateParams(int iarg0, float farg1)
{
	if (farg1 > 0.1)
	{
		//avoid to send twice
		if (iarg0 == 54) sendOSCIntMessage("/live/play", 0);			// play
		if (iarg0 == 53) sendOSCIntMessage("/live/stop", 0);			// stop
		if (iarg0 == 52) sendOSCIntMessage("/live/next/cue", 0);		// next cue
		if (iarg0 == 51) sendOSCIntMessage("/live/prev/cue", 0);		// previous cue
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
		if (iarg0 == 14) mParameterBag->controlValues[iarg0] = (farg1 + 0.01) * mParameterBag->maxExposure;
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
		// right arrow
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
void MessageRouter::setupOSCSender()
{
	// OSC sender with broadcast = true
	mOSCSender.setup(mParameterBag->mOSCDestinationHost, mParameterBag->mOSCDestinationPort, true);
	mOSCSender2.setup(mParameterBag->mOSCDestinationHost2, mParameterBag->mOSCDestinationPort2, true);
}
void MessageRouter::update()
{
	// websockets
	if (mParameterBag->mAreWebSocketsEnabledAtStartup)
	{
		if (mParameterBag->mIsWebSocketsServer)
		{
			mServer.poll();
		}
		else
		{
			if (clientConnected)
			{
				mClient.poll();
				/*double e = getElapsedSeconds();
				if (e - mPingTime > 20.0) {
					mClient.ping();
					mPingTime = e;
				}*/

			}
		}
	}
	/*
	// check for mouse moved message
	if(m.getAddress() == "/mouse/position"){
	// both the arguments are int32's
	Vec2i pos = Vec2i( m.getArgAsInt32(0), m.getArgAsInt32(1));
	Vec2f mouseNorm = Vec2f( pos ) / getWindowSize();
	Vec2f mouseVel = Vec2f( pos - pMouse ) / getWindowSize();
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
	Vec2i pos = Vec2i( m.getArgAsInt32(0), m.getArgAsInt32(1));
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
		bool routeMessage = false;
		mOSCReceiver.getNextMessage(&message);
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
			updateParams(iargs[0], fargs[1]);
		}
		else if (oscAddress == "/live/beat")
		{
			mParameterBag->iBeat = iargs[0];
			routeMessage = true;
		}
		else if (oscAddress == "/live/tempo")
		{
			mParameterBag->mTempo = fargs[0];
			routeMessage = true;
		}
		else if (oscAddress == "/live/track/meter")
		{
			mParameterBag->liveMeter = fargs[2];
			routeMessage = true;
		}
		else if (oscAddress == "/live/name/trackblock")
		{
			mParameterBag->mTrackName = sargs[0];
			for (int a = 0; a < MAX; a++)
			{
				tracks[a] = sargs[a];
			}

		}
		else if (oscAddress == "/live/play")
		{
			osc::Message m;
			m.setAddress("/reymenta/tracklist");

			for (int a = 0; a < MAX; a++)
			{
				if (tracks[a] != "") m.addStringArg(tracks[a]);
			}
			mOSCSender.sendMessage(m);

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
		else if (oscAddress == "/selectShader")
		{
			selectShader(iargs[0], iargs[1]);
		}

		else if (oscAddress == "/joint")
		{
			skeletonIndex = iargs[0];
			jointIndex = iargs[1];
			if (jointIndex < 20)
			{
				skeleton[jointIndex] = Vec4i(iargs[2], iargs[3], iargs[4], iargs[5]);
			}
		}
		else
		{
			console() << "OSC message received: " << oscAddress << std::endl;
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
		}
		stringstream ss;
		ss << message.getRemoteIp() << " adr:" << oscAddress << " ";
		for (int a = 0; a < MAX; a++)
		{
			ss << a << ":" << sargs[a] << " ";
		}
		ss << std::endl;
		mParameterBag->newMsg = true;
		mParameterBag->mMsg = ss.str();
		// filter messages
		if (routeMessage)
		{
			// avoid LiveOSC infinite loop
			if (mParameterBag->mIsOSCSender && mParameterBag->mOSCDestinationPort != 9000) mOSCSender.sendMessage(message);
			if (mParameterBag->mIsOSCSender && mParameterBag->mOSCDestinationPort2 != 9000) mOSCSender2.sendMessage(message);

		}

	}
}




void MessageRouter::sendOSCIntMessage(string controlType, int iarg0, int iarg1, int iarg2, int iarg3, int iarg4, int iarg5)
{
	osc::Message m;
	m.setAddress(controlType);
	m.addIntArg(iarg0);
	m.addIntArg(iarg1);
	m.addIntArg(iarg2);
	m.addIntArg(iarg3);
	m.addIntArg(iarg4);
	m.addIntArg(iarg5);
	m.setRemoteEndpoint(mParameterBag->mOSCDestinationHost, mParameterBag->mOSCDestinationPort);
	mOSCSender.sendMessage(m);
	m.setRemoteEndpoint(mParameterBag->mOSCDestinationHost2, mParameterBag->mOSCDestinationPort2);
	mOSCSender2.sendMessage(m);
}
void MessageRouter::sendOSCStringMessage(string controlType, int iarg0, string sarg1, string sarg2, string sarg3, string sarg4, string sarg5)
{
	osc::Message m;
	m.setAddress(controlType);
	m.addIntArg(iarg0);
	if (sarg1 != "") m.addStringArg(sarg1);
	if (sarg2 != "") m.addStringArg(sarg2);
	if (sarg3 != "") m.addStringArg(sarg3);
	if (sarg4 != "") m.addStringArg(sarg4);
	if (sarg5 != "") m.addStringArg(sarg5);
	m.setRemoteEndpoint(mParameterBag->mOSCDestinationHost, mParameterBag->mOSCDestinationPort);
	mOSCSender.sendMessage(m);
	m.setRemoteEndpoint(mParameterBag->mOSCDestinationHost2, mParameterBag->mOSCDestinationPort2);
	mOSCSender2.sendMessage(m);
}
void MessageRouter::sendOSCColorMessage(string controlType, float val)
{
	osc::Message m;
	m.setAddress(controlType);
	m.addFloatArg(val);
	m.setRemoteEndpoint(mParameterBag->mOSCDestinationHost, mParameterBag->mOSCDestinationPort);
	mOSCSender.sendMessage(m);
	m.setRemoteEndpoint(mParameterBag->mOSCDestinationHost2, mParameterBag->mOSCDestinationPort2);
	mOSCSender2.sendMessage(m);
}
void MessageRouter::sendOSCFloatMessage(string controlType, int iarg0, float farg1, float farg2, float farg3, float farg4, float farg5)
{
	osc::Message m;
	m.setAddress(controlType);
	m.addIntArg(iarg0);
	m.addFloatArg(farg1);
	m.addFloatArg(farg2);
	m.addFloatArg(farg3);
	m.addFloatArg(farg4);
	m.addFloatArg(farg5);
	m.setRemoteEndpoint(mParameterBag->mOSCDestinationHost, mParameterBag->mOSCDestinationPort);
	mOSCSender.sendMessage(m);
	m.setRemoteEndpoint(mParameterBag->mOSCDestinationHost2, mParameterBag->mOSCDestinationPort2);
	mOSCSender2.sendMessage(m);
}
void MessageRouter::updateAndSendOSCFloatMessage(string controlType, int iarg0, float farg1, float farg2, float farg3, float farg4, float farg5)
{
	updateParams(iarg0, farg1);
	sendOSCFloatMessage(controlType, iarg0, farg1, farg2);
}

void MessageRouter::wsPing()
{
	if (clientConnected)
	{
		if (!mParameterBag->mIsWebSocketsServer)
		{
			mClient.ping();
		}
	}
}

void MessageRouter::wsConnect()
{
	// either a client or a server
	if (mParameterBag->mIsWebSocketsServer)
	{
		mServer.addConnectCallback(&MessageRouter::onWsConnect, this);
		mServer.addDisconnectCallback(&MessageRouter::onWsDisconnect, this);
		mServer.addErrorCallback(&MessageRouter::onWsError, this);
		mServer.addInterruptCallback(&MessageRouter::onWsInterrupt, this);
		mServer.addPingCallback(&MessageRouter::onWsPing, this);
		mServer.addReadCallback(&MessageRouter::onWsRead, this);
		mServer.listen(mParameterBag->mWebSocketsPort);
	}
	else
	{
		mClient.addConnectCallback(&MessageRouter::onWsConnect, this);
		mClient.addDisconnectCallback(&MessageRouter::onWsDisconnect, this);
		mClient.addErrorCallback(&MessageRouter::onWsError, this);
		mClient.addInterruptCallback(&MessageRouter::onWsInterrupt, this);
		mClient.addPingCallback(&MessageRouter::onWsPing, this);
		mClient.addReadCallback(&MessageRouter::onWsRead, this);
		wsClientConnect();
	}
	mParameterBag->mAreWebSocketsEnabledAtStartup = true;
	clientConnected = true;
}
void MessageRouter::wsClientConnect()
{
	stringstream s;
	s << "ws://" << mParameterBag->mWebSocketsHost << ":" << mParameterBag->mWebSocketsPort;
	mClient.connect(s.str());
}
void MessageRouter::wsClientDisconnect()
{
	if (clientConnected)
	{
		mClient.disconnect();
	}
}
void MessageRouter::onWsConnect()
{
	clientConnected = true;
	mParameterBag->mMsg = "Connected";
	mParameterBag->newMsg = true;
}

void MessageRouter::onWsDisconnect()
{
	clientConnected = false;
	mParameterBag->mMsg = "Disconnected";
	mParameterBag->newMsg = true;
}

void MessageRouter::onWsError(string err)
{
	mParameterBag->mMsg = "WS Error";
	mParameterBag->newMsg = true;
	if (!err.empty()) {
		mParameterBag->mMsg += ": " + err;
	}

}

void MessageRouter::onWsInterrupt()
{
	mParameterBag->mMsg = "WS Interrupted";
	mParameterBag->newMsg = true;
}

void MessageRouter::onWsPing(string msg)
{
	mParameterBag->mMsg = "WS Ponged";
	mParameterBag->newMsg = true;
	if (!msg.empty())
	{
		mParameterBag->mMsg += ": " + msg;
	}
}

void MessageRouter::onWsRead(string msg)
{
	int left;
	int index;
	mParameterBag->mMsg = "WS onRead";
	mParameterBag->newMsg = true;
	if (!msg.empty())
	{
		mParameterBag->mMsg += ": " + msg;
		string first = msg.substr(0, 1);
		if (first == "{")
		{
			// json
			JsonTree json;
			try
			{
				json = JsonTree(msg);
				JsonTree jsonParams = json.getChild("params");
				for (JsonTree::ConstIter jsonElement = jsonParams.begin(); jsonElement != jsonParams.end(); ++jsonElement)
				{
					int name = jsonElement->getChild("name").getValue<int>();
					float value = jsonElement->getChild("value").getValue<float>();
					if (name > mParameterBag->controlValues.size()) {
						switch (name)
						{
						case 300:
							//selectShader
							left = jsonElement->getChild("left").getValue<int>();
							index = jsonElement->getChild("index").getValue<int>();
							selectShader(left, index);
							break;
						default:
							break;
						}

					}
					else {
						// basic name value 
						mParameterBag->controlValues[name] = value;
					}
				}
				JsonTree jsonSelectShader = json.getChild("selectShader");
				for (JsonTree::ConstIter jsonElement = jsonSelectShader.begin(); jsonElement != jsonSelectShader.end(); ++jsonElement)
				{
				}
			}
			catch (cinder::JsonTree::Exception exception)
			{
				mParameterBag->mMsg += " error jsonparser exception: ";
				mParameterBag->mMsg += exception.what();
				mParameterBag->mMsg += "  ";
			}
		}
		else if (first == "#")
		{
			// fragment shader from live coding
			//mBatchass->getShadersRef()->loadLiveShader(msg);

		}
	}
}
void MessageRouter::wsWrite(string msg)
{
	if (mParameterBag->mAreWebSocketsEnabledAtStartup)
	{
		if (mParameterBag->mIsWebSocketsServer)
		{
			mServer.write(msg);
		}
		else
		{
			if (clientConnected) mClient.write(msg);
		}
	}
}
void MessageRouter::selectShader(bool left, int index)
{
	if (left) {
		mParameterBag->mLeftFragIndex = index;
	}
	else {
		mParameterBag->mRightFragIndex = index;
	}
}
void MessageRouter::colorWrite()
{
	if (mParameterBag->mOSCEnabled)
	{
		sendOSCColorMessage("/fr", mParameterBag->controlValues[1]);
		sendOSCColorMessage("/fg", mParameterBag->controlValues[2]);
		sendOSCColorMessage("/fb", mParameterBag->controlValues[3]);
		sendOSCColorMessage("/fa", mParameterBag->controlValues[4]);
	}
	char col[8];
	int r = mParameterBag->controlValues[1] * 255;
	int g = mParameterBag->controlValues[2] * 255;
	int b = mParameterBag->controlValues[3] * 255;
	sprintf_s(col, sizeof(col), "#%02X%02X%02X", r, g, b);
	wsWrite(col);
}
