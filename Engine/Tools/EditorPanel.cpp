/*********************************************************************
 * \file	EditorPanel.cpp
 * \brief	Defines a EditorPanel class that is the parent class of all panels.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#include "EditorPanel.hpp"

Gui::Entity* EditorPanel::selectedEntity = nullptr;
//std::vector<Gui::Entity> EditorPanel::sceneEntities;
std::list<Gui::Entity> EditorPanel::sceneEntities;
//std::unordered_map<uint32_t, Gui::Entity*> EditorPanel::sceneEntityMap;
std::unordered_map<uint32_t, std::list<Gui::Entity>::iterator> EditorPanel::sceneEntityMap;
std::unordered_map<uint32_t, Gui::Entity*> EditorPanel::sceneTransformUUID;

