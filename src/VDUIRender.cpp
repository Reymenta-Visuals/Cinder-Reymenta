#include "VDUIRender.h"

using namespace VideoDromm;

VDUIRender::VDUIRender(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;

	// contour
	minContour = getMinUniformValueByIndex(26);
	maxContour = getMaxUniformValueByIndex(26);
	iResolutionX = (int)getValueByName("iResolutionX");
	iResolutionY = (int)getValueByName("iResolutionY");

}
float VDUIRender::getValueByName(string aCtrlName) {
	return mVDSession->getFloatUniformValueByName(aCtrlName);
}
float VDUIRender::getValue(unsigned int aCtrl) {
	return mVDSession->getFloatUniformValueByIndex(aCtrl);
}
void VDUIRender::setValue(unsigned int aCtrl, float aValue) {
	mVDSession->setFloatUniformValueByIndex(aCtrl, aValue);
}
void VDUIRender::toggleAuto(unsigned int aCtrl) {
	mVDSession->toggleAuto(aCtrl);
}
void VDUIRender::toggleTempo(unsigned int aCtrl) {
	mVDSession->toggleTempo(aCtrl);
}
void VDUIRender::resetAutoAnimation(unsigned int aCtrl) {
	mVDSession->resetAutoAnimation(aCtrl);
}
float VDUIRender::getMinUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMinUniformValueByIndex(aIndex);
}
float VDUIRender::getMaxUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMaxUniformValueByIndex(aIndex);
}
void VDUIRender::Run(const char* title) {
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);

	ImGui::Begin("Render");
	{
		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		// iResolution
		ctrl = 29;
		//iResolutionX = getValueByName("iResolutionX");
		if (ImGui::DragInt("iResolutionX", &iResolutionX, 1.0f, (int)getMinUniformValueByIndex(ctrl), (int)getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, (float)iResolutionX);
		}
		ctrl = 30;
		//iResolutionY = getValueByName("iResolutionY");
		if (ImGui::DragInt("iResolutionY", &iResolutionY, 1.0f, (int)getMinUniformValueByIndex(ctrl), (int)getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, (float)iResolutionY);
		}

		// iContour
		ctrl = 26;
		if (ImGui::Button("a##contour")) { toggleAuto(ctrl); }
		ImGui::SameLine();
		if (ImGui::Button("t##contour")) { toggleTempo(ctrl); }
		ImGui::SameLine();
		if (ImGui::Button("x##contour")) { resetAutoAnimation(ctrl); }
		ImGui::SameLine();
		contour = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("contour", &contour, 0.001f, minContour, maxContour))
		{
			setValue(ctrl, contour);
		}
		ImGui::DragFloat("mincr", &minContour, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl));
		ImGui::SameLine();
		ImGui::DragFloat("maxcr", &maxContour, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl));
		// windows
		ImGui::Text("render window %dx%d", mVDSettings->mRenderWidth, mVDSettings->mRenderHeight);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.9f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.9f, 0.8f, 0.8f));

		if (ImGui::Button("Auto Layout")) {
			mVDSession->toggleAutoLayout();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Auto Layout for render window");

		// Auto Layout for render window
		if (mVDSession->isAutoLayout()) {
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 1.0f, 0.5f));
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
			// render window width
			static int rw = mVDSettings->mRenderWidth;
			if (ImGui::SliderInt("rdr w", &rw, 640, 4080))
			{
				//mVDSession->setRenderWidth(rw);
				mVDSettings->mRenderWidth = rw;
			}
			ImGui::SameLine();
			// render window height
			static int rh = mVDSettings->mRenderHeight;
			if (ImGui::SliderInt("rdr h", &rh, 480, 1024))
			{
				//mVDSession->setRenderHeight(rh);
				mVDSettings->mRenderHeight = rh;
			}
		}
		ImGui::PopStyleColor(3);

		if (ImGui::Button("Create Window")) {
			mVDSession->createWindow();
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Window")) {
			mVDSession->deleteWindow();
		}



		// alpha blending
		/*if (mVDSession->isEnabledAlphaBlending()) {
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 1.0f, 0.5f));
		}
		else {
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
		}
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.9f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.9f, 0.8f, 0.8f));

		if (ImGui::Button("Alpha Blending")) {
		mVDSession->toggleEnabledAlphaBlending();
		}
		ImGui::PopStyleColor(3);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Enabled Alpha Blending");
		ImGui::SameLine();
		// Render Texture
		if (mVDSession->isRenderTexture()) {
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 1.0f, 0.5f));
		}
		else {
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
		}
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.9f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.9f, 0.8f, 0.8f));

		if (ImGui::Button("Render Texture")) {
		mVDSession->toggleRenderTexture();
		}
		ImGui::PopStyleColor(3);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Render Texture or Mix");*/


		ImGui::Text("fp %dx%d f %dx%d", mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, mVDSettings->mFboWidth, mVDSettings->mFboHeight);
		ImGui::Text("main %dx%d", mVDSettings->mMainWindowWidth, mVDSettings->mMainWindowHeight);
		// feedback
		static int fb = mVDSession->getFeedbackFrames();
		if (ImGui::SliderInt("feedback", &fb, 0, 9))
		{
			mVDSession->setFeedbackFrames(fb);
		}
		ImGui::PopItemWidth();
	}
	ImGui::End();
}
