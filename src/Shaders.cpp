#include "Shaders.h"

using namespace Reymenta;

Shaders::Shaders(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;
	// instanciate the logger class
	log = Logger::create("ShadersLog.txt");
	log->logTimedString("Shaders constructor");
	liveError = true;
	previousFileName = "0";
	currentFileName = "0";

	mCurrentPreviewShader = 0;
	mCurrentRenderShader = 0;

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
			log->logTimedString("mix.frag does not exist, should quit");

		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		log->logTimedString("unable to load/compile shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		log->logTimedString("unable to load shader:" + string(e.what()));
	}
	//load warp shader
	try
	{
		fs::path warpFragFile = getAssetPath("") / "passthru.frag";
		if (fs::exists(warpFragFile))
		{
			mWarpShader = gl::GlslProg::create(loadAsset("passthru.vert"), loadFile(warpFragFile));
		}
		else
		{
			log->logTimedString("passthru.frag does not exist, should quit");

		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		log->logTimedString("unable to load/compile shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		log->logTimedString("unable to load shader:" + string(e.what()));
	}
	//load live shader
	try
	{
		fs::path liveFragFile = getAssetPath("") / "live.frag";
		if (fs::exists(liveFragFile))
		{
			// Load our shader and test if it is correctly compiled
			try {
				mLiveShader = gl::GlslProg::create(loadAsset("passthru.vert"), loadFile(liveFragFile));
				liveError = false;
			}
			catch (gl::GlslProgCompileExc exc){
				console() << exc.what() << endl;
			}
			/*wd::watch(liveFragFile, [this](const fs::path &frag) {

				// Load our shader and test if it is correctly compiled
				try {
				mLiveShader = gl::GlslProg::create(loadAsset("passthru.vert"), frag);
				liveError = false;
				}
				catch (gl::GlslProgCompileExc exc){
				console() << exc.what() << endl;
				}
				});*/
		}
		else
		{
			log->logTimedString("live.frag does not exist:");
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		log->logTimedString("unable to load/compile shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{

		mError = string(e.what());
		log->logTimedString("unable to load shader:" + string(e.what()));
	}
	if (liveError)
	{
		// revert to mix.frag, TODO better quit if mix.frag does not exit
		fs::path mixFragFile = getAssetPath("") / "mix.frag";
		mLiveShader = gl::GlslProg::create(loadAsset("passthru.vert"), loadFile(mixFragFile));
	}
	//load VertexSphereShader shader
	try
	{
		fs::path vertexSphereFragFile = getAssetPath("") / "vertexSphere.frag";
		if (fs::exists(vertexSphereFragFile))
		{
			mVertexSphereShader = gl::GlslProg::create(loadAsset("vertexSphere.vert"), loadFile(vertexSphereFragFile));
		}
		else
		{
			log->logTimedString("vertexSphere.frag does not exist, should quit");
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		log->logTimedString("unable to load/compile shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		log->logTimedString("unable to load shader:" + string(e.what()));
	}	
	// shadertoy include
	vs = loadString(loadAsset("passthru.vert"));
	inc = loadString(loadAsset("shadertoy.inc"));

	validFrag = false;
	validVert = true;

	//fileName = "default.frag";
	fs::path localFile; //= getAssetPath("") / "shaders" / fileName;
	//loadPixelFrag(localFile.string());
	for (size_t m = 0; m < mParameterBag->MAX; m++)
	{
		fileName = toString(m) + ".glsl";
		localFile = getAssetPath("") / fileName;
		mFragFileName = fileName;
		mFragFile = localFile.string();
		mFragmentShadersNames.push_back("unknown");
		loadPixelFragmentShader(localFile.string());
	}
	// init with passthru shader if something goes wrong	
	/*for (size_t m = mFragmentShaders.size(); m < mParameterBag->MAX; m++)
	{
		mFragmentShaders.push_back(gl::GlslProg::create(loadAsset("passthru.vert"), loadAsset("passthru.frag")));
		mFragmentShadersNames.push_back("passthru.frag");
	}*/
	mCurrentPreviewShader = 0;
	mCurrentRenderShader = 0;
	// Create our thread communication buffers.
	//mRequests = new ConcurrentCircularBuffer<LoaderData>(10);
	//mResponses = new ConcurrentCircularBuffer<LoaderData>(10);

	// error on program termination, shutdownLoader() must be called
	// Start the loading thread.
	//setupLoader();
}
void Shaders::setupLoader()
{
	// If succeeded, start the loading thread.
	//mThreadAbort = false;
	//mThread = std::make_shared<std::thread>(&Shaders::loader, this);
}
void Shaders::shutdownLoader()
{
	// Tell the loading thread to abort, then wait for it to stop.
	//mThreadAbort = true;
	//mThread->join();
}
void Shaders::loader()
{
	// Loading loop.
	/*while (!mThreadAbort)
	{
		// Wait for a request.
		if (mRequests->isNotEmpty())
		{
			// Take the request from the buffer.
			LoaderData data;
			mRequests->popBack(&data);

			// Try to load, parse and compile the shader.
			try {
				//std::string vs = loadString(loadAsset("shaders/templates/passThrough2.vert"));
				std::string fs = inc + loadString(loadFile(data.path));
				//data.shader = gl::GlslProg::create(vs.c_str(), fs.c_str());
				data.shadertext = fs.c_str();

				// If the shader compiled successfully, pass it to the main thread.
				mResponses->pushFront(data);
			}
			catch (const std::exception& e) {
				// Uhoh, something went wrong.
				console() << e.what() << endl;
			}
		}
		else {
			// Allow the CPU to do other things for a while.
			std::chrono::milliseconds duration(100);
			std::this_thread::sleep_for(duration);
		}
	}*/
}
void Shaders::loadFragmentShader(boost::filesystem::path aFilePath)
{
	//if (mRequests->isNotFull()) mRequests->pushFront(aFilePath);
}
void Shaders::update()
{
	/*LoaderData data;

	// If we are ready for the next shader, take it from the buffer.
	if (mResponses->isNotEmpty()) {
		mResponses->popBack(&data);

		setFragString(data.shadertext);
	}*/
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
		log->logTimedString(fileName + " unable to load string from file:" + string(e.what()));
	}
	return rtn;
}
string Shaders::getFragError()
{
	return mError;
}

Shaders::~Shaders()
{
	log->logTimedString("Shaders destructor");
}
string Shaders::getShaderName(int aIndex)
{
	return mFragmentShadersNames[aIndex];
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
	mParameterBag->controlValues[18] = mParameterBag->controlValues[17];
	mFragmentShaders[mCurrentRenderShader] = mFragmentShaders[mCurrentPreviewShader];
}
int Shaders::loadPixelFragmentShaderAtIndex(string aFilePath, int index)
{
	int rtn = -1;
	// reset 
	mParameterBag->iFade = false;
	mParameterBag->controlValues[13] = 1.0f;
	if (index > mFragmentShaders.size() - 1) index = mFragmentShaders.size() - 1;
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
			std::string fs = inc + loadString(loadFile(aFilePath));
			rtn = setGLSLStringAtIndex(fs, index);
			if (rtn > -1)
			{
				mFragmentShadersNames[rtn] = getFileName(aFilePath);
			}
		}
		else
		{
			log->logTimedString(mFragFile + " loaded and compiled, does not exist:" + aFilePath);
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		log->logTimedString(aFilePath + " unable to load/compile shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		log->logTimedString(aFilePath + " unable to load shader:" + string(e.what()));
	}

	return rtn;
}
int Shaders::loadPixelFragmentShader(string aFilePath)
{
	int rtn = -1;
	// reset 
	mParameterBag->iFade = false;
	mParameterBag->controlValues[13] = 1.0f;
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
			std::string fs = inc + loadString(loadFile(aFilePath));
			rtn = setGLSLString(fs);
			if (rtn > -1)
			{
				mFragmentShadersNames[rtn] = getFileName(aFilePath);
			}
			log->logTimedString(mFragFile + " loaded and compiled:" );
		}
		else
		{
			log->logTimedString(mFragFile + " does not exist:" );
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		log->logTimedString(aFilePath + " unable to load/compile shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		log->logTimedString(aFilePath + " unable to load shader:" + string(e.what()));
	}

	return rtn;
}

void Shaders::loadCurrentFrag()
{
	mParameterBag->controlValues[4] = 0.0;
	try
	{
		string dbg = currentFrag.c_str();
		mFragmentShaders[mCurrentRenderShader] = gl::GlslProg::create(NULL, currentFrag.c_str());
		log->logTimedString("loadCurrentFrag success");
		mError = "";
		validFrag = true;
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		validFrag = false;
		mError = string(exc.what());
		log->logTimedString("loadCurrentFrag error: " + mError);
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
		timeline().apply(&mParameterBag->iAnim, 1.0f, mParameterBag->mTransitionDuration, EaseOutCubic()).startFn([&]{ mParameterBag->controlValues[16] = 16.0; }).finishFn([&]{
			loadCurrentFrag();
		});
	}
}

int Shaders::setGLSLString(string pixelFrag)
{
	int foundIndex = -1;
	currentFrag = pixelFrag;
	try
	{
		// searching first index of not running shader
		if (mFragmentShaders.size() < 8)
		{
			mFragmentShaders.push_back(gl::GlslProg::create(NULL, currentFrag.c_str()));
			foundIndex = mFragmentShaders.size() - 1;
		}
		else
		{
			bool indexFound = false;
			/*if (mParameterBag->mDirectRender){foundIndex = mParameterBag->mRightFragIndex;}else{*/
			while (!indexFound)
			{
				foundIndex++;
				if (foundIndex != mParameterBag->mLeftFragIndex && foundIndex != mParameterBag->mRightFragIndex && foundIndex != mParameterBag->mPreviewFragIndex) indexFound = true;
				if (foundIndex > mFragmentShaders.size() - 1) indexFound = true;
			}
			//}
			// load the new shader
			mFragmentShaders[foundIndex] = gl::GlslProg::create(NULL, currentFrag.c_str());
		}
		//preview the new loaded shader
		mParameterBag->mPreviewFragIndex = foundIndex;
		log->logTimedString("setGLSLString success");
		mError = "";
		validFrag = true;
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		validFrag = false;
		mError = string(exc.what());
		log->logTimedString("setGLSLString error: " + mError);
	}
	return foundIndex;
}
int Shaders::setGLSLStringAtIndex(string pixelFrag, int index)
{
	int foundIndex = -1;
	currentFrag = pixelFrag;
	try
	{
		// load the new shader
		mFragmentShaders[index] = gl::GlslProg::create(NULL, currentFrag.c_str());
		foundIndex = index;
		//preview the new loaded shader
		mParameterBag->mPreviewFragIndex = index;
		log->logTimedString("setGLSLString success");
		mError = "";
		validFrag = true;
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		validFrag = false;
		mError = string(exc.what());
		log->logTimedString("setGLSLString error: " + mError);
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
			mFragmentShaders[mCurrentPreviewShader] = gl::GlslProg::create(NULL, currentFrag.c_str());
			//if (mParameterBag->mDirectRender) renderPreviewShader();// mFragmentShaders[mCurrentRenderShader] = gl::GlslProg(NULL, currentFrag.c_str());
			log->logTimedString("setFragString success");
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
			log->logTimedString("current live editor mix saved:" + currentFile.string());

			mixFileName = previousFileName + "-" + currentFileName + ".frag";
			fs::path defaultFile = getAssetPath("") / "shaders" / "default" / mixFileName;
			ofstream mDefaultFrag(defaultFile.string(), std::ofstream::binary);
			mDefaultFrag << pixelFrag;
			mDefaultFrag.close();
			log->logTimedString("default mix saved:" + defaultFile.string());
		}
		// if codeeditor mCodeEditor->setValue( pixelFrag );// CHECK 

	}
	catch (gl::GlslProgCompileExc &exc)
	{
		validFrag = false;
		mError = string(exc.what());
		log->logTimedString("setFragString error: " + mError);
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
			log->logTimedString(aFilePath + " content:" + rtn);
		}
		success = true;
	}
	catch (const std::exception &e)
	{
		log->logTimedString(fileName + " unable to load string from text file:" + string(e.what()));
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
#pragma warning(pop) // _CRT_SECURE_NO_WARNINGS
