#include "WebSocketsWrapper.h"

using namespace Reymenta;

WebSockets::WebSockets(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;
	// either a client or a server
	if (mParameterBag->mIsWebSocketsServer)
	{
		mServer.addConnectCallback(&WebSockets::onConnect, this);
		mServer.addDisconnectCallback(&WebSockets::onDisconnect, this);
		mServer.addErrorCallback(&WebSockets::onError, this);
		mServer.addInterruptCallback(&WebSockets::onInterrupt, this);
		mServer.addPingCallback(&WebSockets::onPing, this);
		mServer.addReadCallback(&WebSockets::onRead, this);
		mServer.listen(mParameterBag->mWebSocketsPort);
	}
	else
	{
		mClient.addConnectCallback(&WebSockets::onConnect, this);
		mClient.addDisconnectCallback(&WebSockets::onDisconnect, this);
		mClient.addErrorCallback(&WebSockets::onError, this);
		mClient.addInterruptCallback(&WebSockets::onInterrupt, this);
		mClient.addPingCallback(&WebSockets::onPing, this);
		mClient.addReadCallback(&WebSockets::onRead, this);
		clientConnect();
	}


}

WebSocketsRef WebSockets::create(ParameterBagRef aParameterBag)
{
	return shared_ptr<WebSockets>(new WebSockets(aParameterBag));
}
void WebSockets::setupSender()
{
}
void WebSockets::update()
{
	if (mParameterBag->mIsWebSocketsServer)
	{
		mServer.poll();
	}
	else
	{
		mClient.poll();
	}
}
void WebSockets::clientConnect()
{
	stringstream s;
	s << "ws://" << mParameterBag->mWebSocketsHost << ":" << mParameterBag->mWebSocketsPort;
	mClient.connect( s.str() );//"ws://localhost:9002"
}
void WebSockets::clientDisconnect()
{
	mClient.disconnect();
}
void WebSockets::onConnect()
{
	mText = "Connected";
}

void WebSockets::onDisconnect()
{
	mText = "Disconnected";
}

void WebSockets::onError(string err)
{
	mText = "Error";
	if (!err.empty()) {
		mText += ": " + err;
	}
}

void WebSockets::onInterrupt()
{
	mText = "Interrupted";
}

void WebSockets::onPing(string msg)
{
	mText = "Ponged";
	if (!msg.empty()) 
	{
		mText += ": " + msg;
	}
}

void WebSockets::onRead(string msg)
{
	mText = "Read";
	mParameterBag->WSMsg = msg;
	mParameterBag->newWSMsg = true;
	if (!msg.empty()) 
	{
		mText += ": " + msg;
		string first = msg.substr(0, 1);
		// json
		if (first == "{")
		{
			JsonTree json;
			try
			{
				json = JsonTree(msg);
				JsonTree jsonParams = json.getChild("colors");
				for (JsonTree::ConstIter jsonElement = jsonParams.begin(); jsonElement != jsonParams.end(); ++jsonElement)
				{
					int name = jsonElement->getChild("name").getValue<int>();
					float value = jsonElement->getChild("value").getValue<float>();
					mParameterBag->controlValues[name] = value;
				}
			}
			catch (cinder::JsonTree::Exception exception)
			{
				mText += " error jsonparser exception: ";
				mText += exception.what();
				mText += "  ";
			}
		}


	}
}
void WebSockets::write(string msg)
{
	if (mParameterBag->mIsWebSocketsServer)
	{
		mServer.write(msg);
	}
	else
	{
		mClient.write(msg);

	}
}
