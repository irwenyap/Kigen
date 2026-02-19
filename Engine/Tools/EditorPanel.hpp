/*********************************************************************
 * \file	EditorPanel.hpp
 * \brief	Defines a EditorPanel class that is the parent class of all panels.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#ifndef EDITORPANEL_HPP
#define EDITORPANEL_HPP

#include <string>
#include <unordered_map>
#include <list>
#include "Gui.hpp"

class EditorPanel {
public:
	 /**
	 * @brief Updates the panel's content.
	 *
	 * This method should be overridden in derived classes to implement
	 * specific update logic for each panel. It is called once per frame
	 * to refresh the panel's state and display.
	 */
	virtual void Update(){}

	bool show = true; //indicate if panel visible
	std::string name; //name of panel
	
	static Gui::Entity* selectedEntity; //pointer to currently selected object

	// For Entity Lookups
	static std::unordered_map<uint32_t, std::list<Gui::Entity>::iterator> sceneEntityMap;

	// For Hierarchy Structure
	static std::list<Gui::Entity> sceneEntities;

	// For Entity Transform Lookups
	static std::unordered_map<uint32_t, Gui::Entity*> sceneTransformUUID;
};

#endif // !EDITORPANEL_HPP