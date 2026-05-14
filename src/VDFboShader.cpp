#include "VDFboShader.h"

using namespace videodromm;
/* hydra
uniform float time;
uniform vec2 resolution;
varying vec2 uv;
uniform sampler2D prevBuffer;
*/

VDFboShader::VDFboShader(VDUniformsRef aVDUniforms, VDAnimationRef aVDAnimation, unsigned int aFboIndex, const std::string& aAssetsPath)
	:mVDUniforms{ aVDUniforms },
	mVDAnimation{ aVDAnimation }
{
	CI_LOG_V("VDFboShader constructor");
	// Params
	mVDParams = VDParams::create();
	mAssetsPath = aAssetsPath;
	mFboIndex = aFboIndex;
	std::string shaderType = "fs";
	// hydra
	mIsHydraTex = false;
	/* 20211227 check is hydra needs init:
	for (size_t i{ 0 }; i < 4; i++)
	{
		mInputTextureList[i] = ci::gl::Texture::create(mVDParams->getFboWidth(), mVDParams->getFboHeight(), ci::gl::Texture::Format().loadTopDown(mLoadTopDown));
	}*/
	// load default fragment shader
	mShaderName = mShaderFileName = "inputImage.fs";
	mShaderFragmentString = mVDParams->getDefaultShaderFragmentString();
	shaderInclude = loadString(loadAsset("shadertoy.vd"));

	//mInputTextureIndex = 0;
	// 20211107 only if no texture ?
	setFboTextureAudioMode();
	// CI_LOG_V(json.serialize());
	/* if( json.contains( "shader" ) ) {
		ci::Json shaderJson = json["shader"];
		mShaderName = mShaderFileName = ( shaderJson.contains( "shadername" ) && shaderJson["shadername"].is_string())
	? shaderJson["shadername"].get<std::string>()
	: "inputImage.fs";
		mShaderFragmentString = ( shaderJson.contains( "shadertext" ) && shaderJson["shadertext"].is_string())
	? shaderJson["shadertext"].get<std::string>()
	: "";
		shaderType = ( json.contains( "shadertype" ) && json["shadertype"].is_string())
	? json["shadertype"].get<std::string>()
	: "fs";
	}
	if( json.contains( "texture" ) ) {

		ci::Json textureJson = json["texture"];
		createInputTexture(textureJson);
	}*/

	// init texture
	mRenderedTexture = ci::gl::Texture::create(mVDParams->getFboWidth(), mVDParams->getFboHeight(), ci::gl::Texture::Format().loadTopDown(mLoadTopDown));
	isReady = false;

	// init texture
	// init the fbo whatever happens next
	fboFmt.setColorTextureFormat(fmt);
	mFbo = gl::Fbo::create(mVDParams->getFboWidth(), mVDParams->getFboHeight(), fboFmt);
	mFboMsg = "";
	mFboError = "";
	if (mShaderFragmentString.length() > 0) {
		mValid = setFragmentShaderString(mShaderFragmentString, mShaderName);
	}
	else {
		mValid = loadFragmentShaderFromFile(mShaderName);
	}

	if (mValid) {
		CI_LOG_V("VDFbo constructor success");
	}
	else {
		mFboError = "VDFbo constructor failed";
	}
}
VDFboShader::~VDFboShader(void) {
}
unsigned int VDFboShader::createInputTexture()
{
	unsigned int rtn = 0;
	//unsigned int listIndex = 0;
	mCurrentFilename = mTextureName = "0.jpg";
	mTypestr = "UNKNOWN";
	mTextureMode = VDTextureMode::UNKNOWN;
	mTextureCount = 1;
	mPreloadTextures = false;
	msTotal = 0;
	CI_LOG_V("createInputTexture: mCurrentFilename " + toString(mTextureCount) + " mTextureCount: " + mCurrentFilename + " mPreloadTextures: " + toString(mPreloadTextures));

	switch (mTextureMode)
	{
	case VDTextureMode::AUDIO: // audio
		setFboTextureAudioMode();
		break;
	case VDTextureMode::SEQUENCE: // img seq loaded when ableton runs
		// init with number 1 then getFboTexture will load next images
		mInputTextureList[0].isValid = false; // remove audio texture
		loadNextTexture(1);
		break;
	case VDTextureMode::PARTS: // img parts
		mInputTextureList[0].isValid = false; // remove audio texture
		for (size_t i{ 0 }; i < mTextureCount; i++)
		{
			loadNextTexture(i);
		}
		break;
	case VDTextureMode::SHARED: // shared
		mInputTextureList[0].texture = mSpoutIn.receiveTexture();
		// set name for UI
		mInputTextureList[0].name = mSpoutIn.getSenderName();
		mInputTextureList[0].ms = 0;
		mInputTextureList[0].isValid = true;
		break;

	default:
		colonIndex = mTextureName.find(":");
		if (colonIndex != std::string::npos) {
			// full path
			texFileOrPath = mTextureName;
		}
		else {
			// relative path 
			texFileOrPath = getAssetPath("") / mAssetsPath / mTextureName;
		}
		mExt = "";
		dotIndex = texFileOrPath.filename().string().find_last_of(".");
		if (dotIndex != std::string::npos)  mExt = texFileOrPath.filename().string().substr(dotIndex + 1);
		// image
		if (mExt == "jpg" || mExt == "png") {
			mFboMsg = "jpg or png";
			mTextureMode = VDTextureMode::IMAGE;
			mTypestr = "image";
			loadImageFile(texFileOrPath.string(), 0);
		}
		else {
			// video
			if (mExt == "mp4") {
				bool fileExists = fs::exists(texFileOrPath);
				if (!fileExists) {
					mFboError = texFileOrPath.string() + " video does not exist, trying with parent folder";
					CI_LOG_V(mFboError);
					/*texFileOrPath = getAssetPath("") / mTextureName;
					fileExists = fs::exists(texFileOrPath);
					if (!fileExists) {
						mFboError = texFileOrPath.string() + " video does not exist in parent folder";
						CI_LOG_V(mFboError);
					}*/
				}
				if (fileExists) {
					/* 20220322 try {
						mGlslVideoTexture = gl::GlslProg::create(gl::GlslProg::Format()
							.vertex(loadAsset("video_texture.vs.glsl"))
							.fragment(loadAsset("video_texture.fs.glsl")));
					}
					catch (gl::GlslProgCompileExc ex) {
						CI_LOG_E("<< GlslProg Compile Error >>\n" << ex.what());
					}
					catch (gl::GlslProgLinkExc ex) {
						CI_LOG_E("<< GlslProg Link Error >>\n" << ex.what());
					}
					catch (gl::GlslProgExc ex) {
						CI_LOG_E("<< GlslProg Error >> " << ex.what());
					}
					catch (AssetLoadExc ex) {
						CI_LOG_E("<< Asset Load Error >> " << ex.what());
					}
					mBatchPlaneVideo = gl::Batch::create(geom::Plane().normal(vec3(0, 0, 1)), mGlslVideoTexture);
					if (mBatchPlaneVideo) {
						mBatchPlaneVideo->replaceGlslProg(mGlslVideoTexture);
					}

					for debug if (!mVideo.isStopped()) {
						mVideo.stop();
						std::string videoPath = getAssetPath("sos17avril2022.mp4").string();
	//std::string videoPath = getAssetPath("spidermoon.mov").string();
	mVideo1.loadMovie(videoPath, "Haut-parleurs (Realtek(R) Audio)");
	mVideo1.play();
	mVideo1.getPresentationEndedSignal().connect([]() {
		ci::app::console() << "Video finished playing!" << std::endl;
	});

					mCam.setPerspective(60.0f, getWindowAspectRatio(), 0.01f, 10000.0f);
					mCam.lookAt(vec3(0, 0, 500), vec3(), vec3(0, 1, 0));
					mCam.setAspectRatio(getWindowAspectRatio());
					mCamUi.setCamera(&mCam);
					mCamUi.setMouseWheelMultiplier(-mCamUi.getMouseWheelMultiplier());
					mVDUniforms->setUniformValue(mVDUniforms->IMOUSEX, 0.0f);
					mVDUniforms->setUniformValue(mVDUniforms->IMOUSEY, 0.0f);
					mVDUniforms->setUniformValue(mVDUniforms->IMOUSEZ, 0.0f);
					//mInputTextureRef = gl::Texture::create(, gl::Texture2d::Format().loadTopDown(mLoadTopDown).mipmap(true).minFilter(GL_LINEAR_MIPMAP_LINEAR));
					}*/
					/* mIsVideoLoaded = mVideo.loadMovie( texFileOrPath );
					mVideoDuration = mVideo.getDuration();
					mVideoPos = mVideo.getPosition();
					mVideo.play();*/
					mTypestr = "video";
					mCurrentFilename = mTextureName;
					mTextureMode = VDTextureMode::MOVIE;
				}
				else {
					// default to audio
					mFboMsg = "audio, video does not exist: " + mTextureName;
					setFboTextureAudioMode();
				}
			}
			else {
				// default to audio
				mFboMsg = "audio, not image or video: " + mTextureName;
				setFboTextureAudioMode();
			}
		}
		break;
	}
	int slashIndex = mTextureName.find_last_of("\\");
	(slashIndex != std::string::npos) ? mFboStatus = mTextureName.substr(slashIndex + 1) : mFboStatus = mTextureName;
	rtn = mInputTextureList.size() - 1;
	return rtn;

}
bool VDFboShader::loadFragmentShaderFromFile(const string& aFileOrPath, bool isAudio) {
	mValid = false;
	bool fileExists = true;
	if (aFileOrPath.length() > 0) {
		if (fs::exists(aFileOrPath)) {
			// it's a full path
			mFragFilePath = aFileOrPath;
		}
		else {
			// try in assets folder			
			if (!fs::exists(mFragFilePath)) {
				mFragFilePath = getAssetPath("") / mAssetsPath / aFileOrPath;
				if (!fs::exists(mFragFilePath)) {
					mFragFilePath = getAssetPath("") / aFileOrPath;
					if (!fs::exists(mFragFilePath)) {
						fileExists = false;
						mFboError = "VDFboShader file does not exist in assets root or current subfolder:" + aFileOrPath;
					}
				}
			}
		}
	}
	else {
		mFboError = "VDFboShader file empty";
	}
	if (fileExists) {
		// file exists
		mValid = loadFragmentStringFromFile();
	}
	if (isAudio && mValid) {
		setFboTextureAudioMode();
	}
	return mValid;
}
// private
bool VDFboShader::loadFragmentStringFromFile() {
	mValid = false;
	// load fragment shader
	mFileNameWithExtension = mFragFilePath.filename().string();
	CI_LOG_V("loadFragmentStringFromFile, loading " << mFileNameWithExtension);
	mValid = setFragmentShaderString(loadString(loadFile(mFragFilePath)), mFileNameWithExtension);
	CI_LOG_V(mFragFilePath.string() << " loaded and compiled");
	return mValid;
}
bool VDFboShader::setFragmentShaderString(const std::string& aFragmentShaderString, const std::string& aName) {
	std::string mOriginalFragmentString = aFragmentShaderString;
	std::string mOutputFragmentString = aFragmentShaderString;
	mFboError = "";
	mName = aName;
	mIsHydraTex = false;
	isReady = false;
	// we would like a name without extension
	if (mName.length() == 0) {
		mName = toString((int)getElapsedSeconds());
	}
	else {
		int dotIndex = mName.find_last_of(".");
		int slashIndex = mName.find_last_of("\\");

		if (dotIndex != std::string::npos && dotIndex > slashIndex) {
			mName = mName.substr(slashIndex + 1, dotIndex - slashIndex - 1);
		}
	}
	mShaderName = mName + ".fs";

	std::string mNotFoundUniformsString = "/* " + mName + "\n";

	// load fragment shader
	CI_LOG_V("setFragmentShaderString, loading " << mName);
	try
	{
		std::size_t foundUniform = mOriginalFragmentString.find("uniform ");
		if (foundUniform == std::string::npos) {
			CI_LOG_V("setFragmentShaderString, no uniforms found, we add from shadertoy.vd");
			mOutputFragmentString = "/* " + mName + " */\n" + shaderInclude + mOriginalFragmentString;
		}
		else {
			mOutputFragmentString = "/* " + mName + " */\n" + mOriginalFragmentString;
		}

		// try to compile a first time to get active mUniforms
		mShader = gl::GlslProg::create(mVDParams->getDefaultVertexString(), mOutputFragmentString);
		// update only if success
		mShaderFragmentString = mOutputFragmentString;
		mFboMsg = mName;
		mValid = true;
	}
	catch (gl::GlslProgCompileExc& exc)
	{
		mFboStatus = mName + std::string(exc.what());
		CI_LOG_V("setFragmentShaderString, unable to compile fragment shader:" << mFboError << " frag:" << mName);
	}
	catch (const std::exception& e)
	{
		mFboStatus = mName + std::string(e.what());
		CI_LOG_V("setFragmentShaderString, error on live fragment shader:" << mFboError << " frag:" << mName);
	}
	return mValid;
}
void VDFboShader::loadImageFile(const std::string& aFile, unsigned int aCurrentIndex) {
	if (!mInputTextureList[aCurrentIndex].isValid || aCurrentIndex == 0) {
		fs::path texFileOrPath = aFile;
		bool fileExists = fs::exists(texFileOrPath);
		if (fileExists) {
			// start profiling
			auto start = Clock::now();
			int slashIndex = aFile.find_last_of("\\");
			if (slashIndex != std::string::npos) {
				mCurrentFilename = aFile.substr(slashIndex + 1);
			}
			else {
				mCurrentFilename = aFile;
			}
			try
			{
				mInputTextureList[aCurrentIndex].texture = gl::Texture::create(loadImage(texFileOrPath), gl::Texture2d::Format().loadTopDown(mLoadTopDown).mipmap(true).minFilter(GL_LINEAR_MIPMAP_LINEAR));
			}
			catch (const std::exception& ex)
			{
				CI_LOG_E("<< loadImageFile >> " << mCurrentFilename << " error: " << ex.what());
			}
			mInputTextureList[aCurrentIndex].name = mCurrentFilename;
			mInputTextureList[aCurrentIndex].isValid = true;
			auto end = Clock::now();
			auto msdur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			mInputTextureList[aCurrentIndex].ms = msdur.count();
			msTotal += mInputTextureList[aCurrentIndex].ms;
			//mInputTextureListIndexes[mCurrentImageSequenceIndex] = aTexIndex;// = listIndex
			mFboMsg = mCurrentFilename = mCurrentFilename + " " + toString(mInputTextureList[aCurrentIndex].ms) + "ms";
		}
		else {
			// default to audio
			mFboMsg = "audio, jpg or png does not exist: " + mTextureName;
			setFboTextureAudioMode();
		}
	}
	else {
		// in cache
		mFboMsg = mInputTextureList[aCurrentIndex].name + " cached";
	}
}
// next in sequence
void VDFboShader::loadNextTexture(unsigned int aCurrentIndex) {
	if (mCurrentImageSequenceIndex != aCurrentIndex) {
		mCurrentImageSequenceIndex = aCurrentIndex;
		// try with jpg (space) explorer
		mCurrentFilename = mTextureName + " (" + toString(mCurrentImageSequenceIndex) + ").jpg";
		fs::path texFileOrPath = getAssetPath("") / mTextureName / mCurrentFilename;
		fileExists = fs::exists(texFileOrPath);

		if (!fileExists) {
			// try with jpg (-) photoshop
			mCurrentFilename = mTextureName + "-(" + toString(mCurrentImageSequenceIndex) + ").jpg";
			texFileOrPath = getAssetPath("") / mTextureName / mCurrentFilename;
			fileExists = fs::exists(texFileOrPath);
		}
		if (!fileExists) {
			// try with png (space) explorer
			mCurrentFilename = mTextureName + " (" + toString(mCurrentImageSequenceIndex) + ").png";
			texFileOrPath = getAssetPath("") / mTextureName / mCurrentFilename;
			fileExists = fs::exists(texFileOrPath);
		}
		if (!fileExists) {
			// try with png (-) photoshop
			mCurrentFilename = mTextureName + "-(" + toString(mCurrentImageSequenceIndex) + ").png";
			texFileOrPath = getAssetPath("") / mTextureName / mCurrentFilename;
			fileExists = fs::exists(texFileOrPath);
		}
		if (fileExists) {
			loadImageFile(texFileOrPath.string(), mCurrentImageSequenceIndex);
		}
	}

}
ci::gl::Texture2dRef VDFboShader::getFboTexture() {
	std::string uniformName;
	if (mValid) {
		switch (mTextureMode)
		{
		case VDTextureMode::AUDIO: // audio
			setFboTextureAudioMode();
			break;
		case VDTextureMode::SEQUENCE:
			// image at IBARBEAT must be loaded before bind()
			loadNextTexture((int)mVDUniforms->getUniformValue(mVDUniforms->IBARBEAT));
			if (mPreloadTextures) {
				//CI_LOG_E("IBARBEAT " << (unsigned int)mVDUniforms->getUniformValue(mVDUniforms->IBARBEAT) << " IBEAT " << (unsigned int)mVDUniforms->getUniformValue(mVDUniforms->IBEAT));
				if (mVDUniforms->getUniformValue(mVDUniforms->IBEAT) > 1) {
					// try to load next images
					if (mCacheImageIndex < mTextureCount) {
						mCacheImageIndex++;
						mFboStatus = " " + toString(mCacheImageIndex) + "/" + toString(mTextureCount);
						loadNextTexture(mCacheImageIndex);
					}
				}
				else {
					mFboStatus = toString(mCacheImageIndex) + "/" + toString(mTextureCount) + " loaded in " + toString(msTotal) + "ms";
				}
			}

			break;
		case VDTextureMode::SHARED:
			if (mInputTextureList[0].isValid) {
				mInputTextureList[0].texture = mSpoutIn.receiveTexture();
				mInputTextureList[0].name = mSpoutIn.getSenderName();
				mInputTextureList[0].isValid = true;
			}
			break;
		case VDTextureMode::MOVIE:
			// video
			mFboMsg = "video";
			//gl::pushMatrices();
			//gl::setMatrices(mCam);
			gl::ScopedViewport scopedViewport(getWindowSize());
			//gl::ScopedDepth scopedDepth(true);
			if (mIsVideoLoaded) {
				/* mVideo.update();
				mVideoPos = mVideo.getPosition();
				if (mVideo.isStopped() || mVideo.isPaused()) {
					mVideo.setPosition(0.0);
					mVideo.play();
				}
				vec2 videoSize = vec2(mVideo.getWidth(), mVideo.getHeight());*/
				/*mVDUniforms->setUniformValue(mVDUniforms->IRENDERXYX, mVideo.getWidth()*0.25);
				mVDUniforms->setUniformValue(mVDUniforms->IRENDERXYY, mVideo.getHeight()*0.25);
				mVDUniforms->setVec2UniformValueByIndex(mVDUniforms->IRENDERXY, vec2(mVideo.getWidth()*0.25, mVideo.getHeight()*0.25));*/
				/* mGlslVideoTexture->uniform("uVideoSize", videoSize);
				videoSize *= 0.5f;
				 {
					gl::ScopedColor scopedColor(Colorf::white());
					gl::ScopedModelMatrix scopedModelMatrix;

					ciWMFVideoPlayer::ScopedVideoTextureBind scopedVideoTex(mVideo, 0);
					gl::translate(vec3(mVDUniforms->getUniformValue(mVDUniforms->IMOUSEX)*100.0f, mVDUniforms->getUniformValue(mVDUniforms->IMOUSEY)*100.0f, mVDUniforms->getUniformValue(mVDUniforms->IMOUSEZ)*100.0f));

					gl::scale(vec3(videoSize, 1.0f));
					mBatchPlaneVideo->draw();
				} */
			}
			// restore matrices
			//gl::popMatrices();

			break;
		}
		gl::ScopedFramebuffer fbScp(mFbo);
		if (mVDUniforms->getUniformValue(mVDUniforms->ICLEAR)) {
			gl::clear(Color::black());
		}
		// bind texture
		switch (mTextureMode)
		{
		case VDTextureMode::MOVIE:
			// nothing
			break;
		case VDTextureMode::PARTS:
			for (size_t i{ 0 }; i < mInputTextureList.size(); i++)
			{
				if (mInputTextureList[i].texture) mInputTextureList[i].texture->bind(i);
			}
			break;

			//if (mInputTextureList[0].texture) mInputTextureList[0].texture->bind(0);
			break;
		case VDTextureMode::SEQUENCE:
			if (mInputTextureList[(unsigned int)mVDUniforms->getUniformValue(mVDUniforms->IBARBEAT)].isValid &&
				mInputTextureList[(unsigned int)mVDUniforms->getUniformValue(mVDUniforms->IBARBEAT)].texture) {
				mInputTextureList[(unsigned int)mVDUniforms->getUniformValue(mVDUniforms->IBARBEAT)].texture->bind(0);
			}
			break;
		default:
			if (mIsHydraTex) {
				mInputTextureList[0].texture->bind(253);
				for (size_t i{ 0 }; i < 4; i++)
				{
					mInputTextureList[i].texture->bind(254 + i);
				}
			}
			else {// case VDTextureMode::SHARED, AUDIO,..
				if (mInputTextureList[0].isValid && mInputTextureList[0].texture) {
					mInputTextureList[0].texture->bind(0);
				}
			}
			break;
		}

		// before setting uniforms!
		gl::ScopedGlslProg glslScope(mShader);
		int texNameEndIndex = 0;
		int channelIndex = 0;
		mUniforms = mShader->getActiveUniforms();
		for (const auto& uniform : mUniforms) {

			uniformName = uniform.getName();
			//CI_LOG_E(mShader->getLabel() + ", getShader uniform name:" + uniform.getName() + ", type:" + toString(uniform.getType()) + ", Location:" + toString(uniform.getLocation()));
			//if (mVDAnimation->isExistingUniform(name)) {
			int uniformType = uniform.getType();
			switch (uniformType)
			{

			case GL_FLOAT: // float 5126 0x1406
				if (uniformName == "TIME" || uniformName == "time") {
					mShader->uniform(uniformName, mVDUniforms->getUniformValue(mVDUniforms->ITIME));
				}
				else {
					if (mVDUniforms->isExistingUniform(uniformName)) {
						mShader->uniform(uniformName, mVDUniforms->getUniformValueByName(uniformName));
					}
					else {
						int l = uniform.getLocation();
						mShader->uniform(uniformName, mUniformValueByLocation[l]);
					}
				}
				break;
			case GL_SAMPLER_2D: // sampler2D 35678 0x8B5E
				texNameEndIndex = uniformName.find("iChannel");
				if (texNameEndIndex != std::string::npos && texNameEndIndex != -1) {
					// NASTY BUG! mShader->uniform(name, (uint32_t)(channelIndex));						
					mShader->uniform(uniformName, channelIndex);
					channelIndex++;
				}
				else {
					texNameEndIndex = uniformName.find("tex");
					if (texNameEndIndex != std::string::npos && texNameEndIndex != -1) {
						// hydra fbo
						mIsHydraTex = true;
						// 20210116 TODO 
						mShader->uniform(uniformName, 254 + channelIndex);
						/*
							osc(1,0.5,2).mult(shape(3)).out(o0)
							osc(2,0.5,2).mult(shape(4)).out(o1)
							osc(3,0.5).mult(shape(5)).out(o2)
							osc(4,0.5,2).mult(shape(6)).out(o3)
							src(o2).scale(1.05).rotate(0.1).blend(o1,0.1).blend(o3,0.1).blend(o0,0.1).out(o2)
							render(o2)
						*/
						channelIndex++;
					}
					else {
						// onezero, inputImage
						mShader->uniform(uniformName, 0);
					}
				}

				break;
			case GL_FLOAT_VEC2:// vec2 35664 0x8B50
				if (uniformName == "RENDERSIZE" || uniformName == "resolution") {
					mShader->uniform(uniformName, vec2(mVDParams->getFboWidth(), mVDParams->getFboHeight()));
				}
				else {
					mShader->uniform(uniformName, mVDUniforms->getVec2UniformValueByName(uniformName));
				}
				break;
			case GL_FLOAT_VEC3:// vec3 35665 0x8B51
				mShader->uniform(uniformName, mVDUniforms->getVec3UniformValueByName(uniformName));
				break;
			case GL_FLOAT_VEC4:// vec4 35666 0x8B52
				/*if (uniformName == "iMouse") {
					// mx my =0.0...
					mShader->uniform(uniformName, vec4(mVDUniforms->getUniformValue(mVDUniforms->IMOUSEX), mVDUniforms->getUniformValue(mVDUniforms->IMOUSEY), mVDUniforms->getUniformValue(mVDUniforms->IMOUSEZ), mVDUniforms->getUniformValue(mVDUniforms->IMOUSEW)));
				}
				else {*/
				if (uniformName == "iDate") {
					mShader->uniform(uniformName, vec4(mVDUniforms->getUniformValue(mVDUniforms->IDATEX), mVDUniforms->getUniformValue(mVDUniforms->IDATEY), mVDUniforms->getUniformValue(mVDUniforms->IDATEZ), mVDUniforms->getUniformValue(mVDUniforms->IDATEW)));
					//CI_LOG_E(mShader->getLabel() + ", getShader uniform name:" + uniform.getName() + ", IDATEX:" + toString(mVDUniforms->getUniformValue(mVDUniforms->IDATEX)) + ", IDATEY:" + toString(mVDUniforms->getUniformValue(mVDUniforms->IDATEY)) + ", IDATEZ:" + toString(mVDUniforms->getUniformValue(mVDUniforms->IDATEZ)) + ", IDATEW:" + toString(mVDUniforms->getUniformValue(mVDUniforms->IDATEW)));
				}
				else {
					mShader->uniform(uniformName, mVDUniforms->getVec4UniformValueByName(uniformName));
				}
				//}
				break;
			case GL_INT: // int 5124 0x1404
				// IBEAT 51
				// IBAR 52
				// IBARBEAT 53
				mShader->uniform(uniformName, mVDUniforms->getUniformValueByName(uniformName));
				break;
			case GL_BOOL: // boolean 35670 0x8B56
				//createBoolUniform(name, mVDAnimation->getUniformIndexForName(name), getBoolUniformValueByName(name)); // get same index as vdanimation
				mShader->uniform(uniformName, mVDUniforms->getUniformValueByName(uniformName));
				break;
			case GL_FLOAT_MAT4: // 35676 0x8B5C ciModelViewProjection
				break;
			default:
				break;
			}
		}

		gl::drawSolidRect(Rectf(0, 0, mVDParams->getFboWidth(), mVDParams->getFboHeight()));
		// 20220421 TODO use with shader, not directly
		if (mTextureMode == VDTextureMode::MOVIE)
		{
			gl::ScopedColor scopedColor(Colorf::white());
			gl::ScopedModelMatrix scopedModelMatrix;
			// ciWMFVideoPlayer::ScopedVideoTextureBind scopedVideoTex(mVideo, 0);
			// 20220421 TODO for not 720p:  gl::scale(vec3(1.0f));
			// mVideo.draw(0, 0);
		}


		mRenderedTexture = mFbo->getColorTexture();
		if (!isReady) {
			saveThumbnail(false);
			/*	std::string filename = mName + ".jpg";
				fs::path fr = getAssetPath("") / "thumbs" / filename;

				if (!fs::exists(fr)) {
					CI_LOG_V(fr.string() << " does not exist, creating");
					Surface s8(mRenderedTexture->createSource());
					writeImage(writeFile(fr), s8);
				}*/
		}
	}
	return mRenderedTexture;
}
void VDFboShader::saveThumbnail(bool overwrite) {
	std::string filename = mName + ".jpg";
	fs::path fr = getAssetPath("") / "thumbs" / filename;

	if (!fs::exists(fr) || overwrite) {
		//CI_LOG_V(fr.string() << " does not exist, creating");
		Surface s8(mRenderedTexture->createSource());
		writeImage(writeFile(fr), s8);
	}
}

ci::gl::Texture2dRef VDFboShader::getTexture() {
	if (mValid) {
		if (!isReady) {
			// render once for thumb
			getFboTexture();
			isReady = true;
		}
		getFboTexture();
	}
	return mRenderedTexture;
}
ci::gl::Texture2dRef VDFboShader::getRenderedTexture() {

	return mRenderedTexture;
}

bool									VDFboShader::isValid() {
	return mValid;
};

std::string								VDFboShader::getShaderName() {
	return mShaderName;
};

std::vector<ci::gl::GlslProg::Uniform>	VDFboShader::getUniforms() {
	return mUniforms;
};
