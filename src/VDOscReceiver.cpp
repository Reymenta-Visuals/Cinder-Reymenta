#include "VDOscReceiver.h"

using namespace videodromm;

VDOscReceiverRef VDOscReceiver::create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms)
{
	return std::shared_ptr<VDOscReceiver>(new VDOscReceiver(aVDSettings, aVDAnimation, aVDUniforms));
}

VDOscReceiver::VDOscReceiver(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms) {
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDUniforms = aVDUniforms;
	CI_LOG_V("VDOscReceiver constructor");
	mOSCMsg = "";

}

void VDOscReceiver::setOSCMsg(const std::string& aMsg) {
	mOSCMsg = aMsg;
};
std::string VDOscReceiver::getOSCMsg() {
	return mOSCMsg;
}
void VDOscReceiver::setupOSCReceiver(VDMediatorObservableRef aVDMediator, int aOSCReceiverPort) {
	mVDMediator = aVDMediator;
	//mOscReceiver = std::make_shared<osc::ReceiverUdp>(aOSCReceiverPort);

	//mOscReceiver->setListener("/*",
	//	[&](const osc::Message& msg) {
	//	// touchosc
	//	bool found = false;
	//	std::string ctrl = "";
	//	int index = -1;
	//	int i = 0;
	//	float f = 1.0f;
	//	std::stringstream ss;
	//	vec2 vv = vec2(0.0f);
	//	std::string addr = msg.getAddress();
	//	// handle all msg without page integer first
	//	// midi cc in osc
	//	ss << addr;
	//	ctrl = "/cc";
	//	index = addr.find(ctrl); // 20210110 TODO check if npos or -1 to compare
	//	if (index != std::string::npos)
	//	{
	//		// from midi2osc (Cinder) or hydra standalone
	//		found = true;
	//		i = msg[0].int32();// TODO check was flt() from hydra standalone if ok can merge with midi;
	//		f = msg[1].flt();// was for hydra / 128;
	//		mVDMediator->setUniformValue(i, f);
	//		//ss << " midi from OSC " << i << " value " << f;
	//	}
	//	if (!found)
	//	{
	//		// GrabberSender
	//		ctrl = "grab";
	//		index = addr.find(ctrl);
	//		if (index != std::string::npos)
	//		{
	//			found = true;
	//			f = msg[0].flt();
	//			int lastSlashIndex = addr.find_last_of("/");
	//			//std::string u = addr.substr(dotIndex + 1);
	//			int iu = std::atoi(addr.substr(lastSlashIndex + 1).c_str());
	//			// mult for zoom
	//			if (iu == mVDUniforms->IMULT) {
	//				f *= 4.0f;
	//			}
	//			// zoom 632 644
	//			if (iu == mVDUniforms->IZOOM) {
	//				// TODO if (mVDUniforms->getUniformValue(mVDUniforms->IBARBEAT) > 631.0f) {
	//				//	if (mVDUniforms->getUniformValue(mVDUniforms->IBARBEAT) < 644.0f) {
	//						f *= mVDUniforms->getUniformValue(mVDUniforms->IMULT);
	//					//}
	//				//}
	//			}
	//			mVDMediator->setUniformValue(iu, f);
	//		}
	//	}
	//	if (!found)
	//	{
	//		// accxyz
	//		ctrl = "accxyz";
	//		index = addr.find(ctrl);
	//		if (index != std::string::npos)
	//		{
	//			found = true;
	//		}
	//	}
	//	if (!found)
	//	{
	//		// int32 from Max OSC Midi Send
	//		ctrl = "/Note1";
	//		index = addr.find(ctrl);
	//		if (index != std::string::npos)
	//		{
	//			found = true;
	//			mNote = msg[0].int32();
	//			// set iStart
	//			if (mVelocity > 0) {
	//				// note range 0 to 127 tracks from 110 = midi note D7
	//				if (mNote > 104) {

	//					// load folder in main app
	//					if (mVDUniforms->getUniformValue(mVDUniforms->ITRACK) == (float)mNote) {
	//						// same track, reset 
	//						mVDMediator->setUniformValue(mVDUniforms->IBARSTART, mVDUniforms->getUniformValue(mVDUniforms->IBAR));
	//					}
	//					else {
	//						mVDMediator->setUniformValue(mVDUniforms->ITRACK, (float)mNote);
	//					}
	//				}

	//			}
	//		}
	//	}

	//	if (!found)
	//	{
	//		// int32 from Max OSC Midi Send
	//		ctrl = "/Velocity";
	//		index = addr.find(ctrl);
	//		if (index != std::string::npos)
	//		{
	//			found = true;
	//			mVelocity = msg[0].int32();
	//		}
	//	}
	//	if (!found)
	//	{
	//		// ableton link tempo from ofx abletonLinkToWebsocket
	//		ctrl = "/live/tempo";
	//		index = addr.find(ctrl);
	//		if (index != std::string::npos)
	//		{
	//			found = true;
	//			//mVDAnimation->setAutoBeatAnimation(false);
	//			mVDMediator->setUniformValue(mVDUniforms->IBPM, msg[0].flt());
	//		}
	//	}
	//	if (!found)
	//	{
	//		// int32 0 or 1 play from Transthor
	//		ctrl = "/play";
	//		index = addr.find(ctrl);
	//		if (index != std::string::npos)
	//		{
	//			found = true;
	//		}
	//	}
	//	if (!found)
	//	{
	//		// float ticks from Transthor
	//		ctrl = "/ticks";
	//		index = addr.find(ctrl);
	//		if (index != std::string::npos)
	//		{
	//			found = true;
	//			f = msg[0].flt();
	//			mVDAnimation->useTimeWithTempo();
	//			mVDMediator->setUniformValue(mVDUniforms->ITIME, f);
	//		}
	//	}
	//	if (!found)
	//	{
	//		//	float tempo from Transthor
	//		//	SOS 160.0
	//		//	HumanET	150
	//		//	Hoover	135
	//		//	Massactiv whatelse 138.77
	//		//
	//		ctrl = "/tempo";
	//		index = addr.find(ctrl);
	//		if (index != std::string::npos)
	//		{
	//			found = true;
	//			mVDAnimation->useTimeWithTempo();
	//			f = msg[0].flt();
	//			mVDMediator->setUniformValue(mVDUniforms->IBPM, f);
	//			//ss << " " << f;
	//			//CI_LOG_I("tempo:" + toString(mVDAnimation->getBpm()));
	//		}
	//	}
	//	if (!found)
	//	{
	//		//32 floats from Speckthor
	//		ctrl = "/Spectrum";
	//		index = addr.find(ctrl);
	//		if (index != std::string::npos)
	//		{
	//			found = true;
	//			mVDMediator->setUniformValue(mVDUniforms->IMAXVOLUME, 0.0f);
	//			for (int a{ 0 }; a < msg.getNumArgs(); a++) {
	//				// get the argument type 'f'
	//				if (msg.getArgType(i) == ArgType::FLOAT) {
	//					f = msg[a].flt() * 200.0f * mVDUniforms->getUniformValue(mVDUniforms->IAUDIOX);
	//					if (f > mVDUniforms->getUniformValue(mVDUniforms->IMAXVOLUME))
	//					{
	//						mVDMediator->setUniformValue(mVDUniforms->IMAXVOLUME, f);
	//					}
	//					mVDAnimation->iFreqs[a] = f;
	//					if (a == mVDAnimation->getFreqIndex(0)) mVDMediator->setUniformValue(mVDUniforms->IFREQ0, f);
	//					if (a == mVDAnimation->getFreqIndex(1)) mVDMediator->setUniformValue(mVDUniforms->IFREQ1, f);
	//					if (a == mVDAnimation->getFreqIndex(2)) mVDMediator->setUniformValue(mVDUniforms->IFREQ2, f);
	//					if (a == mVDAnimation->getFreqIndex(3)) mVDMediator->setUniformValue(mVDUniforms->IFREQ3, f);
	//				}
	//			}
	//		}
	//	}
		

		//if (!found)
		//{
		//	// int32 1 to 4 beat from Transthor
		//	ctrl = "/beat";
		//	index = addr.find(ctrl);
		//	if (index != std::string::npos)
		//	{
		//		found = true;
		//		mVDMediator->setUniformValue(mVDUniforms->IBEAT, (float)msg[0].int32() - 1);
		//	}
		//}
		//if (!found)
		//{
		//	// int32 bar from Transthor
		//	ctrl = "/bar";
		//	index = addr.find(ctrl);
		//	if (index != std::string::npos)
		//	{
		//		found = true;
		//		mVDMediator->setUniformValue(mVDUniforms->IBAR, (float)msg[0].int32());

		//	}
		//}

		//if (!found)
		//{
		//	ctrl = "link";
		//	index = addr.find(ctrl);
		//	if (index != std::string::npos)
		//	{
		//		found = true;
		//		double d0 = msg[0].dbl(); // tempo
		//		mVDMediator->setUniformValue(mVDUniforms->IBPM, d0);
		//		double d1 = msg[1].dbl();
		//		int d2 = msg[2].int32();
		//		//! 20200526 mVDSocketio->changeIntValue(mVDUniforms->IBEAT, d2);
		//	}
		//}
		//// azure kinect
		//if (!found)
		//{
		//	ctrl = "kb";
		//	index = addr.find(ctrl);
		//	if (index != std::string::npos)
		//	{
		//		/*	createVec4Uniform("Pelvis", 200, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		//		string adrs = "/kb" + to_string(bodyIdx) + "/" + to_string(i + 200);// jointNames[i];
		//		m.setAddress(adrs);
		//		m.addFloatArg(joint.position.xyz.x);
		//		m.addFloatArg(joint.position.xyz.y);
		//		m.addFloatArg(joint.position.xyz.z);
		//		m.addIntArg(i);
		//		m.addIntArg(bodyIdx);
		//		m.addStringArg(jointNames[i]);*/
		//		try {
		//			int lastSlashIndex = addr.find_last_of("/"); // 0 2
		//			
		//			found = true;
		//			f = msg[0].flt(); //posX
		//			float posY = msg[1].flt();
		//			float posZ = msg[2].flt();
		//			i = msg[3].int32();//bodyId
		//			int joint = std::stoi(addr.substr(lastSlashIndex + 1));
		//			mVDUniforms->setVec4UniformValueByIndex(joint, vec4(f, posY, posZ, i));
		//			if (joint == 227) {
		//				// Nose
		//				mVDMediator->setUniformValue(mVDUniforms->IFREQ0, f);
		//				mVDMediator->setUniformValue(mVDUniforms->IFREQ1, posY);
		//				mVDMediator->setUniformValue(mVDUniforms->IFREQ2, posZ);
		//				mVDMediator->setUniformValue(mVDUniforms->IFREQ3, i);
		//				mNote = joint; 
		//				mVelocity = posY;
		//			}
		//		}
		//		catch (const std::exception& e) {
		//			ss << addr << " not integer";
		//			mVDSettings->setErrorMsg(ss.str());
		//			CI_LOG_E("not integer: " << addr);
		//		}
		//	}
		//}
		//if (!found)
		//{
		//	// touchosc
		//	int page = 0;
		//	try {
		//		// CHECK exception if not integer
		//		page = std::stoi(addr.substr(1, 1)); // 1 to 4

		//		int lastSlashIndex = addr.find_last_of("/"); // 0 2
		//		// if not a page selection
		//		if (addr.length() > 2) {
		//			ctrl = "multifader1";
		//			index = addr.find(ctrl);
		//			if (index != std::string::npos)
		//			{
		//				found = true;
		//				f = msg[0].flt();
		//				i = std::stoi(addr.substr(lastSlashIndex + 1)) + 8;
		//				mVDMediator->setUniformValue(i, f);
		//			}

		//			if (!found)
		//			{
		//				ctrl = "multifader2";
		//				index = addr.find(ctrl);
		//				if (index != std::string::npos)
		//				{
		//					found = true;
		//					f = msg[0].flt();
		//					i = std::stoi(addr.substr(lastSlashIndex + 1)) + 32; // 24 + 8
		//					mVDMediator->setUniformValue(i, f);
		//				}
		//			}

		//			if (!found)
		//			{
		//				ctrl = "multifader";
		//				index = addr.find(ctrl);
		//				if (index != std::string::npos)
		//				{
		//					found = true;
		//					f = msg[0].flt();
		//					i = std::stoi(addr.substr(lastSlashIndex + 1)) + 56; // 48 + 8
		//					mVDMediator->setUniformValue(i, f);
		//				}
		//			}
		//			if (!found)
		//			{
		//				ctrl = "fader";
		//				index = addr.find(ctrl);
		//				if (index != std::string::npos)
		//				{
		//					found = true;
		//					f = msg[0].flt();
		//					i = std::stoi(addr.substr(index + ctrl.length()));
		//					mVDMediator->setUniformValue(i, f);// starts at 1: mVDUniforms->ICOLORX G B
		//				}
		//			}
		//			if (!found)
		//			{
		//				ctrl = "rotary";
		//				index = addr.find(ctrl);
		//				if (index != std::string::npos)
		//				{
		//					found = true;
		//					f = msg[0].flt();
		//					i = std::stoi(addr.substr(index + ctrl.length())) + 10;
		//					mVDMediator->setUniformValue(i, f);
		//				}
		//			}
		//			if (!found)
		//			{
		//				// toggle
		//				ctrl = "multitoggle";
		//				index = addr.find(ctrl);
		//				if (!found && index != std::string::npos)
		//				{
		//					found = true;
		//					// /2/multitoggle/5/3
		//				}
		//			}
		//			if (!found)
		//			{
		//				// toggle
		//				ctrl = "toggle";
		//				index = addr.find(ctrl);
		//				if (!found && index != std::string::npos)
		//				{
		//					found = true;
		//					f = msg[0].flt();
		//					i = std::stoi(addr.substr(index + ctrl.length())) + 80;
		//					mVDAnimation->toggleValue(i);
		//				}
		//			}
		//			if (!found)
		//			{
		//				// push
		//				ctrl = "push";
		//				index = addr.find(ctrl);
		//				if (!found && index != std::string::npos)
		//				{
		//					found = true;
		//					f = msg[0].flt();
		//					i = std::stoi(addr.substr(index + ctrl.length())) + 80;
		//					mVDAnimation->toggleValue(i);
		//				}
		//			}
		//			if (!found)
		//			{
		//				// xy
		//				ctrl = "xy1";
		//				index = addr.find(ctrl);
		//				if (!found && index != std::string::npos)
		//				{
		//					found = true;
		//					vv = vec2(msg[0].flt(), msg[1].flt());
		//				}
		//			}
		//			if (!found)
		//			{
		//				// xy
		//				ctrl = "xy2";
		//				index = addr.find(ctrl);
		//				if (!found && index != std::string::npos)
		//				{
		//					found = true;
		//					vv = vec2(msg[0].flt(), msg[1].flt());
		//				}
		//			}
		//			if (!found)
		//			{
		//				// xy
		//				ctrl = "xy";
		//				index = addr.find(ctrl);
		//				if (!found && index != std::string::npos)
		//				{
		//					found = true;
		//					vv = vec2(msg[0].flt(), msg[1].flt());
		//				}
		//			}
		//		}
		//	}
		//	catch (const std::exception& e) {
		//		ss << addr << " not integer";
		//		mVDSettings->setErrorMsg(ss.str());
		//		CI_LOG_E("not integer: " << addr);
		//	}
		//}
		//if (!found) {
		//	CI_LOG_E("not handled: " << msg.getNumArgs() << " addr: " << addr);
		//	//mVDSettings->mOSCMsg += "\nnot handled: " + addr;
		//	mVDSettings->setMsg("osc not handled: " + addr);
		//}
		//if (addr != "/play" && addr != "/Spectrum" && addr != "/bar" && addr != "/beat"  && addr != "/tempo"  && addr != "/ticks") {
		//	ss << " f:" << f << " i:" << i << " note:" << mNote << " U::" << mVDUniforms->getUniformName(mNote) << " velocity:" << mVelocity;
		//	mOSCMsg = ss.str();
		//}
		//else {
		//	mVDSettings->setMsg(ss.str());
		//}

	//});

	//try {
	//	// Bind the receiver to the endpoint. This function may throw.
	//	mOscReceiver->bind();
	//}
	//catch (const osc::Exception& ex) {
	//	CI_LOG_E("Error binding: " << ex.what() << " val: " << ex.value());
	//}

	//// UDP opens the socket and "listens" accepting any message from any endpoint. The listen
	//// function takes an error handler for the underlying socket. Any errors that would
	//// call this function are because of problems with the socket or with the remote message.
	//mOscReceiver->listen(
	//	[](asio::error_code error, protocol::endpoint endpoint) -> bool {
	//	if (error) {
	//		CI_LOG_E("Error Listening: " << error.message() << " val: " << error.value() << " endpoint: " << endpoint);
	//		return false;
	//	}
	//	else

	//		return true;
	//});
}



