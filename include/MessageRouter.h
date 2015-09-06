#pragma once

#include "cinder/app/AppNative.h"
#include "Resources.h"
#include "ParameterBag.h"


using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta 
{

	typedef std::shared_ptr<class MessageRouter> MessageRouterRef;

	class MessageRouter
	{

	public:
		MessageRouter(ParameterBagRef aParameterBag);
		static	MessageRouterRef create(ParameterBagRef aParameterBag);
		// messages
		void						sendJSON(string params);



	private:
		// parameters
		ParameterBagRef				mParameterBag;

	};
}