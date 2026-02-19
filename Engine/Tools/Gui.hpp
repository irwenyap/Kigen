/*********************************************************************
 * \file	Gui.hpp
 * \brief	Defines a Gui class that contains the logic to render and setup the Graphic User Interface (IMGUI).
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#ifndef GUI_HPP
#define GUI_HPP

#include <string>
#include <iostream>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
//#include "Workspace.hpp"
#include "../Graphics/GraphicsManager.hpp"

//tmp
#include "../Utility/EngineState.hpp"

#include <list>

class Gui {
public:

	struct Entity
	{
		std::string name;
		uint32_t id;

		bool isPrefab = false;

		Gui::Entity* parent = nullptr;              // Parent entity ID (NO_ENTITY if root)
		std::list<Gui::Entity*> children;           // List of child entity IDs

		bool operator==(const Entity& other) const {
			return id == other.id;
		}
	};

	/**
	 * @brief Initializes the GUI system using ImGui.
	 *
	 * This method sets up the ImGui context, initializes the rendering backends for GLFW and OpenGL,
	 * and adds predefined panels to the workspace. It configures ImGui for docking and dark styling.
	 *
	 * @param window A pointer to the GLFW window that will be used for rendering the GUI.
	 */
	static void Init(GLFWwindow* window);

	/**
	 * @brief Updates the GUI for the current frame.
	 *
	 * This method begins a new ImGui frame, updates the panels in the workspace,
	 * and handles rendering of the GUI components. It also provides buttons for
	 * controlling audio playback through the provided AudioManager instance.
	 *
	 * @param textureID The texture ID to be used for rendering in the GUI.
	 * @param audio A pointer to the AudioManager instance for audio control.
	 */
	static void Update(int textureID);

	/**
	 * @brief Cleans up the GUI system.
	 *
	 * This method shuts down the ImGui OpenGL and GLFW backends and destroys
	 * the ImGui context, releasing any resources used by the GUI system.
	 */
	static void Exit();
};

#endif // !GUI_HPP