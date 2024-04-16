#include <Windows.h>
#include "InputManager.hpp"

InputManager& InputManager::GetInstance()
{
	static InputManager inputManager;
	return inputManager;
}

bool InputManager::GetKeyPressed(unsigned short key)
{
	return ((GetAsyncKeyState(key) & 0x8001) != 0);
}