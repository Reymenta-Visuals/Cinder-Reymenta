#pragma once

#include "cinder/app/AppNative.h"
#include "WebSocketClient.h"
#include "WebSocketServer.h"

#include "ParameterBag.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta 
{

	typedef std::shared_ptr<class WebSockets> WebSocketsRef;

	class WebSockets 
	{

	public:
		WebSockets( ParameterBagRef aParameterBag );
		static	WebSocketsRef create( ParameterBagRef aParameterBag );
		void	update();
		void	setupSender();
	private:
		// parameters
		ParameterBagRef				mParameterBag;
		// Web socket client
		void						clientConnect();
		void						clientDisconnect();
		WebSocketClient				mClient;
		void						onConnect();
		void						onDisconnect();
		void						onError(std::string err);
		void						onInterrupt();
		void						onPing(std::string msg);
		void						onRead(std::string msg);
		void						write();
		// Web socket  server
		WebSocketServer				mServer;
		void						serverConnect();
		void						serverDisconnect();

		//
		std::string					mText;
	};
}