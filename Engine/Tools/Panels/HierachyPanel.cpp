/*********************************************************************
 * \file	    HierarchyPanel.cpp
 * \brief	    Defines a HierarchyPanel class that manages everything 
 *              related to the hierarchy panel in the workspace.
 * 
 * \author	    y.ziyangirwen, 2301345
 * \author	    wengkhong.thang, 2301372
 * \email	    wengkhong.thang@digipen.edu
 * \date	    29 September 2024

 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/
#include "HierachyPanel.hpp"

//temp
#include <iostream>

#include "../../ECS/ECSManager.hpp"
#include "../../EventManager.hpp"
#include "../Engine/Components/Name.hpp"
#include "../Engine/Components/Renderer.hpp"
#include "../Engine/Components/UI.hpp"
#include "../Engine/Components/Textbox.hpp"
#include "../../Components/Transform.hpp"
#include "../../Components/VideoPlayer.hpp"
#include "../../AssetManager.hpp"
#include "../../Components/Camera.hpp"


HierachyPanel::HierachyPanel() {
	name = "Hierachy";
	show = true;

    //uint32_t numEntt = ECSManager::GetInstance().GetEntityManager().GetEntities();
    //for (uint32_t i = 0; i < numEntt; ++i) {
    //    Gui::Entity entt = { "Unnamed Object" };
    //    sceneObjects.push_back(entt);
    //}
}

void HierachyPanel::Refresh() {
    sceneEntities.clear();
    sceneEntityMap.clear();
    sceneTransformUUID.clear();

    auto& ecs = ECSManager::GetInstance();
    auto& entityManager = ecs.GetEntityManager();
    uint32_t numEntities = entityManager.GetEntities();

    for (uint32_t i = 0; i < numEntities; ++i) {
        bool isPrefab = (ecs.GetComponent<Name>(i).prefabID != "") ? true : false;
        sceneEntities.emplace_back(ecs.GetComponent<Name>(i).name, i, isPrefab);
        sceneEntityMap[i] = std::prev(sceneEntities.end());
        sceneTransformUUID[ecs.GetComponent<Transform>(i).uuid] = &sceneEntities.back();
    }

    for (auto& entity : sceneEntities) {
        auto& transform = ecs.GetComponent<Transform>(entity.id);

        if (transform.parentUUID != 0) {
            auto parentIt = sceneTransformUUID.find(transform.parentUUID);
            if (parentIt != sceneTransformUUID.end()) {
                entity.parent = parentIt->second;
                parentIt->second->children.push_back(&entity);
            }
        }
    }
}

void HierachyPanel::Update() {
    //ImGui::SetNextWindowPos(ImVec2(1286, 44), ImGuiCond_Once);
    //ImGui::SetNextWindowSize(ImVec2(1308,389), ImGuiCond_Once);

    //if (selectedEntity) std::cout << selectedEntity->id << std::endl;
    //ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;
    ImGui::Begin(name.c_str(), nullptr);


    ImGui::Text("Scene: ");
    ImGui::Separator();

    if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup("HierarchyContextMenu");
    }

    if (ImGui::BeginPopupContextWindow("HierarchyContextMenu")) {
        if (ImGui::MenuItem("Create Entity")) {
            CreateEntity();
        }
        if (ImGui::MenuItem("Create Camera")) {
            CreateCameraEntity();
        }

        ImGui::Separator();

        if (ImGui::BeginMenu("UI")) { // Creates a submenu with an arrow
            if (ImGui::MenuItem("Create Textbox")) {
                CreateTextboxUIEntity();
            }
            if (ImGui::MenuItem("Create Image")) {
                CreateQuadUIEntity();
            }
            if (ImGui::MenuItem("Create Video")) {
                CreateVideoUIEntity();
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
        

    if (ImGui::IsWindowFocused() && selectedEntity && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
        DeleteEntity();
    }
    
    // Render all root entities
    for (auto& entity : sceneEntities) {
        if (entity.parent == nullptr) {
            RenderHierarchy(entity);
        }
    }

    ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y));

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_HIERARCHY_OBJECT")) {
            IM_ASSERT(payload->DataSize == sizeof(uint32_t));
            uint32_t droppedEntityID = *(uint32_t*)payload->Data;

            // Remove the parent since it was dropped onto an empty area
            RemoveParent(droppedEntityID);
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}

void HierachyPanel::RenderHierarchy(Gui::Entity& entity) {
    ImGui::PushID(static_cast<int>(entity.id));

    bool isActive = ECSManager::GetInstance().GetEntityManager().GetActive(entity.id);

    // Highlight the selected entity
    bool isSelected = (selectedEntity && selectedEntity->id == entity.id);

    // Use TreeNodeEx for more control
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (entity.children.empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf; // No children, mark as leaf
    }
    if (isSelected) {
        flags |= ImGuiTreeNodeFlags_Selected; // Mark as selected
    }

    if (!isActive) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Grayed-out text color
    }

    // Render the tree node
    bool nodeOpen = ImGui::TreeNodeEx(entity.name.c_str(), flags);

    if (!isActive) {
        ImGui::PopStyleColor();
    }

    // **Capture UI position of the item**
    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();
    float entityHeight = max.y - min.y;
    float topThreshold = min.y + (entityHeight * 0.25f);
    float bottomThreshold = max.y - (entityHeight * 0.25f);

    // Handle selection logic
    if (ImGui::IsItemClicked()) {
        selectedEntity = &entity;
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        ImGui::SetDragDropPayload("ENTITY_HIERARCHY_OBJECT", &entity.id, sizeof(uint32_t));
        ImGui::Text("Dragging: %s", entity.name.c_str());
        ImGui::EndDragDropSource();
    }

    ImVec2 mousePos = ImGui::GetMousePos();
    bool isHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly);
    if (isHovered && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        if (mousePos.y < bottomThreshold)
            drawList->AddLine(ImVec2(min.x, min.y), ImVec2(max.x, min.y), IM_COL32(255, 255, 255, 255), 2.0f);
        else if (mousePos.y > topThreshold)
            drawList->AddLine(ImVec2(min.x, max.y), ImVec2(max.x, max.y), IM_COL32(255, 255, 255, 255), 2.0f);
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_HIERARCHY_OBJECT")) {

            uint32_t draggedID = *(uint32_t*)payload->Data;
            if (draggedID != entity.id) {
                if (mousePos.y >= topThreshold && mousePos.y <= bottomThreshold) {
                    ChangeParent(draggedID, entity.id);
                }
                else {
                    if (mousePos.y < bottomThreshold) {
                        MoveEntity(draggedID, entity.id, false);
                    } else if (mousePos.y > topThreshold) {
                        MoveEntity(draggedID, entity.id, true);
                    }
                    if (sceneEntityMap[draggedID]->parent)
                        RemoveParent(draggedID);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    // Render children if node is open
    if (nodeOpen) {
        for (auto child : entity.children) {
            RenderHierarchy(*child);
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void HierachyPanel::MoveEntity(uint32_t entityID, uint32_t targetID, bool insertAfter) {
    auto itEntity = sceneEntityMap.find(entityID);
    auto itTarget = sceneEntityMap.find(targetID);
    if (itEntity == sceneEntityMap.end() || itTarget == sceneEntityMap.end()) return;

    auto entityIter = itEntity->second;
    auto targetIter = itTarget->second;

    if (!insertAfter) {
        if (targetIter == sceneEntities.begin()) {
            sceneEntities.splice(sceneEntities.begin(), sceneEntities, entityIter);
            sceneEntityMap[entityID] = sceneEntities.begin();
            return;
        }
    }

    auto insertionPoint = insertAfter ? std::next(targetIter) : targetIter;
    sceneEntities.splice(insertionPoint, sceneEntities, entityIter);
    sceneEntityMap[entityID] = entityIter;
}

void HierachyPanel::ChangeParent(uint32_t childID, uint32_t newParentID) {
    //Gui::Entity* child = sceneEntityMap[childID];
    auto childIt = sceneEntityMap.find(childID);
    auto parentIt = sceneEntityMap.find(newParentID);
    if (childIt == sceneEntityMap.end() || parentIt == sceneEntityMap.end()) return;

    auto& child = *childIt->second;
    auto& newParent = *parentIt->second;

    auto& childT = ECSManager::GetInstance().GetComponent<Transform>(childID);
    auto& parentT = ECSManager::GetInstance().GetComponent<Transform>(newParentID);

    if (child.parent) {
        child.parent->children.remove(&child);
    }
    child.parent = &newParent;
    newParent.children.push_back(&child);

    //if (child->parent) {
    //    auto& siblings = child->parent->children;
    //    siblings.erase(std::remove(siblings.begin(), siblings.end(), child), siblings.end());
    //}

    //child->parent = sceneEntityMap[newParentID];
    //if (child->parent) {
    //    child->parent->children.push_back(child);
    //}

    // Updating component ( NEED TO FIX )
    childT.parent = newParentID;
    childT.parentUUID = parentT.uuid;
    parentT.children.push_back(childID);

    Mat4 parentInverse = parentT.modelToWorldMtx.Inverse();
    Mat4 childLocalTransform = parentInverse * childT.modelToWorldMtx;

    childT.localPosition = childLocalTransform.GetTranslation();
    childT.localScale = childLocalTransform.GetScale();
    childT.localRotation = childLocalTransform.GetRotation();
}

void HierachyPanel::RemoveParent(uint32_t currID) {
    auto it = sceneEntityMap.find(currID);
    if (it == sceneEntityMap.end()) return;
    auto& entity = *it->second;
    //if (!entity.parent) return;

    //entity.parent = nullptr;

    //Gui::Entity* child = sceneEntityMap[currID];

    if (!entity.parent) {
        //std::cerr << "RemoveParent: Child has no parent!" << std::endl;
        if (!entity.children.empty()) {
            //auto& parentT = ECSManager::GetInstance().GetComponent<Transform>(child->id);
            for (auto& entt : it->second->children) {

                auto& childT = ECSManager::GetInstance().GetComponent<Transform>(entt->id);
                childT.position = childT.modelToWorldMtx.GetTranslation();
                childT.scale = childT.modelToWorldMtx.GetScale();
                childT.rotation = childT.modelToWorldMtx.GetRotation();

                childT.parent = MAX_ENTITIES;
                childT.parentUUID = 0;
                entt->parent = nullptr;
            }
        }

        return;
    }

    auto& childT = ECSManager::GetInstance().GetComponent<Transform>(currID);
    auto& parentT = ECSManager::GetInstance().GetComponent<Transform>(childT.parent);

    auto& siblings = parentT.children;
    siblings.erase(std::remove(siblings.begin(), siblings.end(), currID), siblings.end());

    childT.modelToWorldMtx = parentT.modelToWorldMtx * Mat4::BuildTranslation(childT.localPosition) *
        Mat4::BuildZRotation(childT.localRotation.z) *
        Mat4::BuildScaling(childT.localScale.x, childT.localScale.y, childT.localScale.z);

    //auto& guiParentChildren = child->parent->children;
    //guiParentChildren.erase(std::remove(guiParentChildren.begin(), guiParentChildren.end(), child), guiParentChildren.end());
    entity.parent->children.remove(&entity);

    entity.parent = nullptr;
    childT.parent = MAX_ENTITIES;
    childT.parentUUID = 0;

    childT.position = childT.modelToWorldMtx.GetTranslation();
    childT.scale = childT.modelToWorldMtx.GetScale();
    childT.rotation = childT.modelToWorldMtx.GetRotation();

    childT.updated = true;
}

void HierachyPanel::CreateEntity() {
    Entity entt = ECSManager::GetInstance().CreateEntity();
    Gui::Entity guiEntity = { ECSManager::GetInstance().GetComponent<Name>(entt).name, entt };

    sceneEntities.push_back(std::move(guiEntity));
    sceneEntityMap[entt] = std::prev(sceneEntities.end());

    std::cout << "Hierarchy Created ID: " << entt << std::endl;
    std::pair<size_t, size_t> id = ECSManager::GetInstance().renderSystem->AddMesh(RenderSystem::MeshType::QUAD);
    ECSManager::GetInstance().renderSystem->SetTextureToMesh(id.first, "");
    ECSManager::GetInstance().renderSystem->SetVisibility(id.first, true);
    ECSManager::GetInstance().AddComponent(entt, Renderer(id.first, id.second, ""));

	auto& renderer = ECSManager::GetInstance().GetComponent<Renderer>(entt);
	renderer.isInitialized = true;

    ECSManager::GetInstance().renderSystem->SetColorToEntity(entt, ECSManager::GetInstance().renderSystem->EncodeColor(entt));

	for (size_t i = 0; i < GraphicsManager::BatchIndex::MAX_BATCHES; ++i) {
		GraphicsManager::GetInstance().SetBatchUpdateFlag(
            static_cast<GraphicsManager::BatchIndex>(i)
        );
    }

    //selectedEntity = &guiEntity;
}

void HierachyPanel::CreateTextboxUIEntity() {
    Entity entt = ECSManager::GetInstance().CreateEntity();
    Gui::Entity guiEntity = { ECSManager::GetInstance().GetComponent<Name>(entt).name, entt };
    sceneEntities.push_back(std::move(guiEntity));
    sceneEntityMap[entt] = std::prev(sceneEntities.end());
    std::cout << "Hierarchy Created ID: " << entt << std::endl;

    std::pair<size_t, size_t> id = ECSManager::GetInstance().renderSystem->AddMesh(RenderSystem::MeshType::QUAD);
    ECSManager::GetInstance().renderSystem->SetTextureToMesh(id.first, "");
    ECSManager::GetInstance().renderSystem->SetVisibility(id.first, true);
    ECSManager::GetInstance().AddComponent(entt, Renderer(id.first, id.second, ""));

    ECSManager::GetInstance().renderSystem->SetColorToEntity(entt, ECSManager::GetInstance().renderSystem->EncodeColor(entt));
    ECSManager::GetInstance().AddComponent(entt, UI());

    ECSManager::GetInstance().AddComponent(entt, Textbox("Hello"));

	ECSManager::GetInstance().uiSystem->SetTextbox(entt);

    for (auto& batch : GraphicsManager::GetInstance().batches) {
        batch.UpdateBuffers();
    }
}

void HierachyPanel::CreateQuadUIEntity() {
    Entity entt = ECSManager::GetInstance().CreateEntity();
    Gui::Entity guiEntity = { ECSManager::GetInstance().GetComponent<Name>(entt).name, entt };
    sceneEntities.push_back(std::move(guiEntity));
    sceneEntityMap[entt] = std::prev(sceneEntities.end());
    std::cout << "Hierarchy Created ID: " << entt << std::endl;

    std::pair<size_t, size_t> id = ECSManager::GetInstance().renderSystem->AddMesh(RenderSystem::MeshType::QUAD_UI);
    ECSManager::GetInstance().renderSystem->SetTextureToMesh(id.first, "");
    ECSManager::GetInstance().renderSystem->SetVisibility(id.first, true);
    ECSManager::GetInstance().AddComponent(entt, Renderer(id.first, id.second, ""));
    ECSManager::GetInstance().GetComponent<Renderer>(entt).mesh = RenderSystem::MeshType::QUAD_UI;

    ECSManager::GetInstance().AddComponent(entt, UI());

    ECSManager::GetInstance().renderSystem->SetColorToEntity(entt, ECSManager::GetInstance().renderSystem->EncodeColor(entt));

    for (auto& batch : GraphicsManager::GetInstance().batches) {
        batch.UpdateBuffers();
    }
}

void HierachyPanel::CreateVideoUIEntity() {
    Entity entt = ECSManager::GetInstance().CreateEntity();
    Gui::Entity guiEntity = { ECSManager::GetInstance().GetComponent<Name>(entt).name, entt };
    sceneEntities.push_back(std::move(guiEntity));
    sceneEntityMap[entt] = std::prev(sceneEntities.end());
    std::cout << "Hierarchy Created ID: " << entt << std::endl;

    std::pair<size_t, size_t> id = ECSManager::GetInstance().renderSystem->AddMesh(RenderSystem::MeshType::VIDEO_UI);
    ECSManager::GetInstance().renderSystem->SetTextureToMesh(id.first, "");
    ECSManager::GetInstance().renderSystem->SetVisibility(id.first, true);
    ECSManager::GetInstance().AddComponent(entt, Renderer(id.first, id.second, ""));
    ECSManager::GetInstance().GetComponent<Renderer>(entt).mesh = RenderSystem::MeshType::VIDEO_UI;

    ECSManager::GetInstance().AddComponent(entt, UI());

    ECSManager::GetInstance().renderSystem->SetColorToEntity(entt, ECSManager::GetInstance().renderSystem->EncodeColor(entt));

    VideoPlayer vp;
    //vp.videoClip = *AssetManager::GetInstance().Get<VideoClip>("194cd9a50c1-2100e658450661ed-dbf0ea115e2a84c7");
    vp.meshID = id.first;
    ECSManager::GetInstance().AddComponent(entt, vp);

    for (auto& batch : GraphicsManager::GetInstance().batches) {
        batch.UpdateBuffers();
    }
}

void HierachyPanel::CreateCameraEntity() {
    Entity entt = ECSManager::GetInstance().CreateEntity();
    Gui::Entity guiEntity = { ECSManager::GetInstance().GetComponent<Name>(entt).name, entt };
    sceneEntities.push_back(std::move(guiEntity));
    sceneEntityMap[entt] = std::prev(sceneEntities.end());
    std::cout << "Hierarchy Created ID: " << entt << std::endl;

    ECSManager::GetInstance().AddComponent(entt, Camera());

	ECSManager::GetInstance().cameraSystem->SetActiveCamera(entt);

    for (auto& batch : GraphicsManager::GetInstance().batches) {
        batch.UpdateBuffers();
    }
}

void HierachyPanel::DeleteEntity() {
    std::cout << "Hierarchy Removed ID: " << selectedEntity->id << std::endl;
    uint32_t idToDelete = selectedEntity->id;
    RemoveParent(idToDelete);
    
    // Find entity iterator and erase from the list
    auto it = std::find_if(sceneEntities.begin(), sceneEntities.end(),
        [&](Gui::Entity& obj) { return obj.id == idToDelete; });

    if (it != sceneEntities.end()) {
        sceneEntityMap.erase(idToDelete);
        sceneEntities.erase(it);
    }


    //auto it = std::find_if(sceneEntities.begin(), sceneEntities.end(), [](Gui::Entity& obj) {
    //    return &obj == selectedEntity;
    //    });

    //size_t indexToRemove = std::distance(sceneEntities.begin(), it);
    

    //sceneEntities.erase(it);
    //std::swap(sceneEntities[indexToRemove], sceneEntities.back());
    //sceneEntities.pop_back();


    auto r = ECSManager::GetInstance().TryGetComponent<Renderer>(idToDelete);
    if (r.has_value()) {
        ECSManager::GetInstance().renderSystem->SetVisibility(r->get().currentMeshID, false);
        ECSManager::GetInstance().renderSystem->SetVisibility(r->get().currentMeshDebugID, false);
    }
    ECSManager::GetInstance().DestroyEntity(idToDelete);

    selectedEntity = nullptr;
}
