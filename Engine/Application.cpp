#include <Windows.h>
#include <glad/glad.h>
#include "Application.hpp"
#include "InputManager.hpp"
#include "Logger.hpp"

//Shortcut to be changed
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
void mouse_callback(GLFWwindow*, double, double) {
	//Application::mouse_current_x = xposIn;
	//Application::mouse_current_y = yposIn;
}

Application& Application::GetInstance()
{
	static Application app;
	return app;
}

void Application::Init(const char* title, int width, int height)
{
	m_context = std::make_unique<Window>(title, width, height);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		//std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	glfwSetCursorPosCallback(m_context->GetWindow(), mouse_callback);
}

void Application::Run()
{
	while (true) {
		if (InputManager::GetInstance().GetKeyPressed('P')) {
			break;
		}
		if (InputManager::GetInstance().GetKeyPressed('Q')) {
			Logger::GetInstance().Log(Logger::Level::DEBUG, "Hello World!");
		}
		if (InputManager::GetInstance().GetKeyPressed('W')) {
			Logger::GetInstance().Log(Logger::Level::INFO, "Hello World!");
		}
		if (InputManager::GetInstance().GetKeyPressed('E')) {
			Logger::GetInstance().Log(Logger::Level::WARN, "Hello World!");
		}
		if (InputManager::GetInstance().GetKeyPressed('R')) {
			Logger::GetInstance().Log(Logger::Level::ERR, "Hello World!");
		}
	}
}

void Application::Exit()
{
	glfwTerminate();
}
