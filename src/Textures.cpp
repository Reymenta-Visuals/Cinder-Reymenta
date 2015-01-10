/**
* \file Textures.cpp
* \author Bruce LANE
* \date 20 november 2014
*
* Manages the textures.
*
*/
#include "Textures.h"

using namespace Reymenta;

Textures::Textures(ParameterBagRef aParameterBag, ShadersRef aShadersRef)
{
	mParameterBag = aParameterBag;
	mShaders = aShadersRef;
	// instanciate the logger class
	log = Logger::create("RenderTexturesLog.txt");
	log->logTimedString("Textures constructor");

	// init texture
	//startupImage = gl::Texture::create(loadImage(loadResource(IMG)), gl::Texture2d::Format().loadTopDown());

	// init
	string pathToStartupFile = (getAssetPath("") / "startup.jpg").string();
	if (fs::exists(pathToStartupFile))
	{
		log->logTimedString("startup.jpg image found, loading");
		startupImage = gl::Texture::create(loadImage(loadAsset("startup.jpg")), gl::Texture2d::Format().loadTopDown());
		log->logTimedString("startup.jpg image loaded");
	}
	createTexture("startup image", mParameterBag->mFboWidth, mParameterBag->mFboHeight, startupImage);
	// if no input received we wait for the 1st one to replace the startup image
	inputReceived = false;
	// create a rectangle to be drawn with our shader program
	// default is from -0.5 to 0.5, so we scale by 2 to get -1.0 to 1.0
	// coming soon in Cinder? mMesh = gl::VboMesh::create(geom::Rect().scale(vec2(2.0f, 2.0f))); 
	mMesh = gl::VboMesh::create(geom::Rect(Rectf(-2.0, -2.0, 2.0, 2.0)));
	selectedShada = 0;
	selectedInputTexture = 0;
	addShadaFbos();
	// init mixTextures
	for (int a = 0; a < MAX; a++)
	{
		mMixesFbos[a] = gl::Fbo::create(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight, fboFormat.depthTexture());
	}
	createWarpInputs();
	//createWarpFbos();
	log->logTimedString("Textures constructor end");
}
/*void Textures::createWarpFbos()
{
	for (int a = 0; a < MAX; a++)
	{
		WarpFbo newWarpFbo;
		newWarpFbo.textureIndex = 0;
		newWarpFbo.textureMode = 1;
		newWarpFbo.fbo = gl::Fbo::create(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight, fboFormat.depthTexture());
		if (a == 0) newWarpFbo.active = true; else newWarpFbo.active = false;
		mWarpFbos[a] = newWarpFbo;
	}
}*/
void Textures::createWarpInputs()
{
	for (int a = 0; a < MAX; a++)
	{
		WarpInput newWarpInput;
		newWarpInput.leftIndex = 0;
		newWarpInput.leftMode = 0;
		newWarpInput.rightIndex = 0;
		newWarpInput.rightMode = 1;
		newWarpInput.iCrossfade = 1.0;
		newWarpInput.hasTexture = false;
		if (a == 0) newWarpInput.active = true; else  newWarpInput.active = false;
		warpInputs[a] = newWarpInput;
	}
}
void Textures::loadFileFromAssets(string &fileName)
{
	string ext = "";
	// use the last of the dropped files
	boost::filesystem::path mPath = getAssetPath("") / fileName;
	string mFile = mPath.string();
	if (mFile.find_last_of(".") != std::string::npos) ext = mFile.substr(mFile.find_last_of(".") + 1);
	log->logTimedString("loading: " + mFile + " ext: " + ext);

	if (ext == "mov")
	{
		try {
			mMovie.reset();
			// load up the movie, set it to loop, and begin playing
			mMovie = qtime::MovieGlHap::create(mFile);
			mMovie->setLoop(false);
			mMovie->play();
		}
		catch (...) {
			log->logTimedString("loadMovieFromAssets: Unable to load the movie.");
		}
	}
}
void Textures::stopMovie(bool remove)
{
	if (mMovie)
	{
		mMovie->stop();
		if (remove) mMovie.reset();
	}
}
void Textures::playMovie()
{
	if (mMovie)
	{
		mMovie->play();
	}
}

void Textures::loadMovie(const fs::path &movie_path)
{
	try {
		mMovie.reset();
		// load up the movie, set it to loop, and begin playing
		mMovie = qtime::MovieGlHap::create(movie_path);
		mMovie->setLoop(false);
		mMovie->play();

	}
	catch (...) {
		log->logTimedString("loadMovie: Unable to load the movie.");
	}
}
void Textures::fileDrop(string mFile)
{
	string ext = "";
	if (mFile.find_last_of(".") != std::string::npos) ext = mFile.substr(mFile.find_last_of(".") + 1);
	log->logTimedString(mFile + " dropped");

	if (ext == "glsl")
	{
		if (mShaders->loadPixelFragmentShader(mFile))
		{
			mParameterBag->controlValues[13] = 1.0f;
			warpInputs[0].rightIndex = 0;
			warpInputs[0].rightMode = 1;
			warpInputs[0].iCrossfade = 1.0;
			warpInputs[0].active = true;
			mParameterBag->mWarpFbos[0].textureIndex = 0;
			mParameterBag->mWarpFbos[0].textureMode = 1;
			mParameterBag->mWarpFbos[0].active = true;
		}
	}
	else if (ext == "mov")
	{
		loadMovie(mFile);
	}
}

void Textures::setShadaIndex(int index)
{
	log->logTimedString("setShadaIndex:" + toString(index));
	selectedShada = index;
}
void Textures::setInputTextureIndex(int index)
{
	log->logTimedString("setInputTextureIndex:" + toString(index));
	selectedInputTexture = index;
}
void Textures::setWarpInputModeRight(int index, bool shaderMode)
{
	warpInputs[min((MAX)-1, index)].rightMode = shaderMode;
}

WarpInput Textures::setInput(int index, bool left, int currentMode)
{
	string name;
	if (currentMode == 0)
	{
		selectedInputTexture = index;
		// 0 = input texture mode
		name = "T" + toString(selectedInputTexture);
		if (left)
		{
			warpInputs[mParameterBag->selectedWarp].leftIndex = selectedInputTexture;
			warpInputs[mParameterBag->selectedWarp].leftMode = 0; // 0 for input texture
			if (!warpInputs[mParameterBag->selectedWarp].hasTexture)
			{
				// put texture on both sides
				warpInputs[mParameterBag->selectedWarp].hasTexture = true;
				warpInputs[mParameterBag->selectedWarp].rightIndex = selectedInputTexture;
				warpInputs[mParameterBag->selectedWarp].rightMode = 0; // 0 for input texture
			}
		}
		else
		{
			warpInputs[mParameterBag->selectedWarp].rightIndex = selectedInputTexture;
			warpInputs[mParameterBag->selectedWarp].rightMode = 0; // 0 for input texture
			if (!warpInputs[mParameterBag->selectedWarp].hasTexture)
			{
				// put texture on both sides
				warpInputs[mParameterBag->selectedWarp].hasTexture = true;
				warpInputs[mParameterBag->selectedWarp].leftIndex = selectedInputTexture;
				warpInputs[mParameterBag->selectedWarp].leftMode = 0; // 0 for input texture
			}
		}
	}
	else
	{
		// 1 = shader mode
		selectedShada = index;
		//??? mShadaFbos[index].shadaIndex = selectedShada;
		name = "S" + toString(selectedShada);
		if (left)
		{
			warpInputs[mParameterBag->selectedWarp].leftIndex = selectedShada;
			warpInputs[mParameterBag->selectedWarp].leftMode = 1; // 1 for shader
			if (!warpInputs[mParameterBag->selectedWarp].hasTexture)
			{
				// put texture on both sides
				warpInputs[mParameterBag->selectedWarp].hasTexture = true;
				warpInputs[mParameterBag->selectedWarp].rightIndex = selectedShada;
				warpInputs[mParameterBag->selectedWarp].rightMode = 1; // 1 for shader
			}
		}
		else
		{
			warpInputs[mParameterBag->selectedWarp].rightIndex = selectedShada;
			warpInputs[mParameterBag->selectedWarp].rightMode = 1; // 1 for shader
			if (!warpInputs[mParameterBag->selectedWarp].hasTexture)
			{
				// put texture on both sides
				warpInputs[mParameterBag->selectedWarp].hasTexture = true;
				warpInputs[mParameterBag->selectedWarp].leftIndex = selectedShada;
				warpInputs[mParameterBag->selectedWarp].leftMode = 1; // 1 for shader
			}

		}
	}
	log->logTimedString(name + " on selected warp: " + toString(mParameterBag->selectedWarp));
	return warpInputs[mParameterBag->selectedWarp];
}

void Textures::addShadaFbos()
{
	for (int a = 0; a < MAX; a++)
	{
		mShadaFbos[a].fbo = gl::Fbo::create(mParameterBag->mFboWidth, mParameterBag->mFboHeight, fboFormat.depthTexture());
		mShadaFbos[a].shadaIndex = a;
		if (a == 0) mShadaFbos[a].active = true; else mShadaFbos[a].active = false;
	}
}

int Textures::createSpoutTexture(char name[256], unsigned int width, unsigned int height)
{
	if (inputReceived)
	{
		log->logTimedString("createSpoutTexture, new: " + toString(name));
		Sender newTexture;
		// create new texture
		memcpy(&newTexture.SenderName[0], name, strlen(name) + 1);
		newTexture.width = width;
		newTexture.height = height;
		newTexture.texture = gl::Texture::create(width, height);
		//! add to the inputTextures vector
		inputTextures.push_back(newTexture);
	}
	else
	{
		// replace startup image
		log->logTimedString("createSpoutTexture, replace: " + toString(name));
		inputReceived = true;
		memcpy(&inputTextures[0].SenderName[0], name, strlen(name) + 1);
		inputTextures[0].width = width;
		inputTextures[0].height = height;
		inputTextures[0].texture = gl::Texture::create(width, height);
	}
	return inputTextures.size() - 1;
}

int Textures::createTexture(char name[256], unsigned int width, unsigned int height, gl::TextureRef texture)
{
	Sender newTexture;
	// create new texture
	memcpy(&newTexture.SenderName[0], name, strlen(name) + 1);
	newTexture.width = width;
	newTexture.height = height;
	newTexture.texture = texture;
	//! add to the inputTextures vector
	inputTextures.push_back(newTexture);
	return inputTextures.size() - 1;
}
char* Textures::getSenderName(int index)
{
	return &inputTextures[checkedIndex(index)].SenderName[0];
}
ci::gl::TextureRef Textures::getSenderTexture(int index)
{
	return inputTextures[checkedIndex(index)].texture;
}
void Textures::setSenderTextureSize(int index, int width, int height)
{
	inputTextures[checkedIndex(index)].width = width;
	inputTextures[checkedIndex(index)].height = height;
	inputTextures[checkedIndex(index)].texture = gl::Texture::create(width, height);
}

void Textures::setAudioTexture(int audioTextureIndex, unsigned char *signal)
{
	inputTextures[audioTextureIndex].texture = gl::Texture::create(signal, GL_LUMINANCE16I_EXT, 512, 2);
}
int Textures::checkedIndex(int index)
{
	int i = min(index, int(inputTextures.size() - 1));
	return max(i, 0);
}

void Textures::setTextureFromFile(string fileName)
{
	string shortName = "image";
	char *name;
	try
	{
		if (!fs::exists(fileName))
		{
			log->logTimedString("asset file not found: " + fileName);
		}
		else
		{
			log->logTimedString("asset found file: " + fileName);
			fs::path fr = fileName;
			string mFile = fr.string();
			if (mFile.find_last_of("\\") != std::string::npos)
			{
				shortName = mFile.substr(mFile.find_last_of("\\") + 1);
			}
			name = &shortName[0];
			gl::TextureRef newTexture = gl::Texture::create(loadImage(fileName));
			createTexture(name, newTexture->getWidth(), newTexture->getHeight(), newTexture);
			log->logTimedString("asset loaded: " + fileName);
		}
	}
	catch (...)
	{
		log->logTimedString("Load asset error: " + fileName);
	}
}

void Textures::saveThumb()
{
	string filename = mShaders->getFragFileName() + ".jpg";
	try
	{
		mShadaFbos[mParameterBag->mCurrentShadaFboIndex].fbo->bindFramebuffer();
		// Should get the FBO's pixels since it is bound (instead of the screen's)
		Surface fboSurf = copyWindowSurface(Area(ivec2(0, 0), ivec2(mParameterBag->mFboWidth, mParameterBag->mFboHeight)));
		mShadaFbos[mParameterBag->mCurrentShadaFboIndex].fbo->unbindFramebuffer();
		fs::path path = getAssetPath("") / "thumbs" / filename;
		writeImage(path, ImageSourceRef(fboSurf));
		log->logTimedString("saved:" + filename);
		int i = 0;
		for (auto &mFbo : mShadaFbos)
		{
			filename = mShaders->getFragFileName() + static_cast<ostringstream*>(&(ostringstream() << i))->str() + ".jpg";
			mFbo.fbo->bindFramebuffer();
			// Should get the FBO's pixels since it is bound (instead of the screen's)
			Surface fboSurf = copyWindowSurface(Area(ivec2(0, 0), ivec2(mParameterBag->mFboWidth, mParameterBag->mFboHeight)));
			mFbo.fbo->unbindFramebuffer();
			fs::path path = getAssetPath("") / "thumbs" / filename;
			writeImage(path, ImageSourceRef(fboSurf));
			i++;
		}
	}
	catch (const std::exception &e)
	{
		log->logTimedString("unable to save:" + filename + string(e.what()));
	}
}

void Textures::update()
{

}
ci::gl::TextureRef Textures::getTexture(int index)
{
	return inputTextures[checkedIndex(index)].texture;
}
ci::gl::TextureRef Textures::getMixTexture(int index)
{
	if (index > MAX - 1) index = MAX - 1;
	return mMixesFbos[index]->getColorTexture();
}
ci::gl::TextureRef Textures::getWarpTexture(int index)
{
	if (index > MAX - 1) index = MAX - 1;
	return mParameterBag->mWarpFbos[index].fbo->getColorTexture();
}
ci::gl::TextureRef Textures::getFboTexture(int index)
{
	if (index > MAX - 1) index = MAX - 1;
	return mShadaFbos[index].fbo->getColorTexture();
}
void Textures::renderShadersToFbo()
{
	for (auto &mFbo : mShadaFbos)
	{
		if (mFbo.active)
		{
			// this will restore the old framebuffer binding when we leave this function
			// on non-OpenGL ES platforms, you can just call mFbo->unbindFramebuffer() at the end of the function
			// but this will restore the "screen" FBO on OpenGL ES, and does the right thing on both platforms
			gl::ScopedFramebuffer fbScp(mFbo.fbo);
			// clear out the FBO with black
			gl::clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));

			// setup the viewport to match the dimensions of the FBO
			gl::ScopedViewport scpVp(ivec2(0.0), mFbo.fbo->getSize());

			gl::ScopedGlslProg shader(mShaders->getShader(mFbo.shadaIndex));
			getFboTexture(0)->bind(0);
			getSenderTexture(1)->bind(1);
			// draw our screen rectangle
			gl::draw(mMesh);

		}
	}
}
void Textures::renderMixesToFbo()
{
	int i = 0;
	for (auto &mFbo : mMixesFbos)
	{
		if (warpInputs[i].active)
		{
			// this will restore the old framebuffer binding when we leave this function
			// on non-OpenGL ES platforms, you can just call mFbo->unbindFramebuffer() at the end of the function
			// but this will restore the "screen" FBO on OpenGL ES, and does the right thing on both platforms
			gl::ScopedFramebuffer fbScp(mFbo);
			// clear out the FBO with black
			gl::clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));

			// setup the viewport to match the dimensions of the FBO
			gl::ScopedViewport scpVp(ivec2(0.0), mFbo->getSize());

			gl::ScopedGlslProg shader(mShaders->getMixShader());

			if (warpInputs[i].leftMode == 0)
			{
				// 0 for input texture
				getSenderTexture(warpInputs[i].leftIndex)->bind(0);
			}
			else
			{
				// 1 for shader
				getFboTexture(warpInputs[i].leftIndex)->bind(0);
			}
			if (warpInputs[i].rightMode == 0)
			{
				// 0 for input texture
				getSenderTexture(warpInputs[i].rightIndex)->bind(1);
			}
			else
			{
				// 1 for shader
				getFboTexture(warpInputs[i].rightIndex)->bind(1);
			}
			//mShaders->getMixShader()->uniform("iCrossfade", warpInputs[i].iCrossfade);
			mShaders->getMixShader()->uniform("iCrossfade", mParameterBag->controlValues[17]);
			//warpInputs[i].iCrossfade += 0.1;
			//if (warpInputs[i].iCrossfade > 1.0) warpInputs[i].iCrossfade = 0.0;
			gl::draw(mMesh);
		}
		i++;
	}
}
void Textures::renderWarpFbos()
{
	int i = 0;
	for (auto &mWarp : mParameterBag->mWarpFbos)
	{
		if (mWarp.active)
		{
			gl::ScopedFramebuffer fbScp(mWarp.fbo);
			// clear out the FBO with black
			gl::clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));

			// setup the viewport to match the dimensions of the FBO
			gl::ScopedViewport scpVp(ivec2(0.0), mWarp.fbo->getSize());
			//gl::ScopedViewport scpVp(ivec2(0.0), ivec2(mParameterBag->mFboWidth, mParameterBag->mFboHeight));

			gl::ScopedGlslProg shader(mShaders->getWarpShader());
			if (mMovie && mMovie->isPlaying() && mMovie->getTexture())
			{
				mMovie->getTexture()->bind(0);
			}
			else if (mWarp.textureMode == 0)
			{
				// 0 for input texture
				getSenderTexture(mWarp.textureIndex)->bind(0);
			}
			else
			{
				// 1 for shader
				getFboTexture(mWarp.textureIndex)->bind(0);
			}
			mShaders->getWarpShader()->uniform("iAlpha", mParameterBag->controlValues[4]);
			gl::draw(mMesh);

			i++;
		}
	}
}
void Textures::draw()
{
	//! 1 render the active shaders to Fbos
	renderShadersToFbo();
	//! 2 render mixes of shader Fbos as texture, images, Spout sources as Fbos
	renderMixesToFbo();
	//! 3 render warp Fbos as texture, images, Spout sources as Fbos
	renderWarpFbos();

	if (mMovie)
	{
		//mMovie->draw();
	}
}

void Textures::shutdown()
{
	//TODO for inputTextures sTextures.clear();
}
