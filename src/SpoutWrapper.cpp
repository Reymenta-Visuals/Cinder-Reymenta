#include "SpoutWrapper.h"

using namespace Reymenta;

SpoutWrapper::SpoutWrapper(ParameterBagRef aParameterBag, TexturesRef aTextures)
{
	mParameterBag = aParameterBag;
	mTextures = aTextures;
	// instanciate the logger class
	CI_LOG_V("SpoutWrapper constructor");

	bInitialized = false;
	nReceivers = 0;

	CI_LOG_V("SpoutWrapper constructor end");
}

void SpoutWrapper::update()
{
	bool found = false;
	nSenders = mSpoutReceivers[0].GetSenderCount();

	if (nSenders != nReceivers && nSenders > 0)
	{
		//! the name will be filled when the receiver connects to a sender
		mNewSenderName[0] = NULL;
		CI_LOG_V("new sender found or sender deleted");

		nReceivers = 0;
		//! loop to find existing sender with that name
		for (int i = 0; i < nSenders; i++)
		{
			mSpoutReceivers[0].GetSenderName(i, &mNewSenderName[0], MaxSize);
			memcpy(&SenderNames[i][0], mNewSenderName, strlen(mNewSenderName) + 1);

			if (mSpoutReceivers[i].CreateReceiver(&SenderNames[i][0], mNewWidth, mNewHeight))
			{
				bInitialized = true;
				//mTextures->setSenderTextureSize(i, mNewWidth, mNewHeight);
				if (i > int(receiverIndexes.size()) - 1)
				{
					receiverIndexes.push_back(mTextures->createSpoutTexture(&SenderNames[i][0], mNewWidth, mNewHeight));
				}
				CI_LOG_V("create receiver");
				CI_LOG_V(&SenderNames[i][0]);
				nReceivers++;
				CI_LOG_V("new receiver count:");
				CI_LOG_V(mNewSenderName);
				mParameterBag->mWarpFbos[0].textureIndex = 0;
				mParameterBag->mWarpFbos[0].textureMode = mParameterBag->TEXTUREMODEINPUT;
			}
		}
	}
	if (nSenders == 0)
	{
		nReceivers = 0;
		// reset to shader mode
		mParameterBag->mWarpFbos[0].textureMode = mParameterBag->TEXTUREMODESHADER;
	}
}

void SpoutWrapper::draw()
{
	unsigned int width, height;
	int actualReceivers = 0;
	gl::setMatricesWindow(getWindowSize());

	//
	// Try to receive the texture at the current size 
	//
	// NOTE : if ReceiveTexture is called with a framebuffer object bound, that binding will be lost
	// and has to be restored afterwards because Spout uses an fbo for intermediate rendering
	if (bInitialized)
	{
		for (int i = 0; i < nReceivers; i++)
		{
			if (mSpoutReceivers[i].ReceiveTexture(&SenderNames[i][0], width, height, mTextures->getTexture(receiverIndexes[i])->getId(), mTextures->getTexture(receiverIndexes[i])->getTarget()))
			{
				mTextures->setSenderTextureSize(receiverIndexes[i], width, height);
				actualReceivers++;
			}
			else
			{
				mSpoutReceivers[i].ReleaseReceiver();
			}
		}
	}
	nReceivers = actualReceivers;
}

void SpoutWrapper::shutdown()
{
	for (int i = 0; i < nReceivers; i++)
	{
		mSpoutReceivers[i].ReleaseReceiver();
	}
}
