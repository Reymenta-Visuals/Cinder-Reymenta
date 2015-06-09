/**
* \file Shaders.cpp
* \author Bruce LANE
* \date 20 november 2014
*
* Manages the shaders.
*
*/
#include "Shaders.h"

using namespace Reymenta;

Shaders::Shaders(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;
	//! instanciate the logger class
	log = Logger::create("ShadersLog.txt");
	log->logTimedString("Shaders constructor");
	header = loadString(loadAsset("shadertoy.inc"));
	defaultVertexShader = loadString(loadAsset("default.vert"));
	defaultFragmentShader = loadString(loadAsset("default.glsl"));
	validFrag = false;

	string fileName;
	fs::path localFile;
	//! load mix shader
	fileName = "mix.glsl";
	localFile = getAssetPath("") / fileName;
	loadPixelFragmentShader(localFile.string());
	//! load warp shader
	fileName = "warp.glsl";
	localFile = getAssetPath("") / fileName;
	loadPixelFragmentShader(localFile.string());
	//! init some shaders
	for (size_t m = 0; m < MAX; m++)
	{
		mFragmentShaders[m].name = "default.glsl";
		if (m == 0) mFragmentShaders[m].active = true; else mFragmentShaders[m].active = false;
		mFragmentShaders[m].prog = gl::GlslProg::create(gl::GlslProg::Format().vertex(defaultVertexShader.c_str()).fragment(defaultFragmentShader.c_str()));
		/*fileName = toString(m) + ".glsl";
		localFile = getAssetPath("") / fileName;
		if (!loadPixelFragmentShader(localFile.string()))
		{
			//does not exist or compile TODO 0.glsl MUST EXIST FOR NOW
			loadPixelFragmentShader(getAssetPath("") / "0.glsl");
		}*/
	}
}

void Shaders::resize()
{
	// change iResolution
	for (auto &shader : mFragmentShaders)
	{
		auto map = shader.prog->getActiveUniformTypes();
		if (map.find("iResolution") != map.end())
		{
			shader.prog->uniform("iResolution", vec3(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 0.0f));
			//shader.prog->uniform("iResolution", vec3(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight, 0.0f));
		}
	}
	auto mixMap = mMixShader->getActiveUniformTypes();
	if (mixMap.find("iResolution") != mixMap.end())
	{
		mMixShader->uniform("iResolution", vec3(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight, 0.0f));
	}
	auto warpMap = mWarpShader->getActiveUniformTypes();
	if (warpMap.find("iResolution") != warpMap.end())
	{
		mWarpShader->uniform("iResolution", vec3(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 0.0f));
	}

}

gl::GlslProgRef Shaders::getShader(int aIndex)
{
	if (aIndex > MAX - 1) aIndex = MAX - 1;
	if (aIndex < 0) aIndex = 0;
	return mFragmentShaders[aIndex].prog;
}

bool Shaders::loadFragmentShaderString(string incompleteFs, string fileName)
{
	bool rtn = false;
	// reset 
	mParameterBag->iFade = false;
	mParameterBag->controlValues[13] = 1.0f;
	mFragFileName = fileName;
	std::string fs = header + incompleteFs;
	rtn = setGLSLString(fs, mFragFileName);
	return rtn;
}

bool Shaders::loadPixelFragmentShader(const fs::path &fragment_path)
{
	bool rtn = false;
	// reset 
	mParameterBag->iFade = false;
	mParameterBag->controlValues[13] = 1.0f;
	string mFile = fragment_path.string();
	try
	{
		if (mFile.find_last_of("\\") != std::string::npos) mFragFileName = mFile.substr(mFile.find_last_of("\\") + 1);
		if (fs::exists(fragment_path))
		{
			std::string fs = header + loadString(loadFile(fragment_path));
			rtn = setGLSLString(fs, mFragFileName);
		}
		else
		{
			log->logTimedString(mFragFileName + " loaded and compiled, does not exist:" + mFile);
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		log->logTimedString(mFile + " unable to load/compile shader:" + string(exc.what()));
	}
	catch (ci::Exception &e)
	{
		mError = string(e.what());
		log->logTimedString(mFile + " unable to load shader:" + string(e.what()));
	}
	return rtn;
}
void Shaders::update()
{
	int i;
	// get the current time with second-level accuracy
	auto now = boost::posix_time::second_clock::local_time();
	auto date = now.date();
	auto time = now.time_of_day();
	// set each uniform if it exists in the shader program
	// when compiled, only uniforms that are used remain in the program
	for (auto &shader : mFragmentShaders)
	{
		//auto map = shader.prog->getActiveUniformTypes();
		if (shader.prog->findUniform("iChannelResolution", &i))	shader.prog->uniform("iChannelResolution", mParameterBag->iChannelResolution, 4);
		if (shader.prog->findUniform("iGlobalTime", &i))		shader.prog->uniform("iGlobalTime", static_cast<float>(getElapsedSeconds()));
		if (shader.prog->findUniform("iZoom", &i))				shader.prog->uniform("iZoom", mParameterBag->controlValues[13]);
		if (shader.prog->findUniform("iSteps", &i))				shader.prog->uniform("iSteps", (int)mParameterBag->controlValues[16]);
		if (shader.prog->findUniform("iDate", &i))				shader.prog->uniform("iDate", vec4(date.year(), date.month(), date.day_number(), time.total_seconds()));
		if (shader.prog->findUniform("iMouse", &i))				shader.prog->uniform("iMouse", mParameterBag->iMouse);
		if (shader.prog->findUniform("iColor", &i))				shader.prog->uniform("iColor", vec3(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3]));
		if (shader.prog->findUniform("iBackgroundColor", &i))	shader.prog->uniform("iBackgroundColor", vec3(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7]));
		if (shader.prog->findUniform("iAudio0", &i))			shader.prog->uniform("iAudio0", 1);//TODO replace with audioTextureIndex
		if (shader.prog->findUniform("iChannel0", &i))			shader.prog->uniform("iChannel0", 0);
		if (shader.prog->findUniform("iChannel1", &i))			shader.prog->uniform("iChannel1", 1);
		if (shader.prog->findUniform("iFreq0", &i))				shader.prog->uniform("iFreq0", mParameterBag->iFreqs[0]);
		if (shader.prog->findUniform("iFreq1", &i))				shader.prog->uniform("iFreq1", mParameterBag->iFreqs[1]);
		if (shader.prog->findUniform("iFreq2", &i))				shader.prog->uniform("iFreq2", mParameterBag->iFreqs[2]);
		if (shader.prog->findUniform("iFreq3", &i))				shader.prog->uniform("iFreq3", mParameterBag->iFreqs[3]);
		if (shader.prog->findUniform("iRatio", &i))				shader.prog->uniform("iRatio", mParameterBag->controlValues[11]);
		if (shader.prog->findUniform("iRenderXY", &i))			shader.prog->uniform("iRenderXY", mParameterBag->mLeftRenderXY);
		if (shader.prog->findUniform("iAlpha", &i))				shader.prog->uniform("iAlpha", mParameterBag->controlValues[4]);
		if (shader.prog->findUniform("iBlendmode", &i))			shader.prog->uniform("iBlendmode", mParameterBag->controlValues[15]);
		if (shader.prog->findUniform("iRotationSpeed", &i))		shader.prog->uniform("iRotationSpeed", mParameterBag->controlValues[19]);
		if (shader.prog->findUniform("iPixelate", &i))			shader.prog->uniform("iPixelate", mParameterBag->controlValues[18]);
		if (shader.prog->findUniform("iExposure", &i))			shader.prog->uniform("iExposure", mParameterBag->controlValues[14]);
		if (shader.prog->findUniform("iDeltaTime", &i))			shader.prog->uniform("iDeltaTime", mParameterBag->iDeltaTime);
		if (shader.prog->findUniform("iFade", &i))				shader.prog->uniform("iFade", mParameterBag->iFade);
		if (shader.prog->findUniform("iToggle", &i))			shader.prog->uniform("iToggle", mParameterBag->controlValues[46]);
		if (shader.prog->findUniform("iLight", &i))				shader.prog->uniform("iLight", mParameterBag->iLight);
		if (shader.prog->findUniform("iLightAuto", &i))			shader.prog->uniform("iLightAuto", mParameterBag->iLightAuto);
		if (shader.prog->findUniform("iGreyScale", &i))			shader.prog->uniform("iGreyScale", mParameterBag->iGreyScale);
		if (shader.prog->findUniform("iTransition", &i))		shader.prog->uniform("iTransition", mParameterBag->iTransition);
		if (shader.prog->findUniform("iAnim", &i))				shader.prog->uniform("iAnim", mParameterBag->iAnim.value());
		if (shader.prog->findUniform("iRepeat", &i))			shader.prog->uniform("iRepeat", mParameterBag->iRepeat);
		if (shader.prog->findUniform("iVignette", &i))			shader.prog->uniform("iVignette", mParameterBag->controlValues[47]);
		if (shader.prog->findUniform("iInvert", &i))			shader.prog->uniform("iInvert", mParameterBag->controlValues[48]);
		if (shader.prog->findUniform("iDebug", &i))				shader.prog->uniform("iDebug", mParameterBag->iDebug);
		if (shader.prog->findUniform("iShowFps", &i))			shader.prog->uniform("iShowFps", mParameterBag->iShowFps);
		if (shader.prog->findUniform("iFps", &i))				shader.prog->uniform("iFps", mParameterBag->iFps);
		if (shader.prog->findUniform("iTempoTime", &i))			shader.prog->uniform("iTempoTime", mParameterBag->iTempoTime);
		if (shader.prog->findUniform("iGlitch", &i))			shader.prog->uniform("iGlitch", mParameterBag->controlValues[45]);
	}
	//auto mixMap = mMixShader->getActiveUniforms();// getActiveUniformTypes();
	if (mMixShader->findUniform("iGlobalTime", &i))				mMixShader->uniform("iGlobalTime", static_cast<float>(getElapsedSeconds()));
	if (mMixShader->findUniform("iCrossfade", &i))				mMixShader->uniform("iCrossfade", mParameterBag->controlValues[15]);//TODO a crossfader for each warp
	if (mMixShader->findUniform("iAlpha", &i))					mMixShader->uniform("iAlpha", mParameterBag->controlValues[4]);
	if (mMixShader->findUniform("iChannel0", &i))				mMixShader->uniform("iChannel0", 0);
	if (mMixShader->findUniform("iChannel1", &i))				mMixShader->uniform("iChannel1", 1);
	//auto warpMap = mWarpShader->getActiveUniformTypes("iAlpha");
	if (mWarpShader->findUniform("iAlpha", &i))					mWarpShader->uniform("iAlpha", mParameterBag->controlValues[4]);
	if (mWarpShader->findUniform("iChannel0", &i))				mWarpShader->uniform("iChannel0", 0);
}
bool Shaders::setGLSLString(string pixelFrag, string fileName)
{
	int i;
	currentFrag = pixelFrag;
	Shada newShada;
	newShada.name = fileName;
	newShada.active = true;
	newShada.prog = gl::GlslProg::create(gl::GlslProg::Format().vertex(defaultVertexShader.c_str()).fragment(currentFrag.c_str()));
	try
	{
		// special treatment for mix.glsl
		if (fileName == "mix.glsl")
		{
			mMixShader = gl::GlslProg::create(gl::GlslProg::Format().vertex(defaultVertexShader.c_str()).fragment(currentFrag.c_str()));
			log->logTimedString("setGLSLString success for mMixShader ");
			if (mMixShader->findUniform("iResolution", &i)) mMixShader->uniform("iResolution", vec3(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight, 0.0f));

		}
		else if (fileName == "warp.glsl")
		{
			mWarpShader = gl::GlslProg::create(gl::GlslProg::Format().vertex(defaultVertexShader.c_str()).fragment(currentFrag.c_str()));
			log->logTimedString("setGLSLString success for mWarpShader ");
			if (mWarpShader->findUniform("iResolution", &i)) mWarpShader->uniform("iResolution", vec3(mParameterBag->mFboWidth, mParameterBag->mFboHeight, 0.0f));

		}
		else
		{
			int foundIndex = 0;
			/* returns MAX-1! for (int a = 0; a < MAX; a++)
			{
				if (!mFragmentShaders[a].active) foundIndex = a;
			}*/
			// load the new shader
			mFragmentShaders[foundIndex] = newShada;
			//preview the new loaded shader
			mParameterBag->mCurrentShadaFboIndex = foundIndex;
			// lol log->logTimedString("setGLSLString success, mFragmentShaders foundIndex " + static_cast<ostringstream*>(&(ostringstream() << foundIndex))->str());
			log->logTimedString("setGLSLString success, mFragmentShaders foundIndex " + toString(foundIndex));
			// check that uniforms exist before setting the constant uniforms
			if (mFragmentShaders[foundIndex].prog->findUniform("iResolution", &i)) mFragmentShaders[foundIndex].prog->uniform("iResolution", vec3(getWindowWidth(), getWindowHeight(), 0.0f));

		}
		mError = "";
		validFrag = true;
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		validFrag = false;
		mError = string(exc.what());
		log->logTimedString("setGLSLString error: " + mError);
	}
	return validFrag;
}

Shaders::~Shaders()
{
	log->logTimedString("Shaders destructor");
}
string Shaders::getFragError()
{
	return mError;
}
