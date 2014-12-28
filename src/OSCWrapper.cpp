#include "OSCWrapper.h"

using namespace Reymenta;

OSC::OSC(ParameterBagRef aParameterBag, ShadersRef aShadersRef)
{
	mParameterBag = aParameterBag;
	mShaders = aShadersRef;
	for (int i = 0; i < 20; i++)
	{
		skeleton[i] = Vec4i::zero();
	}
	// OSC sender
	mOSCSender.setup(mParameterBag->mOSCDestinationHost, mParameterBag->mOSCDestinationPort);
	// OSC receiver
	mOSCReceiver.setup(mParameterBag->mOSCReceiverPort);
}

OSCRef OSC::create(ParameterBagRef aParameterBag, ShadersRef aShadersRef)
{
	return shared_ptr<OSC>(new OSC(aParameterBag, aShadersRef));
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
		mOSCReceiver.getNextMessage(&message);
		int arg0 = 0;
		int arg1 = 0;
		int arg2 = 0;
		int arg3 = 0;
		int skeletonIndex = 0;
		int jointIndex = 0;
		string oscAddress = message.getAddress();
		string oscArg0 = "";
		string oscArg1 = "";
		string oscArg2 = "";
		string oscArg3 = "";
		int numArgs = message.getNumArgs();
		if (oscAddress == "/midi")
		{
			for (int i = 0; i < message.getNumArgs(); i++) {
				cout << "-- Argument " << i << std::endl;
				cout << "---- type: " << message.getArgTypeName(i) << std::endl;
				if (message.getArgType(i) == osc::TYPE_INT32) {
					try {
						//cout << "------ int value: "<< message.getArgAsInt32(i) << std::endl;
						if (i == 0)
						{
							arg0 = message.getArgAsInt32(i);
							oscArg0 = toString(message.getArgAsInt32(i));
						}
						if (i == 1)
						{
							arg1 = message.getArgAsInt32(i);
							oscArg1 = toString(message.getArgAsInt32(i));
						}
						if (i == 2)
						{
							arg2 = message.getArgAsInt32(i);
							oscArg2 = toString(message.getArgAsInt32(i));
						}
						if (i == 3)
						{
							arg3 = message.getArgAsInt32(i);
							oscArg3 = toString(message.getArgAsInt32(i));
						}
					}
					catch (...) {
						cout << "Exception reading argument as int32" << std::endl;
					}
				}
			}
			if (arg0 < 0) arg0 = 0;
			if (arg1 < 0) arg1 = 0;
			if (arg0 > 4096) arg0 = 4096;
			if (arg1 > 4096) arg1 = 4096;
			float normalizedValue = lmap<float>(arg1, 0.0, 127.0, 0.0, 1.0);
			switch (arg0)
			{
			case 14:
				mParameterBag->controlValues[19] = (arg1 - 63.0) / 63.0;
				break;
			case 17:
				mParameterBag->iCrossfade = normalizedValue;
				break;
			default:
				mParameterBag->controlValues[arg0] = normalizedValue;
				break;
			}

		}
		else if (oscAddress == "/pixelfrag")
		{
			oscArg0 = message.getArgAsString(0);
			mShaders->setFragString(oscArg0);
		}
		else if (oscAddress == "/sumMovement")
		{
			float sumMovement = message.getArgAsFloat(0);
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
			float handsHeadHeight = message.getArgAsFloat(0);
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
			float x = message.getArgAsFloat(0);
			float y = message.getArgAsFloat(1);
			// background green
			mParameterBag->controlValues[6] = y;
			// green
			mParameterBag->controlValues[2] = x;
		}
		/*// background red
		controlValues[5] = 0.1f;
		// background green
		controlValues[6] = 0.1f;
		// background blue
		controlValues[7] = 0.1f;
		// vignette
		controlValues[47] = 1.0f;
		*/
		else if (oscAddress == "/joint")
		{
			skeletonIndex = message.getArgAsInt32(0);
			jointIndex = message.getArgAsInt32(1);
			if (jointIndex < 20)
			{
				arg0 = message.getArgAsInt32(2);
				arg1 = message.getArgAsInt32(3);
				arg2 = message.getArgAsInt32(4);
				arg3 = message.getArgAsInt32(5);
				skeleton[jointIndex] = Vec4i(arg0, arg1, arg2, arg3);
			}
		}
		else if (oscAddress == "/renderwindows")//	sendOSCIntMessage("/renderwindows", mRenderWindowsCount, 0, 0 );
		{
			mParameterBag->mWindowToCreate = message.getArgAsInt32(1);
			oscArg0 = toString(message.getArgAsInt32(0));
			oscArg1 = toString(message.getArgAsInt32(1));

		}
		else if (oscAddress == "/warp/loadimage")
		{
			oscArg0 = message.getArgAsString(0);
			//fs::path imagePath = oscArg0;
			//mImage = loadImage( imagePath );//CHECK
			//mSrcArea = mImage->getBounds();
			// adjust the content size of the warps
			//Warp::setSize( mWarps, mImage->getSize() );
		}
		else if (oscAddress == "/warp/loadmovie")
		{
			oscArg0 = message.getArgAsString(0);
			//fs::path moviePath = oscArg0;
			//loadMovieFile( moviePath );
		}
		else if (oscAddress == "/activeclip/video/positionx/values")
		{
			mParameterBag->iMouse.x = message.getArgAsFloat(0);
			oscArg0 = toString(message.getArgAsFloat(0));
			//pad->setValue( Vec2f(iMouse.x, iMouse.y ) );
		}
		else if (oscAddress == "/composition/video/rotatex/values")
		{
			mParameterBag->iMouse.x = (message.getArgAsFloat(0) + 1)  * mParameterBag->multFactor;
			oscArg0 = toString(message.getArgAsFloat(0));
			//console() << "x: " << iMouse.x  << " y: " << iMouse.y << std::endl;
			//status->setLabel("x: "+toString(iMouse.x)+" y: "+toString(iMouse.y)+" volume: "+toString(maxVolume));

			//pad->setValue( Vec2f(iMouse.x, iMouse.y ) );
		}
		else if (oscAddress == "/composition/video/rotatey/values")
		{
			mParameterBag->iMouse.y = (message.getArgAsFloat(0) + 1) * mParameterBag->multFactor;
			oscArg0 = toString(message.getArgAsFloat(0));
			//pad->setValue( Vec2f(iMouse.x, iMouse.y ) );
		}
		else if (oscAddress == "/activeclip/video/positiony/values")
		{
			mParameterBag->iMouse.y = message.getArgAsFloat(0);
			oscArg0 = toString(message.getArgAsFloat(0));
			//pad->setValue( Vec2f(iMouse.x, iMouse.y ) );
		}
		else
		{
			console() << "OSC message received: " << oscAddress << std::endl;
			for (int i = 0; i < message.getNumArgs(); i++) {
				cout << "-- Argument " << i << std::endl;
				cout << "---- type: " << message.getArgTypeName(i) << std::endl;
				if (message.getArgType(i) == osc::TYPE_INT32) {
					try {
						//cout << "------ int value: "<< message.getArgAsInt32(i) << std::endl;
						if (i == 0)
						{
							arg0 = message.getArgAsInt32(i);
							oscArg0 = toString(message.getArgAsInt32(i));
						}
						if (i == 1)
						{
							arg1 = message.getArgAsInt32(i);
							oscArg1 = toString(message.getArgAsInt32(i));
						}
						if (i == 2)
						{
							arg2 = message.getArgAsInt32(i);
							oscArg2 = toString(message.getArgAsInt32(i));
						}
						if (i == 3)
						{
							arg3 = message.getArgAsInt32(i);
							oscArg3 = toString(message.getArgAsInt32(i));
						}
					}
					catch (...) {
						cout << "Exception reading argument as int32" << std::endl;
					}
				}
				else if (message.getArgType(i) == osc::TYPE_FLOAT) {
					try {
						cout << "------ float value: " << message.getArgAsFloat(i) << std::endl;
						if (i == 0)
						{
							oscArg0 = toString(message.getArgAsFloat(i));
						}
						if (i == 1)
						{
							oscArg1 = toString(message.getArgAsFloat(i));
						}
					}
					catch (...) {
						cout << "Exception reading argument as float" << std::endl;
					}
				}
				else if (message.getArgType(i) == osc::TYPE_STRING) {
					try {
						cout << "------ string value: " << message.getArgAsString(i).c_str() << std::endl;
						if (i == 0)
						{
							oscArg0 = message.getArgAsString(i).c_str();
						}
						if (i == 1)
						{

							oscArg1 = message.getArgAsString(i).c_str();
						}
					}
					catch (...) {
						cout << "Exception reading argument as string" << std::endl;
					}
				}
			}
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
						mShaders->loadFragJson(fragFile.string());
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
			if (name > 0)	oscProcessMessage(name, arg0, arg1);
		}
		string oscString = "osc from:" + message.getRemoteIp() + " adr:" + oscAddress + " 0: " + oscArg0 + " 1: " + oscArg1;
		//mUserInterface->labelOSC->setName( oscString );
		mParameterBag->OSCMsg = oscString;
		//stringstream oscString; 
		//oscString << "osc address: " << oscAddress << " oscArg0: " << oscArg0  << " oscArg1: " << oscArg1;
		//oscStatus->setLabel( oscString );
	}
}
void OSC::oscProcessMessage(int controlName, int arg0, int arg1)
{
	float normalizedValue = lmap<float>(arg0, 0.0, 127.0, 0.0, 1.0);

	if (arg0 < 0) arg0 = 0;
	if (arg1 < 0) arg1 = 0;
	if (arg0 > 4096) arg0 = 4096;
	if (arg1 > 4096) arg1 = 4096;

	switch (controlName)
	{
		//pad
	case 0:
		mParameterBag->mRenderResoXY.x = lmap<float>(arg0, 0, 127, 0.0, 4096.0);
		mParameterBag->mRenderResoXY.y = lmap<float>(arg1, 0, 127, 0.0, 4096.0);
		//pad->setValue(mMousePos);
		break;
	default:
		mParameterBag->controlValues[controlName] = normalizedValue;
		break;
	}
}

void OSC::rotaryChange(int name, float newValue)
{
	switch (name)
	{
	case 14:
		mParameterBag->controlValues[19] = (newValue*127.0 - 63.0) / 63.0;
		break;
	case 17:
		//mParameterBag->iCrossfade = lmap<float>(newValue, 0.0, 127.0, 0.0, 1.0);
		mParameterBag->iCrossfade = newValue;
		break;
	default:
		mParameterBag->controlValues[name] = newValue;
		break;
	}

}
void OSC::toggleChange(int name, float newValue)
{
	if (newValue > 1.0) newValue = 1.0; // for boolean in frag
	mParameterBag->controlValues[name] = newValue;
	if (name > 20 && name < 29)
	{
		if (mParameterBag->mMode != name - 21)
		{
			mParameterBag->mNewMode = name - 21;
		}
	}
	if (name > 30 && name < 39)
	{
		/*if (mParameterBag->mMode == 0) //normal
		{
		mShaders->setCurrentRenderShaderIndex(name - 31);
		}*/
		if (mParameterBag->mMode == 1) //mix
		{
			if (name > 30 && name < 35) mParameterBag->mLeftFboIndex = name - 31;
			if (name > 34 && name < 39) mParameterBag->mRightFboIndex = name - 35;
		}
	}
	else
	{
		switch (name)
		{
		case 31:
			break;
		}
	}


}
void OSC::sendOSCIntMessage(string controlType, int controlName, int controlValue0, int controlValue1)
{
	osc::Message m;
	m.setAddress(controlType);
	m.addIntArg(controlName);
	m.addIntArg(controlValue0);
	mOSCSender.sendMessage(m);
}
void OSC::sendOSCStringMessage(string controlType, string controlString)
{
	osc::Message m;
	m.setAddress(controlType);
	m.addStringArg(controlString);
	mOSCSender.sendMessage(m);
}