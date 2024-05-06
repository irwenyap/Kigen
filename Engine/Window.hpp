#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <memory>
#include <glfw3.h>

class Window {
public:
	Window(const char* title, int width, int height);
	~Window() = default;

	GLFWwindow* GetWindow() const;
	void SwapBuffers();

private:
	std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> m_window;
};

#endif // !WINDOW_HPP