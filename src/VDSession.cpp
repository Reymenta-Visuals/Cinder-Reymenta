//
//  VDsession.cpp
//

#include "VDSession.h"

using namespace VideoDromm;

VDSession::VDSession(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;

	// reset no matter what, so we don't miss anything
	reset();

	// check to see if VDSession.xml file exists and restore if it does
	fs::path params = getAppPath() / sessionFileName;
	if (fs::exists(params))
	{
		restore();
	}
	else
	{
		save();
	}
}

VDSessionRef VDSession::create(ParameterBagRef aParameterBag)
{
	return shared_ptr<VDSession>(new VDSession(aParameterBag));
}

bool VDSession::save()
{

	fs::path path = getAssetPath("") / mParameterBag->mAssetsPath / sessionFileName;

	XmlTree session("session", "");

	XmlTree FlipV("FlipV", "");
	FlipV.setAttribute("value", toString(mFlipV));
	session.push_back(FlipV);

	XmlTree FlipH("FlipH", "");
	FlipH.setAttribute("value", toString(mFlipH));
	session.push_back(FlipH);

	// write XML file
	session.write(writeFile(path));

	return true;
}

bool VDSession::restore()
{
	// check to see if session.xml file exists
	fs::path path = getAppPath() / mParameterBag->mAssetsPath / sessionFileName;
	if (fs::exists(path)) {
		// if it does, restore
		const XmlTree xml(loadFile(path));

		if (!xml.hasChild("session")) {
			return false;
		}
		else {
			const XmlTree session = xml.getChild("session");

			if (session.hasChild("FlipV")) {
				XmlTree FlipV = session.getChild("FlipV");
				mFlipV = FlipV.getAttributeValue<bool>("value");
			}

			if (session.hasChild("FlipH")) {
				XmlTree FlipH = session.getChild("FlipH");
				mFlipH = FlipH.getAttributeValue<bool>("value");
			}

			return true;
		}
	}
	else {
		// if it doesn't, return false
		return false;
	}
}

void VDSession::resetSomeParams() {
}

void VDSession::reset()
{
	// parameters exposed in XML
	mFlipV = false;
	mFlipH = false;

	resetSomeParams();


}
