/*
Copyright (c) 2014, Paul Houx - All rights reserved.
This code is intended for use with the Cinder C++ library: http://libcinder.org

Redistribution and use in source and binary forms, with or without modification, are permitted provided that
the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and
the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
#include "AudioWrapper.h"

using namespace Reymenta;

void AudioWrapper::setFftSize(int willBeMultipledByTwo)
{
	mFftSize = willBeMultipledByTwo * 2;
	//scopeLineInFmt = audio::MonitorSpectralNode::Format().fftSize(mFftSize).windowSize(mWindowSize);
}
void AudioWrapper::setWindowSize(int willBeMultipledByTwo)
{
	mWindowSize = willBeMultipledByTwo * 2;
	//scopeLineInFmt = audio::MonitorSpectralNode::Format().fftSize(mFftSize).windowSize(mWindowSize);
}

AudioWrapper::AudioWrapper(ParameterBagRef aParameterBag, TexturesRef aTexturesRef)
{
	mParameterBag = aParameterBag;
	mTextures = aTexturesRef;
	// instanciate the logger class
	log = Logger::create("AudioLog.txt");
	log->logTimedString("Audio constructor");
	
	mFftSize = 256;
	mWindowSize = 512;
	//kWidth = 256;//512;
	//kHeight = 256;//512;

	// number of frequency bands of our spectrum
	//kBands = 256;// 1024;
	//kHistory = 128;

	// linein
	auto ctx = audio::Context::master();
	mLineIn = ctx->createInputDeviceNode();

	//*mScopeLineInFmt = audio::MonitorSpectralNode::Format().fftSize(512).windowSize(256);
	auto scopeLineInFmt = audio::MonitorSpectralNode::Format().fftSize(mFftSize).windowSize(mWindowSize);
	mMonitorLineInSpectralNode = ctx->makeNode(new audio::MonitorSpectralNode(scopeLineInFmt));
	mLineIn >> mMonitorLineInSpectralNode;
	
	mLineIn->enable();

	// wave
	// TODO: it is pretty surprising when you recreate mScope here without checking if there has already been one added.
	//	- user will no longer see the old mScope, but the context still owns a reference to it, so another gets added each time we call this method.
	auto scopeWaveFmt = audio::MonitorSpectralNode::Format().fftSize(2048).windowSize(1024);
	mMonitorWaveSpectralNode = ctx->makeNode(new audio::MonitorSpectralNode(scopeWaveFmt));

	ctx->enable();

	// Paul Houx
	// setup camera
	mCamera.setPerspective(50.0f, 1.0f, 1.0f, 10000.0f);
	mCamera.setEyePoint(Vec3f(-kWidth / 4, kHeight / 2, -kWidth / 8));
	mCamera.setCenterOfInterestPoint(Vec3f(kWidth / 4, -kHeight / 8, kWidth / 4));

	// create channels from which we can construct our textures
	mChannelLeft = Channel32f(kBands, kHistory);
	mChannelRight = Channel32f(kBands, kHistory);
	memset(mChannelLeft.getData(), 0, mChannelLeft.getRowBytes() * kHistory);
	memset(mChannelRight.getData(), 0, mChannelRight.getRowBytes() * kHistory);

	// create texture format (wrap the y-axis, clamp the x-axis)
	mTextureFormat.setWrapS(GL_CLAMP);
	mTextureFormat.setWrapT(GL_REPEAT);
	mTextureFormat.setMinFilter(GL_LINEAR);
	mTextureFormat.setMagFilter(GL_LINEAR);

	// compile shader
	try {
		mShaders.push_back(gl::GlslProg(loadAsset("spectrum.vert"), loadAsset("spectrum.frag")));
	}
	catch (const std::exception& e) {
		log->logTimedString("Could not compile spectrum.frag or vert");
	}
	try {
		mShaders.push_back(gl::GlslProg(loadAsset("spectrum2.vert"), loadAsset("spectrum2.frag")));
	}
	catch (const std::exception& e) {
		log->logTimedString("Could not compile spectrum2.frag or vert");

	}
	currentShaderIndex = 0;
	// create static mesh (all animation is done in the vertex shader)
	std::vector<Vec3f>	vertices;
	std::vector<Colorf>	colors;
	std::vector<Vec2f>	coords;
	std::vector<size_t>	indices;

	for (size_t h = 0; h < kHeight; ++h)
	{
		for (size_t w = 0; w < kWidth; ++w)
		{
			// add polygon indices
			if (h < kHeight - 1 && w < kWidth - 1)
			{
				size_t offset = vertices.size();

				indices.push_back(offset);
				indices.push_back(offset + kWidth);
				indices.push_back(offset + kWidth + 1);
				indices.push_back(offset);
				indices.push_back(offset + kWidth + 1);
				indices.push_back(offset + 1);
			}

			// add vertex
			vertices.push_back(Vec3f(float(w), 0, float(h)));

			// add texture coordinates
			// note: we only want to draw the lower part of the frequency bands,
			//  so we scale the coordinates a bit
			const float part = 0.5f;
			float s = w / float(kWidth - 1);
			float t = h / float(kHeight - 1);
			coords.push_back(Vec2f(part - part * s, t));

			// add vertex colors
			colors.push_back(Color(CM_HSV, s, 0.5f, 0.75f));
		}
	}

	gl::VboMesh::Layout layout;
	layout.setStaticPositions();
	layout.setStaticColorsRGB();
	layout.setStaticIndices();
	layout.setStaticTexCoords2d();

	mMesh = gl::VboMesh(vertices.size(), indices.size(), layout, GL_TRIANGLES);
	mMesh.bufferPositions(vertices);
	mMesh.bufferColorsRGB(colors);
	mMesh.bufferIndices(indices);
	mMesh.bufferTexCoords2d(0, coords);


	mIsMouseDown = false;
	mMouseUpDelay = 30.0;
	mMouseUpTime = getElapsedSeconds() - mMouseUpDelay;

	// the texture offset has two purposes:
	//  1) it tells us where to upload the next spectrum data
	//  2) we use it to offset the texture coordinates in the shader for the scrolling effect
	mOffset = 0;
}

AudioWrapperRef AudioWrapper::create(ParameterBagRef aParameterBag, TexturesRef aTexturesRef)
{
	return shared_ptr<AudioWrapper>(new AudioWrapper(aParameterBag, aTexturesRef));
}
void AudioWrapper::loadWaveFile(string aFilePath)
{
	try
	{
		if (!fs::exists(aFilePath))
		{
			log->logTimedString("file not found: " + aFilePath);

		}
		else
		{
			log->logTimedString("found file: " + aFilePath);
			auto ctx = audio::master();
			mSourceFile = audio::load(loadFile(aFilePath), audio::master()->getSampleRate());
			mSamplePlayerNode = ctx->makeNode(new audio::FilePlayerNode(mSourceFile, false));
			mSamplePlayerNode->setLoopEnabled(false);
			mSamplePlayerNode >> mMonitorWaveSpectralNode >> ctx->getOutput();
			mSamplePlayerNode->enable();
			//ctx->enable();

			// or connect in series (it is added to the Context's 'auto pulled list')
			//mSamplePlayerNode >> ctx->getOutput();
			mSamplePlayerNode->seek(0);

			auto filePlayer = dynamic_pointer_cast<audio::FilePlayerNode>(mSamplePlayerNode);
			CI_ASSERT_MSG(filePlayer, "expected sample player to be either BufferPlayerNode or FilePlayerNode");

			filePlayer->setSourceFile(mSourceFile);

			//audio::master()->printGraph();

			mSamplePlayerNode->start();
			mParameterBag->mUseLineIn = false;
		}
	}
	catch (...)
	{
		log->logTimedString("loadWaveFile error: " + aFilePath);

	}
}

void AudioWrapper::update()
{
	if (mParameterBag->mUseLineIn || !mSamplePlayerNode)
	{
		mMagSpectrum = mMonitorLineInSpectralNode->getMagSpectrum();
	}
	else
	{
		mMagSpectrum = mMonitorWaveSpectralNode->getMagSpectrum();
	}
	if (mMagSpectrum.empty())
		return;

	unsigned char signal[kBands];
	mParameterBag->maxVolume = 0.0;
	size_t mDataSize = mMagSpectrum.size();
	if (mDataSize > 0)
	{
		float mv;
		float db;
		float maxdb = 0.0f;
		for (size_t i = 0; i < mDataSize; i++) {
			float f = mMagSpectrum[i];
			db = audio::linearToDecibel(f);
			f = db * mParameterBag->mAudioMultFactor;
			if (f > mParameterBag->maxVolume)
			{
				mParameterBag->maxVolume = f; mv = f;
			}
			mParameterBag->mData[i] = f;
			int ger = f;
			signal[i] = static_cast<unsigned char>(ger);

			if (db > maxdb) maxdb = db;

			switch (i)
			{
			case 11:
				mParameterBag->iFreqs[0] = f;
				break;
			case 13:
				mParameterBag->iFreqs[1] = f;
				break;
			case 15:
				mParameterBag->iFreqs[2] = f;
				break;
			case 18:
				mParameterBag->iFreqs[3] = f;
				break;
			default:
				break;
			}

		}
		// store it as a 512x2 texture in UPDATE only!!
		mTextures->setAudioTexture(signal);
	}

	// Paul Houx
	// get spectrum for left and right channels and copy it into our channels
	float* pDataLeft = mChannelLeft.getData() + kBands * mOffset;
	float* pDataRight = mChannelRight.getData() + kBands * mOffset;
	for (size_t i = 0; i < mDataSize; i++) {
		pDataLeft[i] = mMagSpectrum[i] * mParameterBag->mAudioMultFactor;
		pDataRight[i] = mMagSpectrum[i] * mParameterBag->mAudioMultFactor;
	}

	// increment texture offset
	mOffset = (mOffset + 1) % kHistory;

	// clear the spectrum for this row to avoid old data from showing up
	pDataLeft = mChannelLeft.getData() + kBands * mOffset;
	pDataRight = mChannelRight.getData() + kBands * mOffset;
	memset(pDataLeft, 0, kBands * sizeof(float));
	memset(pDataRight, 0, kBands * sizeof(float));

	// animate camera if mouse has not been down for more than 30 seconds
	if (!mIsMouseDown && (getElapsedSeconds() - mMouseUpTime) > mMouseUpDelay)
	{
		float t = float(getElapsedSeconds());
		float x = 0.5f + 0.5f * math<float>::cos(t * 0.07f);
		float y = 0.1f - 0.2f * math<float>::sin(t * 0.09f);
		float z = 0.25f * math<float>::sin(t * 0.05f) - 0.25f;
		Vec3f eye = Vec3f(kWidth * x, kHeight * y, kHeight * z);

		x = 1.0f - x;
		y = -0.3f;
		z = 0.6f + 0.2f *  math<float>::sin(t * 0.12f);
		Vec3f interest = Vec3f(kWidth * x, kHeight * y, kHeight * z);

		// gradually move to eye position and center of interest
		mCamera.setEyePoint(eye.lerp(0.995f, mCamera.getEyePoint()));
		mCamera.setCenterOfInterestPoint(interest.lerp(0.990f, mCamera.getCenterOfInterestPoint()));
		//mParameterBag->mCamera.setEyePoint(eye.lerp(0.995f, mCamera.getEyePoint()));
		//mParameterBag->mCamera.setCenterOfInterestPoint(interest.lerp(0.990f, mCamera.getCenterOfInterestPoint()));
	}

}

void AudioWrapper::draw()
{
	// Set up window	
	mTextures->getFbo(mParameterBag->mAudioFboIndex).bindFramebuffer();
	gl::setViewport(mTextures->getFbo(mParameterBag->mAudioFboIndex).getBounds());	

	gl::clear();
	currentShaderIndex = mParameterBag->mMeshIndex;
	if (mParameterBag->mMeshIndex > mShaders.size() - 1)
	{
		currentShaderIndex = mShaders.size() - 1;
	}

	// use camera
	gl::pushMatrices();
	if (mParameterBag->iFade)
	{
		gl::setMatrices(mParameterBag->mCamera);
	}
	else
	{
		gl::setMatrices(mCamera);
	}
	{
		// bind shader
		mShaders[currentShaderIndex].bind();
		mShaders[currentShaderIndex].uniform("uTexOffset", mOffset / float(kHistory));
		mShaders[currentShaderIndex].uniform("uLeftTex", 0);
		mShaders[currentShaderIndex].uniform("uRightTex", 1);
		mShaders[currentShaderIndex].uniform("iRatio", mParameterBag->controlValues[11]);
		mShaders[currentShaderIndex].uniform("iZoom", mParameterBag->controlValues[13]);

		// create textures from our channels and bind them
		mTextureLeft = gl::Texture(mChannelLeft, mTextureFormat);
		mTextureRight = gl::Texture(mChannelRight, mTextureFormat);

		mTextureLeft.enableAndBind();
		mTextureRight.bind(1);

		// draw mesh using additive blending
		gl::enableAdditiveBlending();
		gl::color(Color(1, 1, 1));
		gl::draw(mMesh);
		gl::disableAlphaBlending();

		// unbind textures and shader
		mTextureRight.unbind();
		mTextureLeft.unbind();
		mShaders[currentShaderIndex].unbind();
	}
	gl::popMatrices();
	mTextures->getFbo(mParameterBag->mAudioFboIndex).unbindFramebuffer();
}

void AudioWrapper::mouseDown(MouseEvent event)
{
	// handle mouse down
	mIsMouseDown = true;

	mMayaCam.setCurrentCam(mCamera);
	mMayaCam.mouseDown(mParameterBag->mCamPosXY);
}

void AudioWrapper::mouseDrag(MouseEvent event)
{
	// handle mouse drag
	mMayaCam.mouseDrag(mParameterBag->mCamPosXY, event.isLeftDown(), event.isMiddleDown(), event.isRightDown());
	mCamera = mMayaCam.getCamera();
}

void AudioWrapper::mouseUp(MouseEvent event)
{
	// handle mouse up
	mMouseUpTime = getElapsedSeconds();
	mIsMouseDown = false;
}