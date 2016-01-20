#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Xml.h"

#include "ParameterBag.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace Reymenta;

namespace VideoDromm {

	typedef std::shared_ptr<class VDSession> VDSessionRef;
	

	class VDSession
	{
	public:
		VDSession(ParameterBagRef aParameterBag);
		static VDSessionRef			create(ParameterBagRef aParameterBag);
		bool						save();
		bool						restore();
		void						reset();
		void						resetSomeParams();


		// fbo 
		bool						mFlipV;
		bool						mFlipH;

	private:
		// parameters
		ParameterBagRef				mParameterBag;
		const string sessionFileName = "VDSession.xml";
	};

}
