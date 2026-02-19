/*********************************************************************
 * \file	HierarchyPanel.hpp
 * \brief	Defines a HierarchyPanel class that manages everything related to the hierarchy panel in the workspace.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#pragma once

#include "../EditorPanel.hpp"
#include <vector>

class EditorPanel;

class HierachyPanel : public EditorPanel {
public:

    /**
     * @brief Constructs a `HierarchyPanel` instance.
     *
     * Initializes the hierarchy panel with a default name, sets its visibility to true,
     * and populates the panel with initial game objects.
     */
	HierachyPanel();

    void Refresh();

    /**
     * @brief Updates the contents of the hierarchy panel.
     *
     * This method creates an ImGui window that displays the list of game objects
     * in the scene. Users can select an object, add a new object, or delete an
     * existing object. The selected object will display additional information.
     */
	void Update() override;

    void RenderHierarchy(Gui::Entity& entity);

    void MoveEntity(uint32_t entityID, uint32_t targetID, bool insertAfter);

    void ChangeParent(uint32_t childID, uint32_t newParentID);

    void RemoveParent(uint32_t currID);
	
private:
    void CreateEntity();
    void CreateTextboxUIEntity();
    void CreateQuadUIEntity();
    void CreateVideoUIEntity();
    void CreateCameraEntity();
    void DeleteEntity();
};