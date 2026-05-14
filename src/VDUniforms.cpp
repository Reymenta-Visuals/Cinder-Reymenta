#include "VDUniforms.h"

using namespace videodromm;
VDUniformsRef VDUniforms::UNIFORMS;
VDUniformsRef	VDUniforms::create() {
	if (UNIFORMS.use_count() == 0) {
		UNIFORMS = VDUniformsRef(new VDUniforms());
	}
	return UNIFORMS;
}
VDUniforms::VDUniforms() {
	// render windows
	mRenderWidth = 1280;
	mRenderHeight = 720;
	// textures
	for (size_t i{ 0 }; i < 30; i++)
	{
		createSampler2DUniform("iChannel" + std::to_string(i), 400 + i, i); // TODO verify doesn't mess up type (uint!)
	}
	createSampler2DUniform("inputImage", 399, 0);// TODO verify doesn't mess up type (uint!)

	//mUniformsJson = getAssetPath("") / mAssetsPath / "uniforms.json";
	/*mUniformsJson = getAssetPath("") / "uniforms.json";
	if (fs::exists(mUniformsJson)) {
		loadUniforms(loadFile(mUniformsJson));
	}
	else {*/
	// global time in seconds
	// TODO 20200301 get rid of iTime createFloatUniform("iTime", ITIME, 0.0f); // 0
	createFloatUniform("TIME", ITIME, 0.0f); // 0
	// sliders
	// red
	createFloatUniform("iColorX", ICOLORX, 0.45f); // 1
	// green
	createFloatUniform("iColorY", ICOLORY, 0.0f); // 2
	// blue
	createFloatUniform("iColorZ", ICOLORZ, 1.0f); // 3
	// Alpha 
	createFloatUniform("iAlpha", IALPHA, 1.0f); // 4
	// red multiplier 
	createFloatUniform("iRedMultiplier", IFRX, 1.0f, 0.0f, 3.0f); // 5
	// green multiplier 
	createFloatUniform("iGreenMultiplier", IFGX, 1.0f, 0.0f, 3.0f); // 6
	// blue multiplier 
	createFloatUniform("iBlueMultiplier", IFBX, 1.0f, 0.0f, 3.0f); // 7
	// sobel gstnsmk
	createFloatUniform("iSobel", ISOBEL, 0.02f, 0.02f, 2.0f); // 8
	// factor
	createFloatUniform("iFactor", IFACTOR, 1.0f, 0.4f, 0.6f); // 9
	// vignette
	createFloatUniform("iVignette", IVIGNETTE, 0.0f); // 10

	// rotary
	// RotationSpeed
	createFloatUniform("iRotationSpeed", IROTATIONSPEED, 0.0f, -0.1f, 1.0f); // 11
	// effect
	createFloatUniform("iEffect", IEFFECT, 0.0f, -0.5f, 0.5f, true); // 12
	// smooth animation
	createFloatUniform("iSmooth", ISMOOTH, 0.005f); // 50 -> 13
	// exposure
	createFloatUniform("iExposure", IEXPOSURE, 1.0f, 0.0f, 13.0f); // 14
	// Pixelate
	createFloatUniform("iPixelate", IPIXELATE, 1.0f, 0.01f, 1.0f); // 15
	// Trixels
	createFloatUniform("iTrixels", ITRIXELS, 0.0f, 0.0f, 1.0f); // 16
	// iChromatic
	createFloatUniform("iChromatic", ICHROMATIC, 0.0f, 0.000000001f); // 17
	// iCrossfade
	createFloatUniform("iCrossfade", IXFADE, 0.27710f, 0.0f, 1.0f); //18 was 42 20231116
	// iMouseX  
	createFloatUniform("iMouseX", IMOUSEX, 0.27710f, 0.0f, 1280.0f); //19
	// iMouseY  
	createFloatUniform("iMouseY", IMOUSEY, 0.5648f, 0.0f, 800.0f); //20
	// iMouseZ  
	createFloatUniform("iMouseZ", IMOUSEZ, 0.0f, 0.0f, 1.0f); //21
	// vignette amount
	createFloatUniform("iMouseW", IMOUSEW, 0.91f, 0.0f, 1.0f); //22 20240518 default was 0.91f

	// iRenderXYX 
	createFloatUniform("iRenderXYX", IRENDERXYX, 0.0f, -1.0f, 1920.0f); // 23
	// iRenderXYY  
	createFloatUniform("iRenderXYY", IRENDERXYY, 0.0f, -1.0f, 1080.0f); // 24 
	// slitscan / matrix (or other) Param1 
	//createFloatUniform("iPixelX", IPIXELX, 0.0f, 0.00000000001f, 40.0f); // 23
	// slitscan / matrix(or other) Param2 
	//createFloatUniform("iPixelY", IPIXELY, 0.0f, 0.00000000001f, 40.0f); // 24
		// zoom
	// createFloatUniform("iZoom", IZOOM, 1.0f, 0.95f, 1.05f, true); // mapnfold 
	createFloatUniform("iZoom", IZOOM, 1.0f, 0.01f, 30.05f, true); // 25
	// Steps
	createFloatUniform("iSteps", ISTEPS, 16.0f, 1.0f, 128.0f); // 26
	// ratio
	createFloatUniform("iRatio", IRATIO, 20.0f, 0.00000000001f, 40.0f); // 27

	// Speed 
	createFloatUniform("iSpeed", ISPEED, 1.0f, 0.01f, 12.0f); // 29

	// contour
	createFloatUniform("iContour", ICONTOUR, 0.0f, 0.0f, 0.5f); // 30
	// weight mix fbo texture 0
	createFloatUniform("iWeight0", IWEIGHT0, 1.0f, 0.0f, 1.0f); // 31
	// weight texture 1
	createFloatUniform("iWeight1", IWEIGHT1, 0.0f, 0.0f, 1.0f); // 32
	// weight texture 2
	createFloatUniform("iWeight2", IWEIGHT2, 0.0f, 0.0f, 1.0f); // 33
	// weight texture 3
	createFloatUniform("iWeight3", IWEIGHT3, 0.0f, 0.0f, 1.0f); // 34
	// weight texture 4
	createFloatUniform("iWeight4", IWEIGHT4, 0.0f, 0.0f, 1.0f); // 35
	// weight texture 5
	createFloatUniform("iWeight5", IWEIGHT5, 0.0f, 0.0f, 1.0f); // 36
	// weight texture 6
	createFloatUniform("iWeight6", IWEIGHT6, 0.0f, 0.0f, 1.0f); // 37
	// weight texture 7
	createFloatUniform("iWeight7", IWEIGHT7, 0.0f, 0.0f, 1.0f); // 38
	// weight texture 8 
	createFloatUniform("iWeight8", IWEIGHT8, 0.0f, 0.0f, 1.0f); // 39
	// elapsed in bar 
	//createFloatUniform("iElapsed", IELAPSED, 0.0f); // 39
	// Audio multfactor 
	createFloatUniform("iAudioX", IAUDIOX, 1.0f, 0.01f, 7.0f); // 40 was 12

	// ISTART
	createFloatUniform("iStart", ISTART, 0.0f, 1.0f, 4200.0f); // 41		
	// delta time in seconds
	createFloatUniform("iDeltaTime", IDELTATIME, 60.0f / 160.0f); // 42 was 25 then 40

	// tempo time
	createFloatUniform("iTempoTime", ITEMPOTIME, 0.1f); //43
	// fps
	createFloatUniform("iFps", IFPS, 60.0f, 0.0f, 100.0f); //44	
	// iBpm 
	createFloatUniform("iBpm", IBPM, 166.0f, 0.000000001f, 400.0f); //45
	// vignette falloff
	createFloatUniform("iVFallOff", IVFALLOFF, 0.31f, 0.0f, 1.0f); // 46
	// hydra time
	// TODO 20211204 check createFloatUniform("time", TIME, 0.0f); // 47
	// bad tv
	createFloatUniform("iBadTv", IBADTV, 0.0f, 0.0f, 5.0f); // 48
	// iTimeFactor
	createFloatUniform("iTimeFactor", ITIMEFACTOR, 1.0f); // 49
	// DisplayMode
	createFloatUniform("iDisplayMode", IDISPLAYMODE, 7.0f, 0.0f, 9.0f); // 51
	// Max Volume Hydra
	createFloatUniform("volume0", IMAXVOLUME, 0.0f, 0.0f, 255.0f); // 52
	// iMultiplier for iZoom
	createFloatUniform("iMultiplier", IMULT, 1.0f, 0.0f, 30.0f); // 53

	// iPars
	createFloatUniform("iPar0", IPAR0, 0.01f, 0.0f, 30.0f); // 54
	createFloatUniform("iPar1", IPAR1, 0.20f, 0.0f, 30.0f); // 55
	createFloatUniform("iPar2", IPAR2, 1.0f, 0.0f, 30.0f); // 56
	createFloatUniform("iPar3", IPAR3, 1.385f, 0.0f, 30.0f); // 57
	createFloatUniform("iPar4", IPAR4, 1.817f, 0.0f, 10.0f); // 58
	createFloatUniform("iPar5", IPAR5, 1.0f, 0.0f, 3.0f); // 59

	// int
	// blend mode 
	createFloatUniform("iBlendmode", IBLENDMODE, 0.0f, 0.0f, 25.0f); // 60
	// beat 
	createFloatUniform("iBeat", IBEAT, 0.0f, 0.0f, 300.0f); // 61
	// bar 
	createFloatUniform("iBar", IBAR, 0.0f, 0.0f, 3008.0f); // 62
	// bar 
	createFloatUniform("iBarBeat", IBARBEAT, 1.0f, 1.0f, 3600.0f); // 63		
	// fbo A
	createFloatUniform("iFboA", IFBOA, 0); // 64
	// fbo B
	createFloatUniform("iFboB", IFBOB, 1); // 65
	// iOutW
	createFloatUniform("iOutW", IOUTW, mRenderWidth); // 66
	// iOutH  
	createFloatUniform("iOutH", IOUTH, mRenderHeight); // 67
	// beats per bar 
	createFloatUniform("iBeatsPerBar", IBEATSPERBAR, 4); // 69
	// iPhase 
	createFloatUniform("iPhase", IPHASE, 1.0f); // 70
	// iTrack 
	createFloatUniform("iTrack", ITRACK, 0.0f, 0.0f, 90000.0f); // 71
	// vignette amount
	createFloatUniform("iVAmount", IVAMOUNT, 0.91f, 0.0f, 1.0f); // 72
	//createVec3Uniform("iChannelResolution[0]", 63, vec3(mVDParams->getFboWidth(), mVDParams->getFboHeight(), 1.0));
	createFloatUniform("iDateX", IDATEX, 0.91f, 0.0f, 90000.0f); // 73 25 hours
	createFloatUniform("iDateY", IDATEY, 0.91f, 0.0f, 90000.0f); // 74 25 hours
	createFloatUniform("iDateZ", IDATEZ, 0.91f, 0.0f, 90000.0f); // 75 25 hours
	createFloatUniform("iDateW", IDATEW, 0.91f, 0.0f, 90000.0f); // 76 25 hours
	// start bar
	createFloatUniform("iBarStart", IBARSTART, 0.0f, 0.0f, 1200.0f); // 77		
	// CODE150 from 80 more 81 dancer 82 iron
	// background red
	createFloatUniform("iBackgroundColorX", IBACKGROUNDCOLORX, 0.56f); // 101
	// background green
	createFloatUniform("iBackgroundColorY", IBACKGROUNDCOLORY, 0.0f); // 102
	// background blue
	createFloatUniform("iBackgroundColorZ", IBACKGROUNDCOLORZ, 1.0f); // 103
	createVec3Uniform("iBackgroundColor", IBACKGROUNDCOLOR, vec3(getUniformValue(IBACKGROUNDCOLORX), getUniformValue(IBACKGROUNDCOLORY), getUniformValue(IBACKGROUNDCOLORZ))); // 401

	// vec3
	// iResolutionX (should be fbowidth?) 
	createFloatUniform("iResolutionX", IRESOLUTIONX, mRenderWidth, 320.01f, 4280.0f); // 121
	// iResolutionY (should be fboheight?)  
	createFloatUniform("iResolutionY", IRESOLUTIONY, mRenderHeight, 240.01f, 2160.0f); // 122

	// slitscan / matrix (or other) Param1 
	createFloatUniform("iPixelX", IPIXELX, 0.0f, -0.5f, 0.5f); // 124 
	// slitscan / matrix(or other) Param2 
	createFloatUniform("iPixelY", IPIXELY, 0.0f, -0.5f, 0.5f); // 125


	// -300 rule
	createVec3Uniform("iResolution", IRESOLUTION, vec3(getUniformValue(IRESOLUTIONX), getUniformValue(IRESOLUTIONY), 1.0)); // 421 
	createVec2Uniform("iRenderXY", IRENDERXY, vec2(getUniformValue(IRENDERXYX), getUniformValue(IRENDERXYY))); // 424 
	createVec3Uniform("iColor", ICOLOR, vec3(0.45, 0.0, 1.0)); // 301
	createVec4Uniform("iDate", IDATE, vec4(2021.0f, 12.0f, 1.0f, 5.0f));// 363

	// boolean
	// invert
	// glitch
	createFloatUniform("iGlitch", IGLITCH, 0.0f); // 81
	createFloatUniform("iClear", ICLEAR, 0.0f); // 82
	// toggle
	createFloatUniform("iToggle", ITOGGLE, 0.0f); // 83

	// invert
	createFloatUniform("iInvert", IINVERT, 0.0f); // 86
	// greyscale 
	createFloatUniform("iGreyScale", IGREYSCALE, 0.0f); //88
	createFloatUniform("iDebug", IDEBUG, 0.0f); // 129
	createFloatUniform("iXorY", IXORY, 0.0f); // 130
	createFloatUniform("iFlipH", IFLIPH, 0.0f); // 131
	createFloatUniform("iFlipV", IFLIPV, 0.0f); // 132
	createFloatUniform("iFlipPostH", IFLIPPOSTH, 0.0f); // 133
	createFloatUniform("iFlipPostV", IFLIPPOSTV, 0.0f); // 134

	// 119 to 124 timefactor from midithor sos
	// floats for warps
	// srcArea 
	createFloatUniform("srcXLeft", SRCXLEFT, 0.0f, 0.0f, 4280.0f); // 160
	createFloatUniform("srcXRight", SRCXRIGHT, mRenderWidth, 320.01f, 4280.0f); // 161
	createFloatUniform("srcYLeft", SRCYLEFT, 0.0f, 0.0f, 1024.0f); // 162
	createFloatUniform("srcYRight", SRCYRIGHT, mRenderHeight, 0.0f, 1024.0f); // 163
	// iFreq0  
	createFloatUniform("iFreq0", IFREQ0, 0.0f, 0.01f, 256.0f); // 140	
	// iFreq1  
	createFloatUniform("iFreq1", IFREQ1, 0.0f, 0.01f, 256.0f); // 141
	// iFreq2  
	createFloatUniform("iFreq2", IFREQ2, 0.0f, 0.01f, 256.0f); // 142
	// iFreq3  
	createFloatUniform("iFreq3", IFREQ3, 0.0f, 0.01f, 256.0f); // 143
	// iSpeedX  
	createFloatUniform("iSpeed0", ISPEED0, 0.0052f); // 170
	createFloatUniform("iSpeed1", ISPEED1, 0.0043f);
	createFloatUniform("iSpeed2", ISPEED2, 0.0045f);
	createFloatUniform("iSpeed3", ISPEED3, 0.003f);
	createFloatUniform("iSpeed4", ISPEED4, 0.0058f);
	createFloatUniform("iSpeed5", ISPEED5, 0.0035f);
	createFloatUniform("iSpeed6", ISPEED6, 0.0041f);
	createFloatUniform("iSpeed7", ISPEED7, 0.0036f);
	createFloatUniform("iSpeed8", ISPEED8, 0.005f);
	createFloatUniform("iSpeed9", ISPEED9, 0.0035f);
	createFloatUniform("iSpeed10", ISPEED10, 0.0052f);
	createFloatUniform("iSpeed11", ISPEED11, 0.0045f);
	createFloatUniform("iSpeed12", ISPEED12, 0.0052f);
	createFloatUniform("iSpeed13", ISPEED13, 0.0042f); // 183
	createFloatUniform("iSpeed14", ISPEED14, 0.0051f);
	createFloatUniform("iSpeed15", ISPEED15, 0.0065f);

	// iColumn Resolume
	createFloatUniform("iColumn1", ICOLUMN1, 0.0f);
	createFloatUniform("iColumn2", ICOLUMN2, 0.0f);
	createFloatUniform("iColumn3", ICOLUMN3, 0.0f);
	createFloatUniform("iColumn4", ICOLUMN4, 0.0f);
	createFloatUniform("iColumn5", ICOLUMN5, 0.0f);
	createFloatUniform("iColumn6", ICOLUMN6, 0.0f);
	createFloatUniform("iColumn7", ICOLUMN7, 0.0f);
	createFloatUniform("iColumn8", ICOLUMN8, 0.0f);

	// vec2
	createVec2Uniform("resolution", RESOLUTION, vec2(1280.0f, 720.0f)); // hydra 150
	createVec2Uniform("RENDERSIZE", RENDERSIZE, vec2(getUniformValue(IRESOLUTIONX), getUniformValue(IRESOLUTIONY))); // isf 151
	// vec4
	createVec4Uniform("iMouse", IMOUSE, vec4(0.27710f, 0.5648f, 0.0f, 0.0f)); // 318
	// vec4 azure kinect
	createVec4Uniform("Pelvis", 200, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("SpineNavel", 201, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("SpineChest", 202, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("Neck", 203, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ClavL", 204, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ShldrL", 205, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ElbowL", 206, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("WristL", 207, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("HandL", 208, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("HandTipL", 209, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ThumbL", 210, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ClavR", 211, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ShldrR", 212, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ElbowR", 213, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("WristR", 214, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("HandR", 215, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("HandTipR", 216, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ThumbR", 217, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("HipL", 218, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("KneeL", 219, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("AnkleL", 220, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("FootL", 221, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("HipR", 222, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("KneeR", 223, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("AnkleR", 224, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("FootR", 225, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("Head", 226, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("Nose", 227, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("EyeL", 228, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("EarL", 229, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("EyeR", 230, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("EarR", 231, vec4(320.0f, 240.0f, 0.0f, 0.0f));
}
void VDUniforms::loadUniforms(const ci::DataSourceRef& source) {

	/* ci::Json json = ci::Json::parse( source->getBuffer() );

	// try to load the specified json file
	if (json.contains("uniforms")) {
		ci::Json u = json["uniforms"];

		// iterate uniforms
		for (size_t i{ 0 }; i < u.size(); i++) {
			ci::Json child = u[i];

			if (child.contains("uniform")) {
				ci::Json w = child["uniform"];
				// create uniform of the correct type
				int uniformType = w.contains("type") ? static_cast<int>(w["type"].get<int>()) : 0;
				switch (uniformType) {
				case GL_FLOAT:
					// float 5126 GL_FLOAT 0x1406
					floatFromJson(child);
					break;
				case GL_SAMPLER_2D:
					// sampler2d 35678 GL_SAMPLER_2D 0x8B5E
					sampler2dFromJson(child);
					break;
				case GL_FLOAT_VEC2:
					// vec2 35664
					vec2FromJson(child);
					break;
				case GL_FLOAT_VEC3:
					// vec3 35665
					vec3FromJson(child);
					break;
				case GL_FLOAT_VEC4:
					// vec4 35666 GL_FLOAT_VEC4
					vec4FromJson(child);
					break;
				case GL_INT:
					// int 5124 GL_INT 0x1404
					floatFromJson(child);
					break;
				case GL_BOOL:
					// boolean 35670 GL_BOOL 0x8B56
					floatFromJson(child);
					break;
				}
			}
		}
	}*/
}

/*void VDUniforms::floatFromJson(const Json::Value& json)
{
	std::string jName;
	int jCtrlIndex;
	float jValue, jMin, jMax;
	if( json.contains( "uniform" ) ) {
		ci::Json u = json["uniform"];
		jName = ( u.contains( "name" ) ) ? u["name"].get<std::string>() : "unknown";
		jCtrlIndex = ( u.contains( "index" ) ) ? u["index"].get<int>() : 249;
		jValue = ( u.contains( "value" ) ) ? u["value"].get<float>() : 0.01f;
		jMin = ( u.contains( "min" ) ) ? u["min"].get<float>() : 0.0f;
		jMax = ( u.contains( "max" ) ) ? u["max"].get<float>() : 1.0f;
		createFloatUniform(jName, jCtrlIndex, jValue, jMin, jMax);
	}
}*/
/*void VDUniforms::sampler2dFromJson(const Json::Value& json)
{
	std::string jName;
	int jCtrlIndex;
	int jTextureIndex;
	if( json.contains( "uniform" ) ) {
		ci::Json u = json["uniform"];
		jName = ( u.contains( "name" ) ) ? u["name"].get<std::string>() : "unknown";
		jCtrlIndex = ( u.contains( "index" ) ) ? u["index"].get<int>() : 250;
		jTextureIndex = ( u.contains( "textureindex" ) ) ? u["textureindex"].get<int>() : 0;
		createSampler2DUniform(jName, jCtrlIndex, jTextureIndex);
	}
}*/
/*void VDUniforms::vec2FromJson(const Json::Value& json)
{
	std::string jName;
	int jCtrlIndex;
	if( json.contains( "uniform" ) ) {
		ci::Json u = json["uniform"];
		jName = ( u.contains( "name" ) ) ? u["name"].get<std::string>() : "unknown";
		jCtrlIndex = ( u.contains( "index" ) ) ? u["index"].get<int>() : 251;
		createVec2Uniform(jName, jCtrlIndex);
	}
}*/
/*void VDUniforms::vec3FromJson(const Json::Value& json)
{
	std::string jName;
	int jCtrlIndex;
	if( json.contains( "uniform" ) ) {
		ci::Json u = json["uniform"];
		jName = ( u.contains( "name" ) ) ? u["name"].get<std::string>() : "unknown";
		jCtrlIndex = ( u.contains( "index" ) ) ? u["index"].get<int>() : 252;
		createVec3Uniform(jName, jCtrlIndex);
	}
}*/
/*void VDUniforms::vec4FromJson(const Json::Value& json)
{
	std::string jName;
	int jCtrlIndex;
	if( json.contains( "uniform" ) ) {
		ci::Json u = json["uniform"];
		jName = ( u.contains( "name" ) ) ? u["name"].get<std::string>() : "unknown";
		jCtrlIndex = ( u.contains( "index" ) ) ? u["index"].get<int>() : 253;
		createVec4Uniform(jName, jCtrlIndex);
	}
}*/

bool VDUniforms::setUniformValue(unsigned int aIndex, float aValue) {
	bool rtn = false;

	// we can't change TIME at index 0
	if (aIndex > 0) {
		if (aIndex == IBPM) {
			if (aValue > 0.0f) {
				shaderUniforms[aIndex].floatValue = aValue;
				setUniformValue(IDELTATIME, 60 / aValue);
			}
		}
		else {
			if (shaderUniforms[aIndex].floatValue != aValue) {
				if (shaderUniforms[aIndex].overrideValue) {
					shaderUniforms[aIndex].floatValue = aValue;
				}
				else {

					if ((aValue >= shaderUniforms[aIndex].minValue && aValue <= shaderUniforms[aIndex].maxValue) || shaderUniforms[aIndex].anim > 0) {
						//shaderUniforms[aIndex].floatValue = aValue;
						switch (aIndex)
						{
						case IBAR:
							//float previousBar = shaderUniforms[aIndex].floatValue;
							//mSavedBar = aValue;
							//shaderUniforms[IBAR].floatValue = mSavedBar - shaderUniforms[IBARSTART].floatValue;
							shaderUniforms[IBAR].floatValue = aValue;
							setIBarBeat();
							break;
						case IBEAT:
							shaderUniforms[IBEAT].floatValue = aValue;
							setIBarBeat();
							break;
						case ITRACK:
							shaderUniforms[ITRACK].floatValue = aValue;
							shaderUniforms[ITIME].floatValue = (float)getElapsedSeconds();
							shaderUniforms[ISTART].floatValue = shaderUniforms[ITIME].floatValue;
							shaderUniforms[IBARSTART].floatValue = shaderUniforms[IBAR].floatValue;
							shaderUniforms[IBAR].floatValue = 0.0f;
							setIBarBeat();
							// reset zoom pixelate todo: all 1 to 30
							shaderUniforms[IZOOM].floatValue = shaderUniforms[IZOOM].defaultValue;
							shaderUniforms[IPIXELATE].floatValue = shaderUniforms[IPIXELATE].defaultValue;
							break;
						case ITOGGLE:
							shaderUniforms[ITOGGLE].floatValue = aValue;
							shaderUniforms[ITIME].floatValue = (float)getElapsedSeconds();
							shaderUniforms[ISTART].floatValue = shaderUniforms[ITIME].floatValue;
							break;
						default:
							shaderUniforms[aIndex].floatValue = aValue;
							break;
						}

						rtn = true;
					}
					else {
						mErrorCode = aIndex;
						if ((aValue < shaderUniforms[aIndex].minValue && aValue > shaderUniforms[aIndex].maxValue)) {
							mErrorCode = 2;
						}
						// TODO 20211011 find bug max = 0.0
						if (shaderUniforms[aIndex].maxValue == 0.0f) {
							mErrorCode = 1;
							shaderUniforms[aIndex].maxValue == 1.0f; // put breakpoint here
						}
					}
				}
			}
		}

	}
	else {

		// no max 
		shaderUniforms[aIndex].floatValue = aValue;
	}
	return rtn;
}
int VDUniforms::stringToIndex(const std::string& key) {
	int rtn = -1;
	if (key == "iTime" || key == "uTime" || key == "time" || key == "TIME") {
		rtn = ITIME;
	}
	// 1
	else if (key == "iColorX") {
		rtn = ICOLORX;
	}
	// green 2
	else if (key == "iColorY") {
		rtn = ICOLORY;
	}
	// blue 3
	else if (key == "iColorZ") {
		rtn = ICOLORZ;
	}
	// Alpha 4 
	else if (key == "iAlpha") {
		rtn = IALPHA;
	}
	// red multiplier 5 
	else if (key == "iRedMultiplier") {
		rtn = IFRX;
	}
	// green multiplier 6 
	else if (key == "iGreenMultiplier") {
		rtn = IFGX;
	}
	// blue multiplier 7 
	else if (key == "iBlueMultiplier") {
		rtn = IFBX;
	}
	// 8
	else if (key == "iSobel") {
		rtn = ISOBEL;
	}
	// 9 
	else if (key == "iFactor") {
		rtn = IFACTOR;
	}
	// vignette 10
	else if (key == "iVignette") {
		rtn = IVIGNETTE;
	}
	// RotationSpeed 11
	else if (key == "iRotationSpeed") {
		rtn = IROTATIONSPEED;
	}
	// effect 12
	else if (key == "iEffect") {
		rtn = IEFFECT;
	}

	// rotary
	// exposure 14
	else if (key == "iExposure") {
		rtn = IEXPOSURE;
	}
	// Pixelate 15
	else if (key == "iPixelate") {
		rtn = IPIXELATE;
	}
	// Trixels 16
	else if (key == "iTrixels") {
		rtn = ITRIXELS;
	}
	// iChromatic 17
	else if (key == "iChromatic") {
		rtn = ICHROMATIC;
	}
	// iCrossfade 18
	else if (key == "iCrossfade") {
		rtn = IXFADE;
	}
	// tempo time 43
	else if (key == "iTempoTime") {
		rtn = ITEMPOTIME;
	}
	// fps 44
	else if (key == "iFps") {
		rtn = IFPS;
	}
	// iBpm 45
	else if (key == "iBpm") {
		rtn = IBPM;
	}
	// Speed  22
	else if (key == "speed") {
		rtn = ISPEED;
	}
	// iRenderXYX 23 
	else if (key == "iRenderXYX") {
		rtn = IRENDERXYX;
	}
	// iRenderXYY 24 
	else if (key == "iRenderXYY") {
		rtn = IRENDERXYY;
	}


	// Steps 26
	else if (key == "iSteps") {
		rtn = ISTEPS;
	}
	// ratio 27
	else if (key == "iRatio") {
		rtn = IRATIO;
	}
	// zoom 13
	else if (key == "iZoom") {
		rtn = IZOOM;
	}
	// contour 30
	else if (key == "iContour") {
		rtn = ICONTOUR;
	}


	// weight mix fbo texture 0 31
	else if (key == "iWeight0") {
		rtn = IWEIGHT0;
	}
	// weight texture 1 32
	else if (key == "iWeight1") {
		rtn = IWEIGHT1;
	}
	// weight texture 2 33
	else if (key == "iWeight2") {
		rtn = IWEIGHT2;
	}
	// weight texture 3 34
	else if (key == "iWeight3") {
		rtn = IWEIGHT3;
	}
	// weight texture 4 35
	else if (key == "iWeight4") {
		rtn = IWEIGHT4;
	}
	// weight texture 5 36
	else if (key == "iWeight5") {
		rtn = IWEIGHT5;
	}
	// weight texture 6 37
	else if (key == "iWeight6") {
		rtn = IWEIGHT6;
	}
	// weight texture 7 38
	else if (key == "iWeight7") {
		rtn = IWEIGHT7;
	}
	// weight texture 8  39
	else if (key == "iWeight8") {
		rtn = IWEIGHT8;
	}
	// Audio multfactor 40 
	else if (key == "iAudioX") {
		rtn = IAUDIOX;
	}
	// iStart 41 
	else if (key == "iStart") {
		rtn = ISTART;
	}
	// delta time in seconds 42
	else if (key == "iDeltaTime") {
		rtn = IDELTATIME;
	}
	// volume0 from hydra 52 
	else if (key == "volume0") {
		rtn = IMAXVOLUME;
	}
	// IMouse vec4 318
	else if (key == "iMouse") {
		rtn = IMOUSE;
	}
	// iMouseX 19  
	else if (key == "iMouseX") {
		rtn = IMOUSEX;
	}
	// iMouseY 20  
	else if (key == "iMouseY") {
		rtn = IMOUSEY;
	}
	// iMouseZ 21  
	else if (key == "iMouseZ") {
		rtn = IMOUSEZ;
	}
	// iMouseW 22
	else if (key == "iMouseW") {
		rtn = IMOUSEW;
	}
	// vignette falloff 46
	else if (key == "iVFallOff") {
		rtn = IVFALLOFF;
	}
	// bad tv 48
	else if (key == "iBadTv") {
		rtn = IBADTV;
	}
	// iTimeFactor 49
	else if (key == "iTimeFactor") {
		rtn = ITIMEFACTOR;
	}
	// IMULT 53
	else if (key == "iMult") {
		rtn = IMULT;
	}
	// smooth animation 13
	else if (key == "iSmooth") {
		rtn = ISMOOTH;
	}
	// DisplayMode 51
	else if (key == "iDisplayMode") {
		rtn = IDISPLAYMODE;
	}
	// iPar0 54
	else if (key == "iPar0") {
		rtn = IPAR0;
	}
	// iPar0 55
	else if (key == "iPar1") {
		rtn = IPAR1;
	}
	// iPar0 56
	else if (key == "iPar2") {
		rtn = IPAR2;
	}
	// iPar0 57
	else if (key == "iPar3") {
		rtn = IPAR3;
	}
	// iPar0 58
	else if (key == "iPar4") {
		rtn = IPAR4;
	}
	// iPar0 59
	else if (key == "iPar5") {
		rtn = IPAR5;
	}
	// int
	// blend mode 60 
	else if (key == "iBlendmode") {
		rtn = IBLENDMODE;
	}
	// beat 61 
	else if (key == "iBeat") {
		rtn = IBEAT;
	}
	// bar 62
	else if (key == "iBar") {
		rtn = IBAR;
	}
	// bar 63
	else if (key == "iBarBeat") {
		rtn = IBARBEAT;
	}
	// fbo A 64
	else if (key == "iFboA") {
		rtn = IFBOA;
	}
	// fbo B 65
	else if (key == "iFboB") {
		rtn = IFBOB;
	}
	// iOutW 66
	else if (key == "iOutW") {
		rtn = IOUTW;
	}
	// iOutH 67  
	else if (key == "iOutH") {
		rtn = IOUTH;
	}
	// beats per bar 69 
	else if (key == "iBeatsPerBar") {
		rtn = IBEATSPERBAR;
	}
	// start bar 77 
	else if (key == "IBarStart") {
		rtn = IBARSTART;
	}
	// 72
	else if (key == "iVAmount") {
		rtn = IVAMOUNT;
	}
	// vec3
	// background 401
	else if (key == "iBackgroundColor") {
		rtn = IBACKGROUNDCOLOR;
	}
	// 101 
	else if (key == "iBackgroundColorX") {
		rtn = IBACKGROUNDCOLORX;
	}
	// 102
	else if (key == "iBackgroundColorY") {
		rtn = IBACKGROUNDCOLORY;
	}
	// 103
	else if (key == "iBackgroundColorZ") {
		rtn = IBACKGROUNDCOLORZ;
	}
	// 421
	else if (key == "iResolution") {
		rtn = IRESOLUTION;
	}
	// iResolutionX (should be fbowidth?) 121 
	else if (key == "iResolutionX") {
		rtn = IRESOLUTIONX;
	}
	// iResolutionY (should be fboheight?) 122  
	else if (key == "iResolutionY") {
		rtn = IRESOLUTIONY;
	}
	// iRender 424
	else if (key == "iRenderXY") {
		rtn = IRENDERXY;
	}
	// slitscan / matrix (or other) Param1 124 
	else if (key == "iPixelX") {
		rtn = IPIXELX;
	}
	// slitscan / matrix(or other) Param2 125 
	else if (key == "iPixelY") {
		rtn = IPIXELY;
	}

	// IPHASE 70 
	else if (key == "iPhase") {
		rtn = IPHASE;
	}

	// ITRACK 71 
	else if (key == "iTrack") {
		rtn = ITRACK;
	}
	// 301
	else if (key == "iColor") {
		rtn = ICOLOR;
	}

	// vec4
//71
	else if (key == "iDate") {
		rtn = IDATE;
	}

	// boolean
	// invert
	// glitch 81
	else if (key == "iGlitch") {
		rtn = IGLITCH;
	}

	// 82
	else if (key == "iClear") {
		rtn = ICLEAR;
	}
	// toggle 83
	else if (key == "iToggle") {
		rtn = ITOGGLE;
	}
	// invert 86
	else if (key == "iInvert") {
		rtn = IINVERT;
	}
	// greyscale 88
	else if (key == "iGreyScale") {
		rtn = IGREYSCALE;
	}
	else if (key == "iDebug") {
		rtn = IDEBUG;
	} // 129
	else if (key == "iXorY") {
		rtn = IXORY;
	} // 130
	else if (key == "iFlipH") {
		rtn = IFLIPH;
	} // 131
	else if (key == "iFlipV") {
		rtn = IFLIPV;
	} // 132
	else if (key == "iFlipPostH") {
		rtn = IFLIPPOSTH;
	} // 133
	else if (key == "iFlipPostV") {
		rtn = IFLIPPOSTV;
	} // 134

	// 119 to 124 timefactor from midithor sos
	// floats for warps
	// srcArea 
	else if (key == "srcXLeft") {
		rtn = SRCXLEFT;
	} // 160
	else if (key == "srcXRight") {
		rtn = SRCXRIGHT;
	} // 161
	else if (key == "srcYLeft") {
		rtn = SRCYLEFT;
	} // 162
	else if (key == "srcYRight") {
		rtn = SRCYRIGHT;
	} // 163
	// iFreq0  
	else if (key == "iFreq0") {
		rtn = IFREQ0;
	} // 140	
	// iFreq1  
	else if (key == "iFreq1") {
		rtn = IFREQ1;
	} // 141
	// iFreq2  
	else if (key == "iFreq2") {
		rtn = IFREQ2;
	} // 142
	// iFreq3  
	else if (key == "iFreq3") {
		rtn = IFREQ3;
	} // 143

	// vec2
	else if (key == "resolution") {
		rtn = RESOLUTION;
	} // hydra 150
	else if (key == "RENDERSIZE") {
		rtn = RENDERSIZE;
	} // isf 151
	else if (key == "iSpeed0") {
		rtn = ISPEED0;
	}
	else if (key == "iSpeed1") {
		rtn = ISPEED1;
	}
	else if (key == "iSpeed2") {
		rtn = ISPEED2;
	}
	else if (key == "iSpeed3") {
		rtn = ISPEED3;
	}
	else if (key == "iSpeed4") {
		rtn = ISPEED4;
	}
	else if (key == "iSpeed5") {
		rtn = ISPEED5;
	}
	else if (key == "iSpeed6") {
		rtn = ISPEED6;
	}
	else if (key == "iSpeed7") {
		rtn = ISPEED7;
	}
	else if (key == "iSpeed8") {
		rtn = ISPEED8;
	}
	else if (key == "iSpeed9") {
		rtn = ISPEED9;
	}
	else if (key == "iSpeed10") {
		rtn = ISPEED10;
	}
	else if (key == "iSpeed11") {
		rtn = ISPEED11;
	}
	else if (key == "iSpeed12") {
		rtn = ISPEED12;
	}
	else if (key == "iSpeed13") {
		rtn = ISPEED13;
	}
	else if (key == "iSpeed14") {
		rtn = ISPEED14;
	}
	else if (key == "iSpeed15") {
		rtn = ISPEED15;
	}

	else if (key == "ciModelViewProjection") {
		rtn = 498; // TODO
	}
	else if (key == "inputImage") {
		rtn = 499; // TODO
	}
	if (rtn == -1) {
		// not found
		createFloatUniform(key, 500); // TODO
	}
	return rtn;
}

bool VDUniforms::isExistingUniform(const std::string& aName) {
	return stringToIndex(aName) != -1;
};

