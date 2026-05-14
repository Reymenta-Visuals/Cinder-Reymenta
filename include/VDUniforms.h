#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
//!  json
//#include "cinder/Json.h"
//#include "jsoncpp/json.h"

using namespace ci;
using namespace ci::app;

namespace videodromm
{
	enum VDTextureMode { UNKNOWN, IMAGE, SEQUENCE, MOVIE, CAMERA, SHARED, AUDIO, STREAM, PARTS };
	enum VDDisplayMode {
		FBO0,
		FBO1,
		FBO2,
		FBO3,
		FBO4,
		FBO5,
		FX,
		POST,
		MIXETTE,
		WARP
	};
	//enum class UniformTypes { FLOAT, SAMPLER2D, VEC2, VEC3, VEC4, INT, BOOL };
	// cinder::gl::GlslProg::Uniform
	struct VDUniformStruct
	{
		std::string						name;
		int								uniformType;
		int								index;
		float							defaultValue;
		float							targetValue;
		float							floatValue;
		float							minValue;
		float							maxValue;
		bool							overrideValue;
		int								anim;
		/*bool							autotime;
		bool							automatic;
		bool							autobass;
		bool							automid;
		bool							autotreble;*/
		int								textureIndex;
		//bool							isValid;
	};

	// stores the pointer to the VDUniforms instance
	typedef std::shared_ptr<class VDUniforms> VDUniformsRef;

	class VDUniforms {
	public:
		VDUniforms();
		static VDUniformsRef				UNIFORMS;
		static VDUniformsRef				create();
		/*{
			return std::shared_ptr<VDUniforms>(new VDUniforms());
		}*/
		enum VDParameters {
			//! maximum number of fbos, shaders, textures
			MAX = 14,
			// uniform indexes
			// float
			ITIME = 0,
			ICOLORX = 1,
			ICOLORY = 2,
			ICOLORZ = 3,
			IALPHA = 4,
			IFRX = 5,
			IFGX = 6,
			IFBX = 7,
			ISOBEL = 8,
			IFACTOR = 9,
			IVIGNETTE = 10,		// was 82 A#4
			IROTATIONSPEED = 11,
			IEFFECT = 12, // was 28

			ISMOOTH = 13,
			IRATIO = 14, // was 27
			IPIXELATE = 15,
			ITRIXELS = 16,
			ICHROMATIC = 17,
			IXFADE = 18,

			IMOUSEX = 19,
			IMOUSEY = 20,
			IMOUSEZ = 21,
			IMOUSEW = 22,

			IRENDERXYX = 23,
			IRENDERXYY = 24,

			IZOOM = 25,
			ISTEPS = 26,
			IEXPOSURE = 27, // was 14

			//27 28 avail
			ISPEED = 29,
			ICONTOUR = 30,

			IWEIGHT0 = 31,
			IWEIGHT1 = 32,
			IWEIGHT2 = 33,
			IWEIGHT3 = 34,
			IWEIGHT4 = 35,
			IWEIGHT5 = 36,
			IWEIGHT6 = 37,
			IWEIGHT7 = 38,
			IWEIGHT8 = 39,

			IAUDIOX = 40,// 40 was 12
			ISTART = 41,
			IDELTATIME = 42,
			ITEMPOTIME = 43,
			IFPS = 44,
			IBPM = 45,
			IVFALLOFF = 46,
			TIME = 47,
			IBADTV = 48,
			ITIMEFACTOR = 49,
			IDISPLAYMODE = 51,
			IMAXVOLUME = 52,
			IMULT = 53,
			IPAR0 = 54,
			IPAR1 = 55,
			IPAR2 = 56,
			IPAR3 = 57,
			IPAR4 = 58,
			IPAR5 = 59,

			// int
			IBLENDMODE = 60,
			IBEAT = 61,
			IBAR = 62,
			IBARBEAT = 63,
			IFBOA = 64,
			IFBOB = 65,
			IOUTW = 66,
			IOUTH = 67,
			IBEATSPERBAR = 69,
			IPHASE = 70,
			ITRACK = 71,
			IVAMOUNT = 72,
			IDATEX = 73,
			IDATEY = 74,
			IDATEZ = 75,
			IDATEW = 76,
			IBARSTART = 77,
			// booleans 80 to 110
			IGLITCH = 81,	// A4
			ICLEAR = 82,	// 
			ITOGGLE = 83,	// B4 for sos
			IINVERT = 86,	// D5
			IGREYSCALE = 88,

			IBACKGROUNDCOLORX = 101,
			IBACKGROUNDCOLORY = 102,
			IBACKGROUNDCOLORZ = 103,

			IRESOLUTIONX = 121,
			IRESOLUTIONY = 122,
			IPIXELX = 124,
			IPIXELY = 125,

			IDEBUG = 129,
			IXORY = 130,
			IFLIPH = 131,
			IFLIPV = 132,
			IFLIPPOSTH = 133,
			IFLIPPOSTV = 134,

			/*
				IRHANDX = 110,
				IRHANDY = 111,
				IRHANDZ = 112,
				ILHANDX = 113,
				ILHANDY = 114,
				ILHANDZ = 115,*/

			IFREQ0 = 140,
			IFREQ1 = 141,
			IFREQ2 = 142,
			IFREQ3 = 143,
			// vec2
			RESOLUTION = 150, // vec2 for hydra
			RENDERSIZE = 151, // vec2 for ISF
			// srcarea
			SRCXLEFT = 160,
			SRCXRIGHT = 161,
			SRCYLEFT = 162,
			SRCYRIGHT = 163,

			ISPEED0 = 170,
			ISPEED1 = 171,
			ISPEED2 = 172,
			ISPEED3 = 173,
			ISPEED4 = 174,
			ISPEED5 = 175,
			ISPEED6 = 176,
			ISPEED7 = 177,
			ISPEED8 = 178,
			ISPEED9 = 179,
			ISPEED10 = 180,
			ISPEED11 = 181,
			ISPEED12 = 182,
			ISPEED13 = 183,
			ISPEED14 = 184,
			ISPEED15 = 185,

			ICOLUMN1 = 251, // Resolume
			ICOLUMN2 = 252,
			ICOLUMN3 = 253,
			ICOLUMN4 = 254,
			ICOLUMN5 = 255,
			ICOLUMN6 = 256,
			ICOLUMN7 = 257,
			ICOLUMN8 = 258,

			ICOLOR = 301,
			IMOUSE = 319,
			IDATE = 373,
			IBACKGROUNDCOLOR = 401,
			IRESOLUTION = 421,
			IRENDERXY = 424

		};
		bool							isExistingUniform(const std::string& aName);
		int								getUniformType(unsigned int aIndex) {
			return shaderUniforms[aIndex].uniformType;
		}
		int								getUniformTypeByName(const std::string& aName) {
			return shaderUniforms[stringToIndex(aName)].uniformType;
		}
		std::string						getUniformName(unsigned int aIndex) {
			return shaderUniforms[aIndex].name;
		}
		float							getDefaultUniformValue(unsigned int aIndex) {
			return shaderUniforms[aIndex].defaultValue;
		}
		float							getTargetUniformValue(unsigned int aIndex) {
			return shaderUniforms[aIndex].targetValue;
		}
		void							setTargetUniformValue(unsigned int aIndex, float aValue) {
			shaderUniforms[aIndex].targetValue = aValue;
		}
		int								getUniformTextureIndex(unsigned int aIndex) {
			return shaderUniforms[aIndex].textureIndex;
		}
		void							setAnim(unsigned int aCtrl, unsigned int aAnim) {
			shaderUniforms[aCtrl].anim = aAnim;
		}
		unsigned int					getAnim(unsigned int aCtrl) {
			return shaderUniforms[aCtrl].anim;
		}
		bool setUniformValue(unsigned int aIndex, float aValue);
		//void setUniformValueByName(const std::string& aName, float aValue);
		int								getUniformIndexForName(const std::string& aName) {
			return shaderUniforms[stringToIndex(aName)].index;
		};

		void setVec2UniformValueByName(const std::string& aName, vec2 aValue) {
			if (aName == "") {
				//CI_LOG_E("empty error");
			}
			else {
				shaderUniforms[stringToIndex(aName + "X")].floatValue = aValue.x;
				shaderUniforms[stringToIndex(aName + "Y")].floatValue = aValue.y;
			}
		}
		void setVec2UniformValueByIndex(unsigned int aIndex, vec2 aValue) {
			//IRES 120 IRESX 121 IRESY 122 TODO COLOR RENDERSIZE
			shaderUniforms[aIndex - 300].floatValue = aValue.x;
			shaderUniforms[aIndex - 299].floatValue = aValue.y;
		}
		void setVec3UniformValueByName(const std::string& aName, vec3 aValue) {
			if (aName != "") {
				shaderUniforms[stringToIndex(aName + "X")].floatValue = aValue.x;
				shaderUniforms[stringToIndex(aName + "Y")].floatValue = aValue.y;
				shaderUniforms[stringToIndex(aName + "Z")].floatValue = aValue.z;
			}
		}
		void setVec3UniformValueByIndex(unsigned int aIndex, vec3 aValue) {
			shaderUniforms[aIndex - 300].floatValue = aValue.x;
			shaderUniforms[aIndex - 299].floatValue = aValue.y;
			shaderUniforms[aIndex - 298].floatValue = aValue.z;
		}
		void setVec4UniformValueByName(const std::string& aName, vec4 aValue) {
			if (aName != "") {
				shaderUniforms[stringToIndex(aName + "X")].floatValue = aValue.x;
				shaderUniforms[stringToIndex(aName + "Y")].floatValue = aValue.y;
				shaderUniforms[stringToIndex(aName + "Z")].floatValue = aValue.z;
				shaderUniforms[stringToIndex(aName + "W")].floatValue = aValue.w;
			}
		}
		void setVec4UniformValueByIndex(unsigned int aIndex, vec4 aValue) {
			shaderUniforms[aIndex - 300].floatValue = aValue.x;
			shaderUniforms[aIndex - 299].floatValue = aValue.y;
			shaderUniforms[aIndex - 298].floatValue = aValue.z;
			shaderUniforms[aIndex - 297].floatValue = aValue.w;
		}
		vec2							getVec2UniformValueByName(const std::string& aName) {
			return vec2(shaderUniforms[stringToIndex(aName + "X")].floatValue,
				shaderUniforms[stringToIndex(aName + "Y")].floatValue);
		}
		vec3							getVec3UniformValueByName(const std::string& aName) {
			return vec3(shaderUniforms[stringToIndex(aName + "X")].floatValue,
				shaderUniforms[stringToIndex(aName + "Y")].floatValue,
				shaderUniforms[stringToIndex(aName + "Z")].floatValue);
		}
		vec4							getVec4UniformValueByName(const std::string& aName) {
			return vec4(shaderUniforms[stringToIndex(aName + "X")].floatValue,
				shaderUniforms[stringToIndex(aName + "Y")].floatValue,
				shaderUniforms[stringToIndex(aName + "Z")].floatValue,
				shaderUniforms[stringToIndex(aName + "W")].floatValue);
		}

		float							getMinUniformValue(unsigned int aIndex) {
			return shaderUniforms[aIndex].minValue;
		}
		float							getMaxUniformValue(unsigned int aIndex) {
			return shaderUniforms[aIndex].maxValue;
		}
		float							getMinUniformValueByName(const std::string& aName) {
			if (aName == "") {
			}
			return shaderUniforms[stringToIndex(aName)].minValue;
		}
		float							getMaxUniformValueByName(const std::string& aName) {
			if (aName == "") {
			}
			return shaderUniforms[stringToIndex(aName)].maxValue;
		}
		float							getUniformValue(unsigned int aIndex) {
			return shaderUniforms[aIndex].floatValue;
		}
		int								getUniformAnim(unsigned int aIndex) {
			return shaderUniforms[aIndex].anim;
		}

		int								getSampler2DUniformValueByName(const std::string& aName) {
			return shaderUniforms[stringToIndex(aName)].textureIndex;
		}
		float							getUniformValueByName(const std::string& aName) {
			if (aName == "") {
				return 1.0f;
			}
			else {
				return shaderUniforms[stringToIndex(aName)].floatValue;
			}
		}
		// public for hydra
		void createFloatUniform(const std::string& aName, int aCtrlIndex, float aValue = 1.0f, float aMin = 0.0f, float aMax = 1.0f, bool overrideValue = false) {
			if (aName != "") {
				shaderUniforms[aCtrlIndex].name = aName;
				shaderUniforms[aCtrlIndex].minValue = aMin;
				shaderUniforms[aCtrlIndex].maxValue = aMax;
				shaderUniforms[aCtrlIndex].defaultValue = aValue;
				shaderUniforms[aCtrlIndex].targetValue = aValue;
				shaderUniforms[aCtrlIndex].overrideValue = overrideValue;
				shaderUniforms[aCtrlIndex].anim = 0;
				shaderUniforms[aCtrlIndex].index = aCtrlIndex;
				shaderUniforms[aCtrlIndex].floatValue = aValue;
				shaderUniforms[aCtrlIndex].uniformType = GL_FLOAT;
				//shaderUniforms[aCtrlIndex].isValid = true;
			}
		}
		void createSampler2DUniform(const std::string& aName, int aCtrlIndex, int aTextureIndex = 0) {
			shaderUniforms[aCtrlIndex].name = aName;
			shaderUniforms[aCtrlIndex].textureIndex = aTextureIndex;
			shaderUniforms[aCtrlIndex].index = aCtrlIndex;
			shaderUniforms[aCtrlIndex].uniformType = GL_SAMPLER_2D;
			//shaderUniforms[aCtrlIndex].isValid = true;
		}
		int getErrorCode() {
			return mErrorCode;
		}

	private:

		std::map<int, VDUniformStruct>		shaderUniforms;
		//fs::path							mUniformsJson;
		//! read a uniforms json file 
		void								loadUniforms(const ci::DataSourceRef& source);
		int									mLastBeat = 0;

		//void								floatFromJson(const Json::Value& json);
		//void								sampler2dFromJson(const Json::Value& json);
		//void								vec2FromJson(const Json::Value& json);
		//void								vec3FromJson(const Json::Value& json);
		//void								vec4FromJson(const Json::Value& json);
		//void								intFromJson(const ci::Json& json);
		//void								boolFromJson(const ci::Json& json);
		// render windows
		int									mRenderWidth;
		int									mRenderHeight;

		void createVec2Uniform(const std::string& aName, int aCtrlIndex, vec2 aValue = vec2(0.0)) {
			shaderUniforms[aCtrlIndex].name = aName;
			shaderUniforms[aCtrlIndex].index = aCtrlIndex;
			shaderUniforms[aCtrlIndex].uniformType = GL_FLOAT_VEC2;
			//shaderUniforms[aCtrlIndex].isValid = true;
			shaderUniforms[aCtrlIndex].floatValue = aValue.x;
			//shaderUniforms[aCtrlIndex].vec2Value = aValue;
		}
		void createVec3Uniform(const std::string& aName, int aCtrlIndex, vec3 aValue = vec3(0.0)) {
			shaderUniforms[aCtrlIndex].name = aName;
			shaderUniforms[aCtrlIndex].index = aCtrlIndex;
			shaderUniforms[aCtrlIndex].uniformType = GL_FLOAT_VEC3;
			//shaderUniforms[aCtrlIndex].isValid = true;
			shaderUniforms[aCtrlIndex].floatValue = aValue.x;
			//shaderUniforms[aCtrlIndex].vec3Value = aValue;
		}
		void createVec4Uniform(const std::string& aName, int aCtrlIndex, vec4 aValue = vec4(0.0)) {
			shaderUniforms[aCtrlIndex].name = aName;
			shaderUniforms[aCtrlIndex].index = aCtrlIndex;
			shaderUniforms[aCtrlIndex].uniformType = GL_FLOAT_VEC4;
			//shaderUniforms[aCtrlIndex].isValid = true;
			shaderUniforms[aCtrlIndex].floatValue = aValue.x;
			//shaderUniforms[aCtrlIndex].vec4Value = aValue;
		}
		void setIBarBeat() {
			shaderUniforms[IBARBEAT].floatValue = (shaderUniforms[IBAR].floatValue - shaderUniforms[IBARSTART].floatValue) * 4 + shaderUniforms[IBEAT].floatValue;
		}
		int									stringToIndex(const std::string& key);
		int									mErrorCode = 0;
		//float								mSavedBar = 0.0f;
		//float								mSavedBeat = 0.0f;
	};
};
