#include "VDUIFbos.h"

using namespace VideoDromm;

VDUIFbos::VDUIFbos(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}

void VDUIFbos::Run(const char* title) {
	// fbos
	// xPos = mVDSettings->uiMargin + mVDSettings->uiXPosCol1;
	// yPos = mVDSettings->uiYPosRow2;

	for (unsigned int f = 0; f < mVDSession->getFboListSize(); f++) {
		xPos = mVDSettings->uiMargin + mVDSettings->uiXPosCol1 + ((mVDSettings->uiLargePreviewW + mVDSettings->uiMargin) * f);
		yPos = mVDSettings->uiYPosRow2;
		ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
		ImGui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);
		sprintf(buf, "%s##fbolbl%d", mVDSession->getFboName(f).c_str(), f);
		ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
		{
			ImGui::PushID(f);
			ImGui::Image((void*)mVDSession->getFboRenderedTexture(f)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			if (ImGui::IsItemHovered()) mVDSession->getFboTexture(f);
			for (unsigned int t = 0; t < mVDSession->getInputTexturesCount(); t++) {
				if (t > 0 && (t % 6 != 0)) ImGui::SameLine();
				if (mVDSession->getFboInputTextureIndex(f) == t) {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(t / 7.0f, 1.0f, 1.0f));
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(t / 7.0f, 0.1f, 0.1f));
				}
				sprintf(buf, "%d##fboit%d%d", t, f, t);
				if (ImGui::Button(buf)) mVDSession->setFboInputTexture(f, t);

				sprintf(buf, "Set input texture to %s", mVDSession->getInputTextureName(t).c_str());
				if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
				ImGui::PopStyleColor(1);
			}
			if (mVDSession->isFboFlipV(f)) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(f / 7.0f, 1.0f, 1.0f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(f / 7.0f, 0.1f, 0.1f));
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(f / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(f / 7.0f, 0.8f, 0.8f));
			sprintf(buf, "FlipV##fboflipv%d", f);
			if (ImGui::Button(buf)) mVDSession->fboFlipV(f);
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			sprintf(buf, "T##fboupd%d", f);
			if (ImGui::Button(buf)) mVDSession->updateShaderThumbFile(f);
			ImGui::Text("wh %dx%d", mVDSession->getFboRenderedTexture(f)->getWidth(), mVDSession->getFboRenderedTexture(f)->getHeight());
			ImGui::PopID();
		}
		ImGui::End();
	}
	// mix fbos
	for (unsigned int m = 0; m < mVDSession->getMixFbosCount(); m++) {
		ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
		ImGui::SetNextWindowPos(ImVec2((m * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3), ImGuiSetCond_Once);
		// TODO ImGui::Begin(mVDMix->getFboLabel(m).c_str(), NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		// heap problem: sprintf(buf, "%s##ml%d", mVDSession->getMixFboName(m).c_str(), m);
		sprintf(buf, "%d##ml%d", m, m);
		ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
		{
			ImGui::PushID(m+100);// unique id
			
			ImGui::Image((void*)mVDSession->getMixTexture(m)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			
			// loop on the fbos
			for (unsigned int a = 0; a < mVDSession->getFboListSize(); a++) {
				if (a > 0 && (a % 6 != 0)) ImGui::SameLine();
				if (mVDSession->getFboAIndex(m) == a) {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(a / 7.0f, 1.0f, 1.0f));
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(a / 7.0f, 0.1f, 0.1f));
				}

				sprintf(buf, "%d##wia%d%d", a, m, a);
				if (ImGui::Button(buf)) mVDSession->setFboAIndex(m, a);
				sprintf(buf, "Set input fbo A to %s", mVDSession->getShaderName(a).c_str());
				if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
				ImGui::PopStyleColor(1);
			}
			// loop on the fbos
			for (unsigned int b = 0; b < mVDSession->getFboListSize(); b++) {
				if (b > 0 && (b % 6 != 0)) ImGui::SameLine();
				if (mVDSession->getFboBIndex(m) == b) {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(b / 7.0f, 1.0f, 1.0f));
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(b / 7.0f, 0.1f, 0.1f));
				}
				sprintf(buf, "%d##wib%d%d", b, m, b);
				if (ImGui::Button(buf)) mVDSession->setFboBIndex(m, b);
				sprintf(buf, "Set input fbo B to %s", mVDSession->getShaderName(b).c_str());
				if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
				ImGui::PopStyleColor(1);
			}
			// crossfade
			float xFade = mVDSession->getMixCrossfade(m);
			sprintf(buf, "xfade##xf%d", m);
			if (ImGui::SliderFloat(buf, &xFade, 0.0f, 1.0f))
			{
				mVDSession->setMixCrossfade(m, xFade);
			}
			// spout output
			if (mVDSession->getSharedMixIndex() == m && mVDSession->isSharedOutputActive()) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 1.0f, 0.5f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.9f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.9f, 0.8f, 0.8f));
			sprintf(buf, "O##sp%d", m);
			if (ImGui::Button(buf)) {
				mVDSession->toggleSharedOutput(m);
			}
			ImGui::PopStyleColor(3);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle Spout output");
			ImGui::SameLine();
			ImGui::Text("mixwh %dx%d", mVDSession->getMixTexture(m)->getWidth(), mVDSession->getMixTexture(m)->getHeight());
			
			ImGui::PopID();
		}
		ImGui::End();
	}
}
