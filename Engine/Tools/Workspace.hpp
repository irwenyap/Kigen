/*********************************************************************
 * \file	Workspace.hpp
 * \brief	Defines a Workspace class that contains the panels needed to be rendered by the engine.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "EditorPanel.hpp"
#include <vector>

class Workspace {
public:

	Workspace();//constructor

    /**
    * @brief Adds an editor panel to the workspace.
    *
    * This static method allows for the addition of an `EditorPanel` to the workspace.
    * The panel is stored in a static vector, enabling easy access and management of
    * all added panels.
    *
    * @param panel A pointer to the `EditorPanel` to be added to the workspace.
    */
	static void AddPanel(EditorPanel* panel) { panels.push_back(panel); }

    // Static vector holding pointers to all editor panels in the workspace.
	static std::vector<EditorPanel*> panels;

};



#endif // !WORKSPACE_H