/*********************************************************************
 * \file		InputManager.cpp
 * \brief		Handles keyboard, mouse and gamepad input
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \author		woonli.wong, 2301308 (woonli.wong@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#include "InputManager.hpp"

InputManager& InputManager::GetInstance()
{
	static InputManager inputManager;
	return inputManager;
}

void InputManager::Initialise(GLFWwindow* window)
{
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPositionCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetJoystickCallback(GamepadCallback);
	InitialiseGamepads();
}

void InputManager::Update()
{
	prevKeyStates = keyStates;
	prevMouseButtonStates = mouseButtonStates;
	prevGamepadButtonStates = gamepadButtonStates;

	glfwPollEvents();

	GLFWgamepadstate state;
	if (glfwGetGamepadState(InputManager::GetInstance().connectedGamepadID, &state)) {
		for (auto& button : gamepadButtonStates) {
			button.second = state.buttons[button.first];
		}
	}
}

bool InputManager::GetKeyDown(int key)
{
	return keyStates[key] && !prevKeyStates[key];
}

bool InputManager::GetKey(int key)
{
	auto it = keyStates.find(key);
	return it != keyStates.end() && it->second;
}

bool InputManager::GetMouseDown(int button)
{
	return mouseButtonStates[button] && !prevMouseButtonStates[button];
}

bool InputManager::GetMouse(int button)
{
	auto it = mouseButtonStates.find(button);
	return it != mouseButtonStates.end() && it->second;
}

double InputManager::GetMouseX() const
{
	return cursorX;
}

double InputManager::GetMouseY() const
{
	return cursorY;
}

bool InputManager::IsGamepadConnected()
{
	return InputManager::GetInstance().connectedGamepadID != NO_GAMEPAD;
}

Vec2 InputManager::GetGamepadLeftStick()
{
	GLFWgamepadstate state;
	if (glfwGetGamepadState(InputManager::GetInstance().connectedGamepadID, &state)) {
		return Vec2(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
	}
	
	return Vec2(0, 0);
}

bool InputManager::IsGamepadButtonDown(int button)
{
	return gamepadButtonStates[button];
}

bool InputManager::IsGamepadButtonPressed(int button)
{
	return gamepadButtonStates[button] && !prevGamepadButtonStates[button];
}

bool InputManager::GetAnyKeyDown() const
{
	for (const auto& mouseButtonState : mouseButtonStates) {
		if (InputManager::GetInstance().GetMouseDown(mouseButtonState.first)) {
			return true;
		}
	}

	if (InputManager::GetInstance().IsGamepadConnected()) {
		for (const auto& buttonState : gamepadButtonStates) {
			if (InputManager::GetInstance().IsGamepadButtonDown(buttonState.first)) {
				return true;
			}
		}
	}
	for (const auto& keyState : keyStates) {
		if (InputManager::GetInstance().GetKeyDown(keyState.first)) {
			return true;
		}
	}

	return false;
}