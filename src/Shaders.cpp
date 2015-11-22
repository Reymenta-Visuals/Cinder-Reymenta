#include "Shaders.h"

using namespace Reymenta;

Shaders::Shaders(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;

	CI_LOG_V("Shaders constructor");
	liveError = true;
	isLiveShaderSetup = false;
	previousFileName = "0";
	currentFileName = "0";

	mCurrentPreviewShader = 0;
	mCurrentRenderShader = 0;
	fs::path passthruVertFile;
	passthruVertFile = getAssetPath("") / "passthru.vert";
	passthruvert = loadString(loadFile(passthruVertFile));
	//load mix shader
	try
	{
		fs::path mixFragFile = getAssetPath("") / "mix.frag";
		if (fs::exists(mixFragFile))
		{
			mMixShader = gl::GlslProg::create(loadAsset("passthru.vert"), loadFile(mixFragFile));
		}
		else
		{
			CI_LOG_V("mix.frag does not exist, should quit");

		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V("unable to load/compile shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V("unable to load shader:" + string(e.what()));
	}
	//load warp shader
	try
	{
		fs::path warpFragFile = getAssetPath("") / "passthru.frag";
		if (fs::exists(warpFragFile))
		{
			mWarpShader = gl::GlslProg::create(loadAsset("passthru.vert"), loadFile(warpFragFile));
			mPassThruShader = gl::GlslProg::create(loadAsset("passthru.vert"), loadFile(warpFragFile));
		}
		else
		{
			CI_LOG_V("passthru.frag does not exist, should quit");

		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V("unable to load/compile shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V("unable to load shader:" + string(e.what()));
	}

	// shadertoy include
	vs = loadString(loadAsset("live.vert"));
	shaderInclude = loadString(loadAsset("shadertoy.inc"));

	validFrag = false;
	validVert = true;
	// live frag file
	liveFragFile = getAssetPath("") / "live.frag";
	if (fs::exists(liveFragFile))
	{
		// Load our shader and test if it is correctly compiled
		try
		{
			mParameterBag->mShaderToLoad = loadString(loadAsset("live.frag"));
			mLiveShader = gl::GlslProg::create(loadAsset("live.vert"), loadFile(liveFragFile));
			liveError = false;
		}
		catch (gl::GlslProgCompileExc exc){
			console() << exc.what() << endl;
		}
	}
	else
	{
		CI_LOG_V("live.frag does not exist");
	}
	//fileName = "default.frag";
	fs::path localFile; //= getAssetPath("") / "shaders" / fileName;
	// load 0.glsl to 4.glsl from assets folder
	for (size_t m = 0; m < 5; m++)
	{
		fileName = toString(m) + ".glsl";
		localFile = getAssetPath("") / fileName;
		mFragFileName = fileName;
		mFragFile = localFile.string();
		loadPixelFragmentShader(localFile.string());
	}
	// load 5.glsl to 15.glsl from assets folder
	for (size_t m = 5; m < mParameterBag->MAX; m++)
	{
		fileName = toString(m) + ".glsl";
		localFile = getAssetPath("") / mParameterBag->mAssetsPath / fileName;
		mFragFileName = fileName;
		mFragFile = localFile.string();
		loadPixelFragmentShader(localFile.string());
	}
	// init with passthru shader if something goes wrong	
	for (size_t m = mFragmentShaders.size(); m < 8; m++)
	{
		Shada newShader;
		newShader.shader = gl::GlslProg::create(loadAsset("passthru.vert"), loadAsset("passthru.frag"));
		newShader.name = "passthru.frag";
		newShader.active = true;
		mFragmentShaders.push_back(newShader);
	}
	mCurrentPreviewShader = 0;
	mCurrentRenderShader = 0;
	// Create our thread communication buffers.
	mRequests = new ConcurrentCircularBuffer<LoaderData>(100);
	mResponses = new ConcurrentCircularBuffer<LoaderData>(100);

	// Start the loading thread.
	if (!setupLoader()) {
		CI_LOG_E("Failed to create the loader thread and context.");
		
	}
	// Load our textures and transition shader in the main thread.
	try {

		mShaderTransition = gl::GlslProg::create(loadAsset("common/shadertoy.vert"), loadAsset("common/shadertoy.frag"));
	}
	catch (const std::exception& e) {
		// Quit if anything went wrong.
		CI_LOG_EXCEPTION("Failed to load common shaders:", e);
		
	}

	// Tell our loading thread to load the first shader. The path is converted to LoaderData implicitly.
	mRequests->pushFront(getAssetPath("metrics.glsl"));
}
bool Shaders::setupLoader()
{
	auto ctx = gl::env()->createSharedContext(gl::context());
	if (!ctx)
		return false;

	// If succeeded, start the loading thread.
	mThreadAbort = false;
	mThread = std::make_shared<std::thread>(&Shaders::loader, this, ctx);

	return true;
}
void Shaders::setupLiveShader()
{
	if (!isLiveShaderSetup)
	{
		//load live shader
		try
		{
			//wd::watch(liveFragFile, [this](const fs::path &livePath)
			/*wd::watch(liveFragFile, static_cast < function < void(const fs::path &frag)>>([this](const fs::path & liveFragFile)
			{
			//this->loadLiveShader();

			}));*/
			isLiveShaderSetup = true;

		}
		catch (gl::GlslProgCompileExc &exc)
		{
			mError = string(exc.what());
			CI_LOG_V("unable to load/compile shader:" + string(exc.what()));
		}
		catch (const std::exception &e)
		{

			mError = string(e.what());
			CI_LOG_V("unable to load shader:" + string(e.what()));
		}
		if (liveError)
		{
			mParameterBag->mMsg = mError;
			mParameterBag->newMsg = true;
			// revert to mix.frag, TODO better quit if mix.frag does not exit
			fs::path mixFragFile = getAssetPath("") / "mix.frag";
			mLiveShader = gl::GlslProg::create(loadAsset("live.vert"), loadFile(mixFragFile));
		}

	}
}
string Shaders::loadLiveShader(string frag)
{
	string rtn = "";
	// Load our shader and test if it is correctly compiled
	liveError = true;
	try
	{
		mLiveShader = gl::GlslProg::create(vs.c_str(), frag.c_str());
		liveError = false;
	}
	catch (gl::GlslProgCompileExc exc)
	{
		rtn = string(exc.what());
	}
	return rtn;
}
void Shaders::shutdownLoader()
{
	// Tell the loading thread to abort, then wait for it to stop.
	mThreadAbort = true;
	if (mThread)
		mThread->join();
	// Properly destroy the buffers.
	if (mResponses) delete mResponses;
	mResponses = nullptr;
	if (mRequests) delete mRequests;
	mRequests = nullptr;
}
void Shaders::loader(gl::ContextRef ctx)
{
	// This only works if we can make the render context current.
	ctx->makeCurrent();

	// Loading loop.
	while (!mThreadAbort) {
		// Wait for a request.
		if (mRequests->isNotEmpty()) {
			// Take the request from the buffer.
			LoaderData data;
			mRequests->popBack(&data);

			// Try to load, parse and compile the shader.
			try {
				std::string vs = loadString(loadAsset("common/shadertoy.vert"));
				std::string fs = loadString(loadAsset("shadertoy.inc")) + loadString(loadFile(data.path));

				data.shader = gl::GlslProg::create(gl::GlslProg::Format().vertex(vs).fragment(fs));

				// If the shader compiled successfully, pass it to the main thread.
				mResponses->pushFront(data);
			}
			catch (const std::exception& e) {
				// Uhoh, something went wrong, but it's not fatal.
 				CI_LOG_EXCEPTION("Failed to compile the shader: ", e);
			}
		}
		else {
			// Allow the CPU to do other things for a while.
			std::chrono::milliseconds duration(100);
			std::this_thread::sleep_for(duration);
		}
	}
}
void Shaders::loadFragmentShader(boost::filesystem::path aFilePath)
{
	//if (mRequests->isNotFull()) mRequests->pushFront(aFilePath);
}
void Shaders::update()
{
	LoaderData data;

	// If we are ready for the next shader, take it from the buffer.
	if (!mShaderNext && mResponses->isNotEmpty()) {
		mResponses->popBack(&data);

		mPathNext = data.path;
		mShaderNext = data.shader;



	}
	/*
	setFragString(data.shadertext);
	}*/
}
void Shaders::addRequest(boost::filesystem::path aFilePath) {
	if (mRequests->isNotFull()) {
		mRequests->pushFront(aFilePath);
	}
}
void Shaders::swapShaders() {
	mShaderCurrent = mShaderNext;
	mShaderNext.reset();

	mPathCurrent = mPathNext;
	mPathNext.clear();
}
void Shaders::random()
{
	const fs::path assets = getAssetPath("");

	// Find all *.frag files.
	std::vector<fs::path> shaders;
	for (fs::recursive_directory_iterator it(assets), end; it != end; ++it) {
		if (fs::is_regular_file(it->path()))
			if (it->path().extension() == ".glsl")
				shaders.push_back(it->path());
	}

	if (shaders.empty())
		return;

	// Load random *.frag file, but make sure it is different from the current shader.
	size_t idx = getElapsedFrames() % shaders.size();
	if (shaders.at(idx) == mPathCurrent)
		idx = (idx + 1) % shaders.size();

	if (mRequests->isNotFull())
		mRequests->pushFront(shaders.at(idx));
}
string Shaders::getFragStringFromFile(string fileName)
{
	string rtn = "";
	try
	{
		rtn = loadString(loadAsset(fileName));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V(fileName + " unable to load string from file:" + mError);
		mParameterBag->mMsg = mError;
		mParameterBag->newMsg = true;
	}
	return rtn;
}
string Shaders::getFragError()
{
	return mError;
}

Shaders::~Shaders()
{
	CI_LOG_V("Shaders destructor");
}

string Shaders::getFileName(string aFilePath)
{
	string fName;
	if (aFilePath.find_last_of("\\") != std::string::npos)
	{
		fName = aFilePath.substr(aFilePath.find_last_of("\\") + 1);
	}
	else
	{
		fName = aFilePath;
	}
	return fName;
}
string Shaders::getNewFragFileName(string aFilePath)
{
	return getFileName(aFilePath) + ".frag";
}

void Shaders::renderPreviewShader()
{
	//mShader = mPreviewShader;
	mParameterBag->controlValues[18] = mParameterBag->controlValues[21];
	mFragmentShaders[mCurrentRenderShader] = mFragmentShaders[mCurrentPreviewShader];
}
void Shaders::removePixelFragmentShaderAtIndex(int index)
{
	mFragmentShaders[index].shader = mPassThruShader;
	mFragmentShaders[index].name = "passthru";
	mFragmentShaders[index].active = false;

}
int Shaders::loadPixelFragmentShaderAtIndex(string aFilePath, int index)
{
	int rtn = -1;
	// reset 
	mParameterBag->iFade = false;
	mParameterBag->controlValues[22] = 1.0f;
	if (index > mFragmentShaders.size() - 1)
	{
		// search inactive shader
		// default to the last element
		index = mFragmentShaders.size() - 1;
		for (int i = 0; i < mFragmentShaders.size() - 1; i++)
		{
			if (!mFragmentShaders[i].active) index = i;
		}
	}
	try
	{
		fs::path fr = aFilePath;
		string name = "unknown";
		string mFile = fr.string();
		if (mFile.find_last_of("\\") != std::string::npos) name = mFile.substr(mFile.find_last_of("\\") + 1);
		mFragFileName = name;
		if (fs::exists(fr))
		{
			validFrag = false;
			std::string fs = shaderInclude + loadString(loadFile(aFilePath));
			rtn = setGLSLStringAtIndex(fs, name, index);
			if (rtn > -1)
			{
				mParameterBag->mMsg = name + " loadPixelFragmentShaderAtIndex success";
				mParameterBag->newMsg = true;
			}
		}
		else
		{
			CI_LOG_V(mFragFile + " does not exist:" + aFilePath);
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V(aFilePath + " unable to load/compile shader:" + mError);
		mParameterBag->mMsg = mError;
		mParameterBag->newMsg = true;
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V(aFilePath + " unable to load shader:" + mError);
		mParameterBag->mMsg = mError;
		mParameterBag->newMsg = true;
	}

	return rtn;
}
int Shaders::loadPixelFragmentShader(string aFilePath)
{
	int rtn = -1;
	// reset 
	mParameterBag->iFade = false;
	mParameterBag->controlValues[22] = 1.0f;
	try
	{
		fs::path fr = aFilePath;
		string name = "unknown";
		string mFile = fr.string();
		if (mFile.find_last_of("\\") != std::string::npos) name = mFile.substr(mFile.find_last_of("\\") + 1);
		mFragFileName = name;
		if (fs::exists(fr))
		{
			validFrag = false;
			std::string fs = shaderInclude + loadString(loadFile(aFilePath));
			rtn = setGLSLString(fs, name);
			if (rtn > -1)
			{
				CI_LOG_V(mFragFile + " loaded and compiled");
				mParameterBag->mMsg = name + " loadPixelFragmentShader success";
				mParameterBag->newMsg = true;
				//mFragmentShadersNames[rtn] = name;
			}
		}
		else
		{
			CI_LOG_V(mFragFile + " does not exist");
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V(aFilePath + " unable to load/compile shader err:" + mError);
		mParameterBag->mMsg = mError;
		mParameterBag->newMsg = true;
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V(aFilePath + " unable to load shader err:" + mError);
		mParameterBag->mMsg = mError;
		mParameterBag->newMsg = true;
	}

	return rtn;
}

void Shaders::loadCurrentFrag()
{
	mParameterBag->controlValues[4] = 0.0;
	try
	{
		//string dbg = currentFrag.c_str();
		mFragmentShaders[mCurrentRenderShader].shader = gl::GlslProg::create(NULL, currentFrag.c_str());
		mFragmentShaders[mCurrentRenderShader].name = "some.frag";
		mFragmentShaders[mCurrentRenderShader].active = true;

		CI_LOG_V("loadCurrentFrag success");
		mError = "";
		validFrag = true;
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		validFrag = false;
		mError = string(exc.what());
		CI_LOG_V("loadCurrentFrag error: " + mError);
	}
	// reset to no transition
	mParameterBag->iTransition = 0;
	// avoid looping or transition to run
	mParameterBag->iAnim = 1.0;
	if (mParameterBag->controlValues[18] < 0.5)
	{
		//Right
		mParameterBag->controlValues[18] = 1.0;
	}
	else
	{
		//Left
		mParameterBag->controlValues[18] = 0.0;

	}
	timeline().apply(&mParameterBag->iAnim, 1.0f, 0.1f, EaseOutCubic()).finishFn([&]{
		mParameterBag->controlValues[4] = 1.0;
	});

}

void Shaders::doTransition()
{
	if (mParameterBag->iTransition > 0)
	{
		mParameterBag->iAnim = 128.0;
		timeline().apply(&mParameterBag->iAnim, 1.0f, mParameterBag->mTransitionDuration, EaseOutCubic()).startFn([&]{ mParameterBag->controlValues[20] = 16.0; }).finishFn([&]{
			loadCurrentFrag();
		});
	}
}

int Shaders::setGLSLString(string pixelFrag, string name)
{
	int foundIndex = -1;
	currentFrag = pixelFrag;
	try
	{
		Shada newShader;
		newShader.shader = gl::GlslProg::create(passthruvert.c_str(), currentFrag.c_str());
		newShader.name = name;
		newShader.active = true;
		// searching first index of not running shader
		if (mFragmentShaders.size() < mParameterBag->MAX)
		{
			mFragmentShaders.push_back(newShader);
			foundIndex = mFragmentShaders.size() - 1;
		}
		else
		{
			bool indexFound = false;
			while (!indexFound)
			{
				foundIndex++;
				if (foundIndex != mParameterBag->mLeftFragIndex && foundIndex != mParameterBag->mRightFragIndex && foundIndex != mParameterBag->mPreviewFragIndex) indexFound = true;
				if (foundIndex > mFragmentShaders.size() - 1) indexFound = true;
			}

			// load the new shader
			mFragmentShaders[foundIndex].shader = newShader.shader;
			mFragmentShaders[foundIndex].name = name;
			mFragmentShaders[foundIndex].active = true;
		}
		//preview the new loaded shader
		mParameterBag->mPreviewFragIndex = foundIndex;
		CI_LOG_V("setGLSLString success");
		mParameterBag->mMsg = name + " setGLSLString success";
		mParameterBag->newMsg = true;
		mError = "";
		validFrag = true;
	}
	catch (gl::GlslProgCompileExc exc)
	{
		validFrag = false;
		// TODO CI_LOG_E("Problem Compiling ImGui::Renderer shader " << exc.what());
		foundIndex = -1;
		mError = string(exc.what());
		mParameterBag->mMsg = "setGLSLString file: " + name + " error:" + mError;
		mParameterBag->newMsg = true;
		CI_LOG_V(mParameterBag->mMsg);
	}
	return foundIndex;
}

void Shaders::setShaderMicroSeconds(int index, int micro)
{
	mFragmentShaders[index].microseconds = micro;
}
int Shaders::setGLSLStringAtIndex(string pixelFrag, string name, int index)
{
	int foundIndex = -1;
	currentFrag = pixelFrag;
	try
	{
		// load the new shader
		mFragmentShaders[index].shader = gl::GlslProg::create(passthruvert.c_str(), currentFrag.c_str());
		mFragmentShaders[index].name = name;
		mFragmentShaders[index].active = true;

		foundIndex = index;
		//preview the new loaded shader
		mParameterBag->mPreviewFragIndex = index;
		CI_LOG_V("setGLSLStringAtIndex success");
		mError = "";
		mParameterBag->mMsg = name + " setGLSLStringAtIndex success";
		mParameterBag->newMsg = true;
		validFrag = true;
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		validFrag = false;
		mError = string(exc.what());
		CI_LOG_V("setGLSLStringAtIndex error: " + mError);
		mParameterBag->mMsg = mError;
		mParameterBag->newMsg = true;
	}
	return foundIndex;
}

bool Shaders::setFragString(string pixelFrag)
{
	currentFrag = pixelFrag;
	try
	{
		if (mParameterBag->iTransition > 0)
		{
			doTransition();
		}
		else
		{
			mFragmentShaders[mCurrentPreviewShader].shader = gl::GlslProg::create(passthruvert.c_str(), currentFrag.c_str());
			mFragmentShaders[mCurrentPreviewShader].name = "some.frag";
			mFragmentShaders[mCurrentPreviewShader].active = true;

			//if (mParameterBag->mDirectRender) renderPreviewShader();// mFragmentShaders[mCurrentRenderShader] = gl::GlslProg(NULL, currentFrag.c_str());
			CI_LOG_V("setFragString success");
			mError = "";
			validFrag = true;
		}
		// save as current.frag for code editor
		/* not refreshed but ok to load before live code is enabled */
		if (mParameterBag->iDebug)
		{
			fs::path currentFile = getAssetPath("") / "shaders" / "current.frag";
			ofstream mCurrentFrag(currentFile.string(), std::ofstream::binary);
			mCurrentFrag << pixelFrag;
			mCurrentFrag.close();
			CI_LOG_V("current live editor mix saved:" + currentFile.string());

			mixFileName = previousFileName + "-" + currentFileName + ".frag";
			fs::path defaultFile = getAssetPath("") / "shaders" / "default" / mixFileName;
			ofstream mDefaultFrag(defaultFile.string(), std::ofstream::binary);
			mDefaultFrag << pixelFrag;
			mDefaultFrag.close();
			CI_LOG_V("default mix saved:" + defaultFile.string());
		}
		// if codeeditor mCodeEditor->setValue( pixelFrag );// CHECK 

	}
	catch (gl::GlslProgCompileExc &exc)
	{
		validFrag = false;
		mError = string(exc.what());
		CI_LOG_V("setFragString error: " + mError);
		mParameterBag->mMsg = mError;
		mParameterBag->newMsg = true;
	}

	return validFrag;
}


bool Shaders::loadTextFile(string aFilePath)
{
	bool success = false;
	string rtn = "";
	try
	{
		fs::path txtFile = aFilePath;
		if (fs::exists(txtFile))
		{
			DataSourceRef dsr = loadFile(txtFile);
			rtn = loadString(loadFile(txtFile));
			CI_LOG_V(aFilePath + " content:" + rtn);
		}
		success = true;
	}
	catch (const std::exception &e)
	{
		CI_LOG_V(fileName + " unable to load string from text file:" + string(e.what()));
	}
	return success;
	/* TODO
	// parse json
	if ( parseFragJson( mFile ) )
	{
	if ( mUserInterface->mCodeEditor )
	{
	string s = fragBegin + fragGlobalFunctions + fragFunctions + fragMainHeader + fragMainLines + fragEnd;
	mUserInterface->mCodeEditor->setValue( s );
	mUserInterface->mCodeEditor->write( mFile + "-" + ci::toString( (int)getAverageFps() ) + ".frag" );
	}
	*/
}
void Shaders::createThumbsFromDir(string filePath)
{
	int i = 0;
	string ext = "";
	fs::path p(filePath);
	for (fs::directory_iterator it(p); it != fs::directory_iterator(); ++it)
	{
		if (fs::is_regular_file(*it))// && mFragmentShaders.size() < mParameterBag->MAX
		{
			string fileName = it->path().filename().string();
			int dotIndex = fileName.find_last_of(".");

			if (dotIndex != std::string::npos)
			{
				ext = fileName.substr(dotIndex + 1);
				if (ext == "glsl")
				{
					try
					{
						std::string fs = shaderInclude + loadString(loadFile(it->path()));

						Shada newShader;
						newShader.shader = gl::GlslProg::create(passthruvert.c_str(), fs.c_str());
						newShader.name = fileName;
						newShader.active = true;
						mFragmentShaders.push_back(newShader);
						CI_LOG_V("createThumbsFromDir loaded and compiled " + fileName);

						/*mParameterBag->mPreviewFragIndex = mFragmentShaders.size() - 1;*/
					}
					catch (gl::GlslProgCompileExc &exc)
					{
						validFrag = false;
						mError = string(exc.what());
						CI_LOG_V("createThumbsFromDir error: " + mError + " on " + fileName);
					}
					//sequenceTextures.push_back(ci::gl::Texture(loadImage(filePath + fileName)));
				}
			}
		}
	}
}

#pragma warning(pop) // _CRT_SECURE_NO_WARNINGS
