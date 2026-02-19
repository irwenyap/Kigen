/*********************************************************************
 * \file	GameViewPanel.cpp
 * \brief	Defines a GameViewPanel class that manages everything related to
 *			the game view panel in the workspace.
 *
 * \author	y.ziyangirwen, 2301345
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#include "GameViewPanel.hpp"
#include "../Engine/ECS/ECSManager.hpp"
#include "../../Components/Camera.hpp"

GameViewPanel::GameViewPanel()
{
	name = "Game";
	show = true;

	tempTexture2D = 0;
	textureUUID = "fbo11";
}

void GameViewPanel::Init()
{


}

void GameViewPanel::Update()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);


	//auto& camera = GraphicsManager::GetInstance().camera;
	auto camera = ECSManager::GetInstance().TryGetComponent<Camera>(GraphicsManager::GetInstance().activeCamera);
	if (camera.has_value()) {
		static const float ar = camera->get().width / camera->get().height;
		ImVec2 availableSize = ImGui::GetContentRegionAvail();

		ImVec2 displaySize;
		if (availableSize.x / availableSize.y > ar) {
		    // Fit by height
		    displaySize.y = availableSize.y;
		    displaySize.x = displaySize.y * ar;
		}
		else {
		    // Fit by width
		    displaySize.x = availableSize.x;
		    displaySize.y = displaySize.x / ar;
		}

		camera->get().hasUpdatedProj = true;

		if (tempTexture2D == 0) {
			tempTexture2D = GraphicsManager::GetInstance().GenerateTempTexture(textureUUID.c_str());
		} else {
			GraphicsManager::GetInstance().BindTempTexture(textureUUID.c_str(), tempTexture2D);
		}

		GraphicsManager::GetInstance().UpdateTempTexture(textureUUID.c_str(), tempTexture2D);

		//ImGui::Image((void*)(intptr_t)(tempTexture2D), ImVec2(camera->get().width, camera->get().height), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::Image((void*)(intptr_t)(tempTexture2D), displaySize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

		GraphicsManager::GetInstance().UnbindTempTexture();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

void GameViewPanel::Exit()
{
}
