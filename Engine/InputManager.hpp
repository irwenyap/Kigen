#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

class InputManager {
public:
	static InputManager& GetInstance();

	static bool GetKeyPressed(unsigned short key);
};


#endif // !INPUT_MANAGER_HPP