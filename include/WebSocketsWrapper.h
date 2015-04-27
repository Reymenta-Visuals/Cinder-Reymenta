#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Json.h"

#include "WebSocketClient.h"
#include "WebSocketServer.h"

#include "ParameterBag.h"
// Utils
#include "Batchass.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta 
{

	typedef std::shared_ptr<class WebSockets> WebSocketsRef;

	class WebSockets 
	{

	public:
		WebSockets( ParameterBagRef aParameterBag, BatchassRef aBatchass );
		static						WebSocketsRef create( ParameterBagRef aParameterBag, BatchassRef aBatchass );
		void						update();
		void						setupSender();
		void						write(std::string msg);
		void						clientConnect();
		void						ping();
	private:
		// parameters
		ParameterBagRef				mParameterBag;
		// utils
		BatchassRef					mBatchass;

		// Web socket client
		void						clientDisconnect();
		WebSocketClient				mClient;
		void						onConnect();
		void						onDisconnect();
		void						onError(std::string err);
		void						onInterrupt();
		void						onPing(std::string msg);
		void						onRead(std::string msg);
		// Web socket  server
		WebSocketServer				mServer;
		void						serverConnect();
		void						serverDisconnect();
		double						mPingTime;

		//
		std::string					mText;
	};
}