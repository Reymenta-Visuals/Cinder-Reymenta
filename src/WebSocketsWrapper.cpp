#include "WebSocketsWrapper.h"

using namespace Reymenta;

WebSockets::WebSockets(ParameterBagRef aParameterBag, BatchassRef aBatchass)
{
	mParameterBag = aParameterBag;
	mBatchass = aBatchass;
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
	mPingTime = getElapsedSeconds();

}

WebSocketsRef WebSockets::create(ParameterBagRef aParameterBag, BatchassRef aBatchass)
{
	return shared_ptr<WebSockets>(new WebSockets(aParameterBag, aBatchass));
}
void WebSockets::setupSender()
{
}
void WebSockets::ping()
{
	if (!mParameterBag->mIsWebSocketsServer)
	{
		mClient.ping();
	}
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
		double e = getElapsedSeconds();
		if (e - mPingTime > 20.0) {
			mClient.ping();
			mPingTime = e;
		}
	}
}
void WebSockets::clientConnect()
{
	stringstream s;
	s << "ws://" << mParameterBag->mWebSocketsHost << ":" << mParameterBag->mWebSocketsPort;
	mClient.connect(s.str());
}
void WebSockets::clientDisconnect()
{
	mClient.disconnect();
}
void WebSockets::onConnect()
{
	mParameterBag->mMsg = "Connected";
	mParameterBag->newMsg = true;
}

void WebSockets::onDisconnect()
{
	mParameterBag->mMsg = "Disconnected";
	mParameterBag->newMsg = true;
}

void WebSockets::onError(string err)
{
	mParameterBag->mMsg = "WS Error";
	mParameterBag->newMsg = true;
	if (!err.empty()) {
		mParameterBag->mMsg += ": " + err;
	}

}

void WebSockets::onInterrupt()
{
	mParameterBag->mMsg = "WS Interrupted";
	mParameterBag->newMsg = true;
}

void WebSockets::onPing(string msg)
{
	mParameterBag->mMsg = "WS Ponged";
	mParameterBag->newMsg = true;
	if (!msg.empty())
	{
		mParameterBag->mMsg += ": " + msg;
	}
}

void WebSockets::onRead(string msg)
{
	mParameterBag->mMsg = "WS onRead";
	mParameterBag->newMsg = true;
	if (!msg.empty())
	{
		mParameterBag->mMsg += ": " + msg;
		string first = msg.substr(0, 1);
		if (first == "{")
		{
			// json
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
				mParameterBag->mMsg += " error jsonparser exception: ";
				mParameterBag->mMsg += exception.what();
				mParameterBag->mMsg += "  ";
			}
		}
		else if (first == "#")
		{
			// fragment shader from live coding
			mBatchass->getShadersRef()->loadLiveShader(msg);

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
