#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Json.h"

#include "WebSocketClient.h"
#include "WebSocketServer.h"

#include "ParameterBag.h"

// MessageRouter
#include "MessageRouter.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta 
{

	typedef std::shared_ptr<class WebSockets> WebSocketsRef;

	class WebSockets 
	{

	public:
		WebSockets(ParameterBagRef aParameterBag, MessageRouterRef aMessageRouter);
		static						WebSocketsRef create(ParameterBagRef aParameterBag, MessageRouterRef aMessageRouter);
		void						update();
		void						write(std::string msg);
		void						connect();
		void						ping();
		bool						isClientConnected() { return clientConnected; };
	private:
		// parameters
		ParameterBagRef				mParameterBag;
		// MessageRouter
		MessageRouterRef			mMessageRouter;

		// Web socket client
		void						clientDisconnect();
		WebSocketClient				mClient;
		void						clientConnect();
		void						onConnect();
		void						onDisconnect();
		void						onError(std::string err);
		void						onInterrupt();
		void						onPing(std::string msg);
		void						onRead(std::string msg);
		bool						clientConnected;
		// Web socket  server
		WebSocketServer				mServer;
		void						serverConnect();
		void						serverDisconnect();
		double						mPingTime;

	};
}