/*********************************************************************
 * \file		Window.cpp
 * \brief		Handles the window of the application
 *
 * \author		Yap Zi Yang Irwen, y.ziyangirwen, 2301345
 * \email		y.ziyangirwen@digipen.edu
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#include "Logger.hpp"
#include "Window.hpp"

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
	glViewport(0, 0, width, height);
}

Window::Window(std::string title, int width, int height, bool fullscreen):

	m_window(nullptr, glfwDestroyWindow),
	m_title(title)
{
	if (!glfwInit()) {
		Logger::Instance().Log(Logger::Level::ERR, "[Window] Failed to initialize GLFW");
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef INSTALLER
	m_window.reset(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr));
	fullscreen;
#else
	title; width; height;
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	if (!monitor) {
		std::cerr << "Failed to get primary monitor" << std::endl;
		glfwTerminate();
		return;
	}

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	if (!mode) {
		std::cerr << "Failed to get video mode" << std::endl;
		glfwTerminate();
		return;
	}

	if (fullscreen) {
		//m_window.reset(glfwCreateWindow(mode->width, mode->height, "NANO | SpiceBoys", monitor, nullptr));

		int bestWidth = mode->width;
		int bestHeight = mode->height;

		// Get all available video modes
		int count;
		const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);

		for (int i = 0; i < count; i++) {
			const GLFWvidmode& m = modes[i];

			const float aspect = (float)m.width / (float)m.height;
			const float targetAspect = 16.0f / 9.0f;
			const float aspectTolerance = 0.05f;

			if (fabs(aspect - targetAspect) < aspectTolerance &&
				m.width <= mode->width &&
				m.height <= mode->height) {
				// Prefer higher resolutions
				if (m.width * m.height > bestWidth * bestHeight) {
					bestWidth = m.width;
					bestHeight = m.height;
				}
			}
		}

		if (bestWidth == mode->width && bestHeight == mode->height) {
			bestHeight = mode->height;
			bestWidth = (bestHeight / 9) * 16;

			if (bestWidth > mode->width) {
				bestWidth = mode->width;
				bestHeight = (bestWidth * 9) / 16;
			}
		}

		m_window.reset(glfwCreateWindow(bestWidth, bestHeight, "NANO | SpiceBoys", monitor, nullptr));
	}
	else
		m_window.reset(glfwCreateWindow(width, height, "NANO | SpiceBoys", nullptr, nullptr));

#endif

	if (!m_window) {
		Logger::Instance().Log(Logger::Level::ERR, "[Window] Failed to create GLFW window");
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(m_window.get());

	glfwSetFramebufferSizeCallback(m_window.get(), framebuffer_size_callback);
	//glfwSwapInterval(0);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(m_window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

GLFWwindow* Window::GetWindow() const
{
	return m_window.get();
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(m_window.get());
}

void Window::SetTitle(const char* title)
{
	glfwSetWindowTitle(m_window.get(), (m_title + std::string(title)).c_str());
}