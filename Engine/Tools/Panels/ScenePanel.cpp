/*********************************************************************
 * \file	    ScenePanel.cpp
 * \brief	    Defines a ScenePanel class that manages everything
 *              related to the scene editor panel in the workspace.
 *
 * \author		wengkhong.thang, 2301372 (wengkhong.thang@digipen.edu)
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date	    29 September 2024

 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/
#include "ScenePanel.hpp"

#include <iostream>
#include "../ProcessTime.hpp"
#include "../../Utility/EngineConfig.hpp"
#include "../Engine/ECS/ECSManager.hpp"
#include "../ImGui/ImGuizmo/ImGuizmo.h"
#include "../Engine/Components/Name.hpp"
#include "../Engine/Components/Transform.hpp"
#include "../Engine/Components/Renderer.hpp"
#include "../Engine/Application.hpp"
#include "../Engine/Utility/Serializer.hpp"
#include "HierachyPanel.hpp"
#include "../PrefabManager.hpp"
#include "../../Components/Camera.hpp"

 //extern EngineConfig config;

 //std::filesystem::path droppedPath;//tmp drag and drop
 //bool isSceneFile = false;

 //float spMouseX = 0.f;
 //float spMouseY = 0.f;

extern HierachyPanel hp;

ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;

ScenePanel::ScenePanel() {
	name = "Scene";
	show = true;
	tempTexture2D = 0;

	textureUUID = "fbo9";

	//ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	//viewportSize = Vec2(viewportPanelSize.x,viewportPanelSize.y);
}

void ScenePanel::Init()
{
}

void ScenePanel::SetTextureId(int*) {
	//textureID = texture;
}

void ScenePanel::Update()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	
	if (ImGui::IsKeyPressed(ImGuiKey_7)) textureUUID = "fbo8"; // UI
	if (ImGui::IsKeyPressed(ImGuiKey_8)) textureUUID = "fbo9"; // ENGINE
	if (ImGui::IsKeyPressed(ImGuiKey_9)) textureUUID = "fbo12"; // OBJ_PICKING_UI
	if (ImGui::IsKeyPressed(ImGuiKey_0)) textureUUID = "fbo11"; // GAME_FINAL
	

	// Display the rendered scene
	//ImGui::Text("Rendered Scene");

	// Display time statistics
	//ImGui::Text("FPS: %.2f", ProcessTime::fps);
	//ImGui::SameLine();
	//ImGui::Text("Physics: %.2f%%", ProcessTime::percentMap[ProcessTime::PHYSICS]);
	//ImGui::SameLine();
	//ImGui::Text("Graphics: %.2f%%", ProcessTime::percentMap[ProcessTime::GRAPHICS]);
	//ImGui::SameLine();
	//ImGui::Text("Audio: %.2f%%", ProcessTime::percentMap[ProcessTime::AUDIO]);
	//ImGui::SameLine();
	//ImGui::Text("Misc: %.2f%%", ProcessTime::percentMap[ProcessTime::MISC]);

	auto& camera = GraphicsManager::GetInstance().camera;

	ImVec2 panelPos = ImGui::GetCursorScreenPos();
	ImVec2 panelSize = ImGui::GetContentRegionAvail();

	ImVec2 mousePos = ImGui::GetMousePos();
	float localX = mousePos.x - panelPos.x;
	float localY = mousePos.y - panelPos.y;
	float spMouseX = localX / panelSize.x;
	float spMouseY = localY / panelSize.y;

	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(true);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(panelPos.x, panelPos.y, panelSize.x, panelSize.y);

	if (localX < 0 || localY < 0 || localX > panelSize.x || localY > panelSize.y) {
		//return; // Mouse is outside the panel
	} else {
		float scrollOffset = ImGui::GetIO().MouseWheel;
		if (scrollOffset != 0) {
			float zoomSpeed = 0.1f; // Zoom sensitivity
			camera.SetZoom(scrollOffset * zoomSpeed);
		}

		// Moving the camera
		if (ImGui::IsWindowFocused()) {
			float camSpeed = 10.f;
			if (ImGui::IsKeyDown(ImGuiKey_UpArrow)) {
				camera.MoveUp(camSpeed);
			} else if (ImGui::IsKeyDown(ImGuiKey_DownArrow)) {
				camera.MoveDown(camSpeed);
			}
			if (ImGui::IsKeyDown(ImGuiKey_LeftArrow)) {
				camera.MoveLeft(camSpeed);
			} else if (ImGui::IsKeyDown(ImGuiKey_RightArrow)) {
				camera.MoveRight(camSpeed);
			}

			// Object Picking
			if (!ImGuizmo::IsUsingAny()) {
				if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
					int x = static_cast<int>(spMouseX * Application::GetWindowSize().first);
					int y = static_cast<int>(spMouseY * Application::GetWindowSize().second);
					Vec4 colour = GraphicsManager::GetInstance().GetPixelColor(
						GraphicsManager::GetInstance().frameBuffers[GraphicsManager::FrameBufferIndex::OBJ_PICKING_ENGINE], x, y);

					uint32_t clickedEntity = ECSManager::GetInstance().renderSystem->DecodeColor(colour);
					//std::cout << "Clicked on entity: " << clickedEntity << std::endl;
					if (sceneEntityMap.find(clickedEntity) != sceneEntityMap.end()) {
						selectedEntity = &*sceneEntityMap.find(clickedEntity)->second;
						//auto r = ECSManager::GetInstance().TryGetComponent<Renderer>(selectedEntity->id);
						//if (r.has_value()) {
						//	ECSManager::GetInstance().renderSystem->SetVisibility(r->get().currentMeshDebugID, true);
						//}
					}
					else {
						//if (selectedEntity) {
						//	auto r = ECSManager::GetInstance().TryGetComponent<Renderer>(selectedEntity->id);
						//	if (r.has_value()) {
						//		ECSManager::GetInstance().renderSystem->SetVisibility(r->get().currentMeshDebugID, false);
						//	}
						//}
						selectedEntity = nullptr;
					}
				}
			}
		}

		// Camera Dragging
		static bool isDragging = false;
		static ImVec2 lastMousePos;
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
			if (!isDragging) {
				isDragging = true;
				lastMousePos = ImGui::GetMousePos();
			}

			ImVec2 currentMousePos = ImGui::GetMousePos();
			ImVec2 delta = ImVec2(currentMousePos.x - lastMousePos.x, currentMousePos.y - lastMousePos.y);

			float viewportWidth = panelSize.x;
			float viewportHeight = panelSize.y;

			float ndcX = delta.x / viewportWidth * 2.0f;
			float ndcY = delta.y / viewportHeight * 2.0f;

			float worldDeltaX = ndcX * (camera.screenWidth / 2.0f) / camera.zoom;
			float worldDeltaY = ndcY * (camera.screenHeight / 2.0f) / camera.zoom;

			camera.MoveRight(-worldDeltaX);
			camera.MoveUp(worldDeltaY);

			lastMousePos = currentMousePos;
		} else {
			isDragging = false;
		}

		if (ImGui::IsKeyPressed(ImGuiKey_W)) {
			currentOperation = ImGuizmo::TRANSLATE;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_E)) {
			currentOperation = ImGuizmo::SCALE;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_R)) {
			currentOperation = ImGuizmo::ROTATE;
		}
	}

	RenderImguiImage();

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB")) {
			std::string prefabPath(static_cast<const char*>(payload->Data), payload->DataSize);
			Entity entt = Serializer::GetInstance().DeserializePrefab(prefabPath);

			//auto& camera = GraphicsManager::GetInstance().camera;
			ImVec2 currentMousePos = ImGui::GetMousePos();
			ImVec2 relativeMousePos = ImVec2(currentMousePos.x - panelPos.x, currentMousePos.y - panelPos.y);

			float ndcX = (relativeMousePos.x / panelSize.x) * 2.0f - 1.0f;

			float invertedY = panelSize.y - relativeMousePos.y;
			float ndcY = (invertedY / panelSize.y) * 2.0f - 1.0f;

			float worldX = ndcX * (camera.screenWidth / 2.0f) / camera.zoom + camera.position.x;
			float worldY = ndcY * (camera.screenHeight / 2.0f) / camera.zoom + camera.position.y;

			auto& n = ECSManager::GetInstance().GetComponent<Name>(entt);
			PrefabManager::GetInstance().prefabsMap[n.prefabID].push_back(entt);

			auto& t = ECSManager::GetInstance().GetComponent<Transform>(entt);
			t.position = Vec3(worldX, worldY, 0.f);

			auto& r = ECSManager::GetInstance().GetComponent<Renderer>(entt);
			std::pair<size_t, size_t> mesh = ECSManager::GetInstance().renderSystem->AddMesh(r.mesh);
			r.currentMeshID = mesh.first;
			r.currentMeshDebugID = mesh.second;
			ECSManager::GetInstance().renderSystem->SetTextureToMesh(mesh.first, r.uuid);
			ECSManager::GetInstance().renderSystem->SetVisibility(mesh.first, true);
			r.isInitialized = true;

			ECSManager::GetInstance().renderSystem->SetColorToEntity(entt, ECSManager::GetInstance().renderSystem->EncodeColor(entt));

			for (size_t i = 0; i < GraphicsManager::BatchIndex::MAX_BATCHES; ++i) {
				GraphicsManager::GetInstance().SetBatchUpdateFlag(
					static_cast<GraphicsManager::BatchIndex>(i)
				);
			}

			sceneEntities.emplace_back(Gui::Entity{ n.name, entt, true });
			sceneEntityMap[entt] = std::prev(sceneEntities.end());
			sceneTransformUUID[ECSManager::GetInstance().GetComponent<Transform>(entt).uuid] = &sceneEntities.back();
		}
		ImGui::EndDragDropTarget();
	}

	if (selectedEntity) {
		auto& transform = ECSManager::GetInstance().GetComponent<Transform>(selectedEntity->id);

		float modelMatrix[16];


		// If the entity has a parent, convert local to world before manipulation
		Mat4 worldMatrix;
		if (transform.parent != MAX_ENTITIES) {
			// Convert local transform into matrix
			ImGuizmo::RecomposeMatrixFromComponents(
				&transform.localPosition.x,
				&transform.localRotation.x,
				&transform.localScale.x,
				modelMatrix
			);

			worldMatrix = ECSManager::GetInstance().GetComponent<Transform>(transform.parent).modelToWorldMtx * Mat4(modelMatrix);
		} else {
			// Convert local transform into matrix
			ImGuizmo::RecomposeMatrixFromComponents(
				&transform.position.x,
				&transform.rotation.x,
				&transform.scale.x,
				modelMatrix
			);

			worldMatrix = Mat4(modelMatrix);
		}

		float worldMatrixGuizmo[16];
		memcpy(worldMatrixGuizmo, worldMatrix.a, sizeof(float) * 16);

		bool manipulated = ImGuizmo::Manipulate(
			glm::value_ptr(GraphicsManager::GetInstance().camera.GetViewMatrix()),
			glm::value_ptr(GraphicsManager::GetInstance().camera.GetProjectionMatrix()),
			currentOperation,
			transform.parent ? ImGuizmo::LOCAL : ImGuizmo::WORLD,
			worldMatrixGuizmo
		);

		// If transform was changed, decompose the new matrix back into transform components
		if (manipulated) {
			Mat4 newWorldMatrix = Mat4(worldMatrixGuizmo);
			Mat4 newLocalMatrix;

			if (transform.parent != MAX_ENTITIES) {
				Mat4 parentInverse = ECSManager::GetInstance().GetComponent<Transform>(transform.parent).modelToWorldMtx.Inverse();
				newLocalMatrix = parentInverse * newWorldMatrix;

				ImGuizmo::DecomposeMatrixToComponents(
					newLocalMatrix.a,
					&transform.localPosition.x,
					&transform.localRotation.x,
					&transform.localScale.x
				);
			} else {
				newLocalMatrix = newWorldMatrix;

				ImGuizmo::DecomposeMatrixToComponents(
					newLocalMatrix.a,
					&transform.position.x,
					&transform.rotation.x,
					&transform.scale.x
				);
			}

			auto cameraCom = ECSManager::GetInstance().TryGetComponent<Camera>(selectedEntity->id);
			if (cameraCom.has_value()) {
				cameraCom->get().hasUpdatedView = true;
			}

			transform.updated = true;
		}

		if (ImGui::IsKeyPressed(ImGuiKey_F)) {
			camera.SetPosition(transform.position.x, transform.position.y);
			camera.SetZoom(0.2f);
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

void ScenePanel::RenderImguiImage() {
	auto& camera = GraphicsManager::GetInstance().camera;
	ImVec2 availableSize = ImGui::GetContentRegionAvail();

	// Dynamically update the camera dimensions to match the panel size
	camera.screenWidth = availableSize.x;
	camera.screenHeight = availableSize.y;
	camera.SetProjectionMatrix();

	if (tempTexture2D == 0) {
		tempTexture2D = GraphicsManager::GetInstance().GenerateTempTexture(textureUUID.c_str());
	} else {
		GraphicsManager::GetInstance().BindTempTexture(textureUUID.c_str(), tempTexture2D);
	}

	GraphicsManager::GetInstance().UpdateTempTexture(textureUUID.c_str(), tempTexture2D);

	ImGui::Image((void*)(intptr_t)(tempTexture2D), availableSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

	GraphicsManager::GetInstance().UnbindTempTexture();
}

//void ScenePanel::RenderImguiImage()
//{
//    // Aspect ratio is fixed at 16:9
//    static const float ar = 16.f / 9.f;
//
//    ImVec2 availableSize = ImGui::GetContentRegionAvail();
//
//    ImVec2 displaySize;
//    if (availableSize.x / availableSize.y > ar) {
//        // Fit by height
//        displaySize.y = availableSize.y;
//        displaySize.x = displaySize.y * ar;
//    }
//    else {
//        // Fit by width
//        displaySize.x = availableSize.x;
//        displaySize.y = displaySize.x / ar;
//    }
//
//    ImVec2 offset = ImVec2((availableSize.x - displaySize.x) * 0.5f,
//                            (availableSize.y - displaySize.y) * 0.5f);
//
//    // Should be half of the available size
//    //offset.x = offset.x + availableSize.x * 0.5f - displaySize.x * 0.5f;
//    //offset.y = offset.y + availableSize.y * 0.5f - displaySize.y * 0.5f;
//    offset.x = (availableSize.x - displaySize.x) * 0.5f;
//    offset.y = (availableSize.y - displaySize.y) * 0.5f;
//
//
//    //ImVec2 pos = ImGui::GetWindowPos();
//    //std::cout << ImGui::GetMousePos().x << " " << ImGui::GetMousePos().y << std::endl;
//
//    ImGui::SetCursorPos(offset);
//
//    if (tempTexture2D == 0) {
//        tempTexture2D = GraphicsManager::GetInstance().GenerateTempTexture(textureUUID.c_str());
//    }
//    else {
//        GraphicsManager::GetInstance().BindTempTexture(textureUUID.c_str(), tempTexture2D);
//    }
//
//    GraphicsManager::GetInstance().UpdateTempTexture(textureUUID.c_str(), tempTexture2D);
//
//    ImGui::Image((void*)(intptr_t)(tempTexture2D), availableSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
//    //ImGui::Image((void*)(intptr_t)(tempTexture2D), ImVec2(1280, 720), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
//
//    GraphicsManager::GetInstance().UnbindTempTexture();
//}

void ScenePanel::Exit()
{
}