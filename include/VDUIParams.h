#pragma once

#include "VDTypes.h"

namespace videodromm {

	class VDUIParams {
	private:
		const unsigned	uiMargin{ 4 };
		// jpg / gif thumb for HeavyM ShaderLibrary
		const unsigned	previewWidth{ 120 };
		const unsigned	previewHeight{ 68 };
		const unsigned	previewFboWidth{ 70 };
		const unsigned	previewFboHeight{ 40 };//(unsigned)mPreviewFboWidth * mAspectRatio 39
		const unsigned	fboWidth{ 1280 };
		const unsigned	fboHeight{ 720 };//(unsigned)(mFboWidth * mAspectRatio)

		// imgui positions and sizes
		const unsigned	uiYPosRow1{ 4 };
		unsigned	uiLargeW = (previewFboWidth + uiMargin) * 3;
		unsigned	uiLargeH = (previewFboHeight + uiMargin) * 5;
		unsigned	uiLargePreviewW = (unsigned)(previewFboWidth * 1.7);
		unsigned	uiLargePreviewH = (unsigned)(previewHeight * 2.8);
		unsigned	uiPreviewH = previewHeight + uiMargin;
		unsigned	uiXPosCol1 = uiLargeW + uiMargin * 2;
		unsigned	uiXPosCol2 = uiXPosCol1 + uiMargin * 2 + previewFboWidth;
		unsigned	uiXPosCol3 = (unsigned)(uiXPosCol2 * 3.35 + uiMargin);
		unsigned	uiYPosRow2 = uiYPosRow1 + uiLargePreviewH + uiMargin; // details, textures
		unsigned	uiYPosRow3 = uiYPosRow2 + uiLargeH + uiLargePreviewH + 40;// 60;
		unsigned	uiYPosRow4 = uiYPosRow3 + uiLargePreviewH + uiMargin;
		unsigned	uiYPosRow5 = uiYPosRow4 + uiLargePreviewH + uiMargin;
		unsigned	uiYPosRow6 = uiYPosRow5 + uiLargePreviewH + uiMargin;
		unsigned	uiSmallH = uiYPosRow2 - uiYPosRow1 - uiMargin;
		unsigned	uiSmallPreviewW = (unsigned)(previewFboWidth * 0.6);
		unsigned	uiSmallPreviewH = (unsigned)(previewFboHeight * 0.6);

	public:
		VDUIParams(const VDUIParams&) = delete;
		VDUIParams()
		{
			/*uiLargeW = (previewFboWidth + uiMargin) * 3;
			uiLargeH = (unsigned)((previewFboHeight + uiMargin) * 5);
			uiLargePreviewW = (unsigned)(previewFboWidth * 1.7);
			uiLargePreviewH = (unsigned)(previewHeight * 2.8);
			uiPreviewH = previewHeight + uiMargin;
			uiXPosCol1 = uiLargeW + uiMargin * 2;
			uiXPosCol2 = uiXPosCol1 + uiMargin * 2 + previewFboWidth;
			uiXPosCol3 = (unsigned)(uiXPosCol2 * 3.35 + uiMargin);
			uiYPosRow2 = uiYPosRow1 + uiLargeH + uiMargin;
			uiYPosRow3 = (unsigned)(uiYPosRow2 + uiLargePreviewH * 1.7 + uiMargin);
			uiYPosRow4 = uiYPosRow3 + uiLargePreviewH + uiMargin;
			uiYPosRow5 = uiYPosRow4 + uiLargePreviewH + uiMargin;
			uiYPosRow6 = uiYPosRow5 + uiLargePreviewH + uiMargin;
			uiSmallH = uiYPosRow2 - uiYPosRow1 - uiMargin;*/
		}
		unsigned getFboWidth() const noexcept {
			return fboWidth;
		};
		unsigned getFboHeight() const noexcept {
			return fboHeight;
		};
		unsigned getPreviewFboWidth() const noexcept {
			return previewFboWidth;
		};
		unsigned getPreviewFboHeight() const noexcept {
			return previewFboHeight;
		};
		unsigned getPreviewWidth() const noexcept {
			return previewWidth;
		};
		unsigned getPreviewHeight() const noexcept {
			return previewHeight;
		};
		unsigned getUIMargin() const noexcept {
			return uiMargin;
		};
		unsigned getUIXPosCol1() const noexcept {
			return uiXPosCol1;
		};
		unsigned getUIXPosCol2() const noexcept {
			return uiXPosCol2;
		};

		unsigned getUIXPosCol3() const noexcept {
			return uiXPosCol3;
		};
		unsigned getUIYPosRow1() const noexcept {
			return uiYPosRow1;
		};
		unsigned getUIYPosRow2() const noexcept {
			return uiYPosRow2;
		};
		unsigned getUIYPosRow3() const noexcept {
			return uiYPosRow3;
		};
		unsigned getUILargeW() const noexcept {
			return uiLargeW;
		};
		unsigned getUILargeH() const noexcept {
			return uiLargeH;
		};
		unsigned getUILargePreviewW() const noexcept {
			return uiLargePreviewW;
		};
		unsigned getUILargePreviewH() const noexcept {
			return uiLargePreviewH;
		};
		unsigned getUISmallPreviewW() const noexcept {
			return uiSmallPreviewW;
		};
		unsigned getUISmallPreviewH() const noexcept {
			return uiSmallPreviewH;
		};
	};

}