#include "VDUIMouse.h"

using namespace VideoDromm;

VDUIMouse::VDUIMouse(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIMouse::~VDUIMouse() {
	
}

void VDUIMouse::Run(const char* title) {
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
	// Mouse
#pragma region mouse

	ImGui::Begin("Mouse");
	{
		ImGui::Text("Position: %.1f,%.1f", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
		ImGui::Text("Clic %d", ImGui::GetIO().MouseDown[0]);
		mouseGlobal ^= ImGui::Button("mouse gbl");
		if (mouseGlobal)
		{
			mVDSettings->mRenderPosXY.x = ImGui::GetIO().MousePos.x; ImGui::SameLine();
			mVDSettings->mRenderPosXY.y = ImGui::GetIO().MousePos.y;
			//mVDSettings->iMouse.z = ImGui::GetIO().MouseDown[0];
		}
		else
		{
			//mVDSettings->iMouse.z = ImGui::Button("mouse click");
		}
		ImGui::SliderFloat("MouseX", &mVDSettings->mRenderPosXY.x, 0, mVDSettings->mFboWidth);
		ImGui::SliderFloat("MouseY", &mVDSettings->mRenderPosXY.y, 0, mVDSettings->mFboHeight);
	}
	ImGui::End();
#pragma endregion mouse
}
