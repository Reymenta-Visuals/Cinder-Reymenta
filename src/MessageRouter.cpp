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

void MessageRouter::sendJSON(string params) {
	mWebSockets->write(params);
	if (mParameterBag->mOSCEnabled) {

	}
}