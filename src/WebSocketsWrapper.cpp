#include "WebSocketsWrapper.h"

using namespace Reymenta;

WebSockets::WebSockets(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;
	mClient.addConnectCallback(&WebSockets::onConnect, this);
	mClient.addDisconnectCallback(&WebSockets::onDisconnect, this);
	mClient.addErrorCallback(&WebSockets::onError, this);
	mClient.addInterruptCallback(&WebSockets::onInterrupt, this);
	mClient.addPingCallback(&WebSockets::onPing, this);
	mClient.addReadCallback(&WebSockets::onRead, this);
	connect();
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
	mClient.poll();
}
void WebSockets::connect()
{
	mClient.connect("ws://localhost:9002");
}
void WebSockets::disconnect()
{
	mClient.disconnect();
}
void WebSockets::onConnect()
{
	mText = "Connected";
	mClient.ping();
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
	if (!msg.empty()) {
		mText += ": " + msg;
	}
}

void WebSockets::onRead(string msg)
{
	mText = "Read";
	if (!msg.empty()) {
		mText += ": " + msg;
	}
}
