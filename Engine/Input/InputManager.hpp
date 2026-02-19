/*********************************************************************
 * \file		InputManager.hpp
 * \brief		Handles keyboard, mouse and gamepad input
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <unordered_map>
#include "../Graphics/Window.hpp"
#include "../Tools/ImGui/imgui_impl_glfw.h"
#include "../Core/Vec.hpp"
#include <iostream>

class InputManager {
public:
	static InputManager& GetInstance();

    void Initialise(GLFWwindow* window);
    void Update();
    
	bool GetKeyDown(int key);
	bool GetKey(int key);

    bool GetMouseDown(int button);
    bool GetMouse(int button);

    double GetMouseX() const;
    double GetMouseY() const;

    /**
     * \brief Returns whether there is a connected gamepad.
     *
     * This method checks if there is a valid connectedGamepadID.
     *
     * \return True if there is a connected gamepad, and false otherwise.
     */
    bool IsGamepadConnected();

    /**
     * \brief Gets the connected gamepad's left thumbstick movement axis.
     *
     * This method returns a Vec2 representing the connected gamepad's left
     * thumbstick movement axis from -1.0 to 1.0 inclusive.
     *
     * \return Vec2 representing the gamepad's left thumbstick movement axis
     * from -1.0 to 1.0 inclusive.
     */
    Vec2 GetGamepadLeftStick();

    /**
     * \brief Returns whether the specified button is held down on the
     * connected gamepad.
     * 
     * This method returns true if the specified button is held down on
     * the current frame.
     *
     * \return True if the specified button is held down, and false
     * otherwise.
     */
    bool IsGamepadButtonDown(int button);

    /**
     * \brief Returns whether the specified button is pressed down on the
     * connected gamepad.
     *
     * This method returns true if the specified button is pressed down on
     * the current frame.
     *
     * \return True if the specified button is pressed down, and false
     * otherwise.
     */
    bool IsGamepadButtonPressed(int button);

    bool GetAnyKeyDown() const;

private:
    InputManager() : cursorX(0.0), cursorY(0.0), scrollOffsetX(0.0), scrollOffsetY(0.0), connectedGamepadID(NO_GAMEPAD) {}

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        window, scancode, mods;
        InputManager::GetInstance().keyStates[key] = (action != GLFW_RELEASE);
#ifndef INSTALLER
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
#endif
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        window, mods;
        InputManager::GetInstance().mouseButtonStates[button] = (action != GLFW_RELEASE);
#ifndef INSTALLER
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
#endif
    }

    static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
        window;
        InputManager::GetInstance().cursorX = xpos;
        InputManager::GetInstance().cursorY = ypos;
#ifndef INSTALLER
        ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
#endif
    }

    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        window;
        InputManager::GetInstance().scrollOffsetX += xoffset;
        InputManager::GetInstance().scrollOffsetY += yoffset;
#ifndef INSTALLER
        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
#endif
    }

    /**
     * \brief Callback function for gamepad connection/disconnection.
     *
     * This function gets called whenever GLFW detects a gamepad connection
     * or disconnection.
     *
     * \param int gamepadID The detected gamepadID.
     * \param int event The event type detected from GLFW.
     */
    static void GamepadCallback(int gamepadID, int event) {
        if (!InputManager::GetInstance().IsGamepadConnected() && glfwJoystickPresent(gamepadID) && glfwJoystickIsGamepad(gamepadID) && event == GLFW_CONNECTED) {
            InputManager::GetInstance().connectedGamepadID = gamepadID;
            std::cout << "Gamepad connected: " << glfwGetGamepadName(gamepadID) << std::endl;
        }
        else if (event == GLFW_DISCONNECTED) {
            std::cout << "Gamepad disconnected." << std::endl;
            InputManager::GetInstance().connectedGamepadID = NO_GAMEPAD;
        }
    }

    /**
     * \brief Checks whether there is a connected gamepad.
     *
     * This method loops through all GLFW joysticks to check if there is a
     * connected gamepad.
     */
    void InitialiseGamepads() {
        for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; ++i) {
            if (glfwJoystickPresent(i) && glfwJoystickIsGamepad(i)) {
                std::cout << "Gamepad detected at startup: " << glfwGetGamepadName(i) << std::endl;
                InputManager::GetInstance().connectedGamepadID = i;
                break;
            }
        }
    }

	std::unordered_map<int, bool> keyStates;
	std::unordered_map<int, bool> mouseButtonStates;
	std::unordered_map<int, bool> gamepadButtonStates;

    std::unordered_map<int, bool> prevKeyStates;
    std::unordered_map<int, bool> prevMouseButtonStates;
    std::unordered_map<int, bool> prevGamepadButtonStates;

	double cursorX, cursorY;
	double scrollOffsetX, scrollOffsetY;

    int connectedGamepadID;
    static constexpr int NO_GAMEPAD = -1;
};


#endif // !INPUT_MANAGER_HPP