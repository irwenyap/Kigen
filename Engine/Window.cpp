#include "Logger.hpp"
#include "Window.hpp"

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
	glViewport(0, 0, width, height);
}

Window::Window(const char* title, int width, int height)
	: m_window(nullptr, glfwDestroyWindow)
{
	if (!glfwInit()) {
		Logger::GetInstance().Log(Logger::Level::ERR, "Failed to initialize GLFW");
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window.reset(glfwCreateWindow(width, height, title, nullptr, nullptr));

	if (!m_window) {
		Logger::GetInstance().Log(Logger::Level::ERR, "Failed to create GLFW window");
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(m_window.get());

	glfwSetFramebufferSizeCallback(m_window.get(), framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(m_window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

GLFWwindow* Window::GetWindow() const
{
	return m_window.get();
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(m_window.get());
}
