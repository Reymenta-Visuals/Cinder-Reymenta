#pragma once

#include "VDTypes.h"
#include "VDUIParams.h"

namespace videodromm {
	typedef std::shared_ptr<class VDParams> VDParamsRef;
	class VDParams {
	private:
		static VDParamsRef PARAMS;
		VDUIParams		uiParams;
		const float		mAspectRatio{ 0.5625 }; // ratio 4:3 (0.75) 16:9 (0.5625)
		std::string						mDefaultVertexString;
	public:
		static VDParamsRef create();
		VDParams() : uiParams{ } {}
		std::string getDefaultVertexString() const noexcept {
			return "#version 150\nuniform mat4 ciModelViewProjection; in vec4 ciPosition; in vec4 ciColor; in vec2 ciTexCoord0; out vec4 vertColor; out vec2 vertTexCoord0;"
				"void main() { vertColor = ciColor; vertTexCoord0 = ciTexCoord0; gl_Position = ciModelViewProjection * ciPosition; }";
		};
		std::string getInvertedDefaultShaderFragmentString() const noexcept {
			return "void main(void) { vec2 uv = gl_FragCoord.xy / RENDERSIZE.xy; vec3 rgb = 1.0 - IMG_NORM_PIXEL(inputImage, uv).xyz; fragColor=vec4(rgb, 1.0);}";
			// not inverted return "void main(void) { vec2 uv = gl_FragCoord.xy / RENDERSIZE.xy; vec3 rgb = IMG_NORM_PIXEL(inputImage, uv).xyz; fragColor=vec4(rgb, 1.0);}";
		};
		std::string getDefaultShaderFragmentString() const noexcept {
			//  inverted return "void main(void) { vec2 uv = gl_FragCoord.xy / RENDERSIZE.xy; vec3 rgb = 1.0 - IMG_NORM_PIXEL(inputImage, uv).xyz; fragColor=vec4(rgb, 1.0);}";
			return "void main(void) { vec2 uv = gl_FragCoord.xy / RENDERSIZE.xy; vec3 rgb = IMG_NORM_PIXEL(inputImage, uv).xyz; fragColor=vec4(rgb, 1.0);}";
		};
		unsigned getFboWidth() const noexcept {
			return uiParams.getFboWidth();
		};
		unsigned getFboHeight() const noexcept {
			return uiParams.getFboHeight();
		};
		unsigned getPreviewFboWidth() const noexcept {
			return uiParams.getPreviewFboWidth();
		};
		unsigned getPreviewFboHeight() const noexcept {
			return uiParams.getPreviewFboHeight();
		};
		unsigned getPreviewWidth() const noexcept {
			return uiParams.getPreviewWidth();
		};
		unsigned getPreviewHeight() const noexcept {
			return uiParams.getPreviewHeight();
		};
		unsigned getUIMargin() const noexcept {
			return uiParams.getUIMargin();
		};
		unsigned getUIXPosCol1() const noexcept {
			return uiParams.getUIXPosCol1();
		};
		unsigned getUIXPosCol2() const noexcept {
			return uiParams.getUIXPosCol2();
		};
		unsigned getUIXPosCol3() const noexcept {
			return uiParams.getUIXPosCol3();
		};
		unsigned getUIYPosRow1() const noexcept {
			return uiParams.getUIYPosRow1();
		};
		unsigned getUIYPosRow2() const noexcept {
			return uiParams.getUIYPosRow2();
		};
		unsigned getUIYPosRow3() const noexcept {
			return uiParams.getUIYPosRow3();
		};
		unsigned getUILargeW() const noexcept {
			return uiParams.getUILargeW();
		};
		unsigned getUILargeH() const noexcept {
			return uiParams.getUILargeH();
		};
		unsigned getUILargePreviewW() const noexcept {
			return uiParams.getUILargePreviewW();
		};
		unsigned getUILargePreviewH() const noexcept {
			return uiParams.getUILargePreviewH();
		};
		unsigned getUISmallPreviewW() const noexcept {
			return uiParams.getUISmallPreviewW();
		};
		unsigned getUISmallPreviewH() const noexcept {
			return uiParams.getUISmallPreviewH();
		};

		~VDParams()
		{

		}
	};

}