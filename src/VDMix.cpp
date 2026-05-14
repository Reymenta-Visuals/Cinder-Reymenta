#include "VDMix.h"


//using namespace ci;
//using namespace ci::app;

namespace videodromm {

	VDMix::VDMix(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms)
	{
		// Params
		mVDParams = VDParams::create();

		CI_LOG_V("VDMix readSettings");
		// Settings
		mVDSettings = aVDSettings;
		// Animation
		mVDAnimation = aVDAnimation;
		// Uniforms
		mVDUniforms = aVDUniforms;

		mDefaultTexture = ci::gl::Texture::create(mVDParams->getFboWidth(), mVDParams->getFboHeight(), ci::gl::Texture::Format().loadTopDown(false));

		mixPath = getAssetPath("") / "mix.json";
		if (fs::exists(mixPath))
		{
			restore(mixPath);
		}
		else
		{
			// Create json file if it doesn't already exist.
			std::ofstream oStream(mixPath.string());
			oStream.close();
			save();
		}

		mMixetteTexture = ci::gl::Texture::create(mVDParams->getFboWidth(), mVDParams->getFboHeight(), ci::gl::Texture::Format().loadTopDown(false));
		// init fbo format
		fmt.setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
		fmt.setBorderColor(Color::black());
		// uncomment this to enable 4x antialiasing
		//fboFmt.setSamples( 4 );
		fboFmt.setColorTextureFormat(fmt);
		mMixetteFbo = gl::Fbo::create(mVDParams->getFboWidth(), mVDParams->getFboHeight(), fboFmt);
		fs::path mMixetteFilePath = getAssetPath("") / "mixette.glsl";
		if (!fs::exists(mMixetteFilePath)) {
			mError = mMixetteFilePath.string() + " does not exist";
			CI_LOG_V(mError);
			mVDSettings->setErrorMsg(mError);
		}

		mGlslMixette = gl::GlslProg::create(mVDParams->getDefaultVertexString(), loadString(loadFile(mMixetteFilePath)));
	} // constructor

	bool VDMix::save()
	{
		/*Json doc;

		Json settings = Json::makeArray("settings");
		settings.push_back( ci::Json( "assetspath", mAssetsPath ) );
		doc.push_back( settings );
		doc.write(writeFile(mixPath), Json::WriteOptions());*/
		return true;
	}


	void VDMix::restore(const fs::path& aFilePath)
	{
		// check to see if json file exists
		if (!fs::exists(aFilePath)) {
			return;
		}
		try {
			/* std::ifstream iStream( aFilePath.string() );
			ci::Json	  doc;
			iStream >> doc;

			if (doc.contains("settings")) {
				ci::Json settings = doc["settings"];
				if (settings.contains("assetspath"))
					mAssetsPath = settings["assetspath"].get<std::string>();
			}
			if (doc.contains("uniforms")) {
				ci::Json uniforms = doc["uniforms"];
				for (unsigned int i = 0; i < 100; i++)
				{
					std::string uniformName = mVDUniforms->getUniformName(i);
					if (uniforms.contains(uniformName)) {
						mVDUniforms->setUniformValue(i, uniforms[uniformName].get<float>());
					}
				}
			}*/


			/* 20211227 mTextureList moved to fboshader if (doc.hasChild("camera")) {
				Json settings(doc.getChild("camera"));
				if (settings.hasChild("texturename")) {
					TextureCameraRef tc(TextureCamera::create());
					mTextureList.push_back(tc);
					// init with shader, colors inverted
					Json jsonInverted;
					Json shaderInverted = ci::Json::makeArray("shader");
					shaderInverted.addChild(ci::Json("shadername", "inverted"));
					shaderInverted.pushBack(ci::Json("shadertype", "fs"));
					shaderInverted.pushBack(ci::Json("shadertext", mVDParams->getInvertedDefaultShaderFragmentString()));
					jsonInverted.addChild(shaderInverted);
					Json textureInverted = ci::Json::makeArray("texture");
					textureInverted.addChild(ci::Json("texturename", "audio"));
					textureInverted.pushBack(ci::Json("texturetype", "audio"));
					textureInverted.pushBack(ci::Json("texturemode", 0));
					jsonInverted.addChild(textureInverted);
					mMixFboShader = VDFboShader::create(mVDUniforms, mVDAnimation, jsonInverted, 0, mAssetsPath);
					mFboShaderList.push_back(mMixFboShader);
					setFboInputTexture(getFboShaderListSize() - 1, 1);
				}
			}
			if (doc.hasChild("shared")) {
				Json settings(doc.getChild("shared"));
				if (settings.hasChild("name")) {
					ts = TextureShared::create();
					mTextureList.push_back(ts);
				}
			}*/
		}
		catch (const std::exception& exc) {
			CI_LOG_W(exc.what());
		}
	}
	unsigned int VDMix::getValidFboIndex(unsigned int aFboIndex) {
		return math<int>::min(aFboIndex, (unsigned int)mFboShaderList.size() - 1);
	}

	unsigned int VDMix::createFboShaderTexture(unsigned int aFboIndex, const std::string& aFolder)
	{
		unsigned int rtn = 0;
		if (aFolder != "") mAssetsPath = aFolder;
		VDFboShaderRef fboShader = VDFboShader::create(mVDUniforms, mVDAnimation, aFboIndex, mAssetsPath);
		if (mFboShaderList.size() == 0 || aFboIndex == 0) { // 20220321 tmp
			mFboShaderList.push_back(fboShader);
			rtn = (unsigned int)mFboShaderList.size() - 1;
		}
		else if (aFboIndex <= mFboShaderList.size() - 1) {
			rtn = aFboIndex;
			mFboShaderList[rtn] = fboShader;
		}
		else {
			mFboShaderList.push_back(fboShader);
			rtn = (unsigned int)mFboShaderList.size() - 1;
		}
		return rtn;
	}
	std::vector<ci::gl::GlslProg::Uniform> VDMix::getFboShaderUniforms(unsigned int aFboShaderIndex) {
		return mFboShaderList[aFboShaderIndex]->getUniforms();
	}

	float VDMix::getUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex) {
		return mFboShaderList[aFboShaderIndex]->getUniformValueByLocation(aLocationIndex);
	};
	void VDMix::setUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex, float aValue) {
		mFboShaderList[aFboShaderIndex]->setUniformValueByLocation(aLocationIndex, aValue);
	};

	unsigned int VDMix::findAvailableIndex(unsigned int aFboShaderIndex)
	{
		unsigned int rtn = aFboShaderIndex;
		unsigned int iSecond = (unsigned int)getElapsedSeconds();
		CI_LOG_V(" mCurrentSecond " + toString(mCurrentSecond) + " getElapsedSeconds " + toString(iSecond) + " mCurrentIndex " + toString(mCurrentIndex));
		// For hydra, several shaders can be received at once
		if (aFboShaderIndex == 0) {
			if (iSecond != mCurrentSecond) {
				mCurrentSecond = iSecond;
				mCurrentIndex = 0;
			}
			else {
				mCurrentIndex++;
			}
			aFboShaderIndex = mCurrentIndex;
		}

		// init the list the first time
		if (mFboShaderList.size() == 0) {
			// create fbo
			VDFboShaderRef fboShader = VDFboShader::create(mVDUniforms, mVDAnimation, 0, mAssetsPath);
			mFboShaderList.push_back(fboShader);
			rtn = (unsigned int)mFboShaderList.size() - 1;
		}
		else {
			// change current existing fbo
			if (aFboShaderIndex < mFboShaderList.size()) {
				rtn = aFboShaderIndex;
			}
			else {
				// add to list until MAXSHADERS is reached
				if (aFboShaderIndex < MAXSHADERS) {
					// create fbo
					mFboShaderList.push_back(VDFboShader::create(mVDUniforms, mVDAnimation, mFboShaderList.size(), mAssetsPath));
					rtn = mFboShaderList.size() - 1;
				}
				else {
					// reuse existing, last one if no invalid found
					rtn = mFboShaderList.size() - 1;
					unsigned int found = -1;
					for (auto& fbo : mFboShaderList) {
						found++;
						if (!fbo->isValid()) {
							rtn = found;
							break;
						}
					}
				}
			}
		}
		return rtn;
	}

	bool VDMix::setFragmentShaderString(const string& aFragmentShaderString, const std::string& aName, unsigned int aFboShaderIndex) {
		// received from websocket, tested with hydra
		// Création du json pour le shader
		//nlohmann::json shader = nlohmann::json::array();
		//shader.push_back({ {"shadername", aName} });
		//
		//ci::Json shadertypeJson;
		//shadertypeJson["shadertype"] = "fs";
		//shader.push_back(shadertypeJson);
		//
		//shader.push_back({ {"shadertext", aFragmentShaderString} });

		// Création du json pour la texture
		//nlohmann::json texture = nlohmann::json::array();
		//texture.push_back("texture");
		//texture.push_back({ {"texturename", "audio"} });
		//texture.push_back({ {"texturetype", "audio"} });
		//
		//// Création du json global
		//nlohmann::json json;
		//json["shader"] = shader;
		//json["texture"] = texture;

		int rtn = findAvailableIndex(aFboShaderIndex);
		mFboShaderList[rtn]->setFragmentShaderString(aFragmentShaderString, aName);
		return rtn;
	}
	int VDMix::loadFragmentShader(const std::string& aFilePath, unsigned int aFboShaderIndex) {
		//ci::Json json;
		//ci::Json shader = ci::Json::array();
		//shader.push_back({ {"shadername", "todo.txt"} });
		//shader.push_back( { { "shadertype", "fs" } } );
		//shader.push_back( { { "shadertext", "todo" } } );
		//json.push_back( shader );
		//ci::Json texture = ci::Json::array();
		//texture.push_back("texture");
		//texture.push_back( { { "texturename", "audio" } } );
		//texture.push_back( { { "texturetype", "audio" } } );
		//
		//json.push_back( texture );

		// if aFboShaderIndex is out of bounds try to find invalid fbo index or create a new fbo until MAX
		int rtn = findAvailableIndex(aFboShaderIndex);

		mFboShaderList[rtn]->loadFragmentShaderFromFile(aFilePath, true);//isAudio=true for dnd glsl files
		mVDSettings->setMsg("loaded " + mFboShaderList[rtn]->getShaderName() + "\n try at " + toString(aFboShaderIndex) + " valid at " + toString(rtn));
		return rtn;
	}
	ci::gl::TextureRef VDMix::getMixetteTexture(unsigned int aFboIndex) {

		gl::ScopedFramebuffer fbScp(mMixetteFbo);
		// clear out the FBO with black
		gl::clear(Color::black());

		// nasty bug! bind to 100+f
		/* int f = 0;
		for (auto &fbo : mFboShaderList) {
			if (mFboShaderList[f]->isValid()) {// white mix bug && mVDAnimation->getUniformValue(mVDUniforms->IWEIGHT0 + f) > 0.05f) {
				//fbo->getTexture()->bind(f); not in right order
				mFboShaderList[f]->getTexture()->bind(100 + f);
			}
			f++;
		}
		gl::ScopedGlslProg prog(mGlslMixette);
		mGlslMixette->uniform("iResolution", vec3(mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONX), mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONY), 1.0));
		mGlslMixette->uniform("iBlendmode", (int)mVDUniforms->getUniformValue(mVDUniforms->IBLENDMODE));
		int i = 0;
		for (auto &fbo : mFboShaderList) {
			if (fbo->isValid()) {// white mix bug && mVDAnimation->getUniformValue(mVDUniforms->IWEIGHT0 + i) > 0.1f) {
				mGlslMixette->uniform("iChannel" + toString(i), 100 + i);
				mGlslMixette->uniform("iWeight" + toString(i), mVDUniforms->getUniformValue(mVDUniforms->IWEIGHT0 + i));
			}
			i++;
		}
		new test:*/
		/* done in next for loop:
		int f = 0;
		for (auto &fbo : mFboShaderList) {
			if (mFboShaderList[f]->isValid()) {// white mix bug && mVDAnimation->getUniformValue(mVDUniforms->IWEIGHT0 + f) > 0.05f) {
				//fbo->getTexture()->bind(f); not in right order
				mFboShaderList[f]->getTexture()->bind(100 + f);
			}
			f++;
		}*/
		gl::ScopedGlslProg prog(mGlslMixette);
		mGlslMixette->uniform("iResolution", vec3(mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONX), mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONY), 1.0));
		mGlslMixette->uniform("iBlendmode", (int)mVDUniforms->getUniformValue(mVDUniforms->IBLENDMODE));
		int i = 0;
		for (auto& fbo : mFboShaderList) {
			if (fbo->isValid()) {// white mix bug
				if (mVDUniforms->getUniformValue(mVDUniforms->IWEIGHT0 + i) > 0.01f) mFboShaderList[i]->getTexture()->bind(100 + i);
				mGlslMixette->uniform("iChannel" + toString(i), 100 + i);
				mGlslMixette->uniform("iWeight" + toString(i), mVDUniforms->getUniformValue(mVDUniforms->IWEIGHT0 + i));
			}
			i++;
		}

		gl::drawSolidRect(Rectf(0, 0, mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONX), mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONY)));
		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mMixetteFbo->getSize());
		mMixetteTexture = mMixetteFbo->getColorTexture();
		return mMixetteTexture;// mMixetteFbo->getColorTexture();
	}
#pragma region blendmodes


#pragma region textures
	void VDMix::loadImageFile(const std::string& aFile, unsigned int aFboIndex) {
		int rtn = math<int>::min(aFboIndex, mFboShaderList.size() - 1);
		fs::path texFileOrPath = aFile;
		if (fs::exists(texFileOrPath)) {

			std::string ext = "";
			int dotIndex = texFileOrPath.filename().string().find_last_of(".");
			if (dotIndex != std::string::npos)  ext = texFileOrPath.filename().string().substr(dotIndex + 1);
			if (ext == "jpg" || ext == "png") {
				// 20220321  tmp if (mFboShaderList.size() < 1) {
					// no fbos, create one
				//ci::Json json;
				//ci::Json texture = ci::Json::array();
				//	texture.push_back("texture");
				//	texture.push_back({ {"texturename", aFile} });
				//	texture.push_back({ {"texturetype", "image"} });
				//	texture.push_back({ {"texturemode", 1} });
				//	texture.push_back({ {"texturecount", 1} });
				//	json["texture"] = texture;
				//	ci::Json shader = ci::Json::array();
				//	shader.push_back({ {"shadername", "inputImage.fs"} });
				//	shader.push_back( ci::Json::object({ {"shadertype", "fs"} }) );
				//	json["shader"] = shader;
				//	createFboShaderTexture(json, aFboIndex);
					/* 20220321 tmp }
				else {
					mFboShaderList[rtn]->loadImageFile(aFile);
					// 20211227 was setInputTextureRef(mTextureList[mTextureList.size() - 1]->getTexture());
				}*/
			}
		}
	}
	void VDMix::loadVideoFile(const std::string& aFile, unsigned int aFboIndex) {
		int rtn = math<int>::min(aFboIndex, mFboShaderList.size() - 1);
		fs::path texFileOrPath = aFile;
		if (fs::exists(texFileOrPath)) {

			std::string ext = "";
			int dotIndex = texFileOrPath.filename().string().find_last_of(".");
			if (dotIndex != std::string::npos)  ext = texFileOrPath.filename().string().substr(dotIndex + 1);
			if (ext == "mp4") {
				// 20220321  tmp if (mFboShaderList.size() < 1) {
					// no fbos, create one
				//ci::Json json;
				//ci::Json texture = ci::Json::array();
				//	texture.push_back("texture");
				//	texture.push_back({ {"texturename", aFile} });
				//	texture.push_back({ {"texturetype", "video"} });
				//	texture.push_back({ {"texturemode", 3} });
				//	texture.push_back({ {"texturecount", 1} });
				//	json["texture"] = texture;
				//	ci::Json shader = ci::Json::array();
				//	shader.push_back({ {"shadername", "inputVideo.fs"} });
				//	shader.push_back( ci::Json::object({ {"shadertype", "fs"} }) );
				//	json["shader"] = shader;
				//	createFboShaderTexture(json, aFboIndex);
					/* 20220321 tmp }
				else {
					mFboShaderList[rtn]->loadImageFile(aFile);
					// 20211227 was setInputTextureRef(mTextureList[mTextureList.size() - 1]->getTexture());
				}*/
			}
		}
	}

#pragma endregion textures

} // namespace videodromm
