/*********************************************************************
 * \file		Window.hpp
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

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <memory>
#include <glfw3.h>
#include <string>

 /**
  * \class Window
  * \brief Manages the creation and behavior of the GLFW window used in the application.
  *
  * The Window class is responsible for creating and managing a GLFW window. It also provides methods
  * for handling buffer swapping and updating the window title.
  */
class Window {
public:
	/**
	 * \brief Constructs a Window object with the specified title, width, and height.
	 * \param title The title of the window.
	 * \param width The width of the window in pixels.
	 * \param height The height of the window in pixels.
	 */
	Window(std::string title, int width, int height, bool fullscreen);

	/**
	 * \brief Destructor for the Window class.
	 */
	~Window() = default;

	/**
	 * \brief Retrieves the underlying GLFW window pointer.
	 * \return A pointer to the GLFWwindow.
	 */
	GLFWwindow* GetWindow() const;

	/**
	 * \brief Swaps the front and back buffers of the window, used for rendering.
	 */
	void SwapBuffers();

	/**
	 * \brief Updates the title of the window.
	 * \param title The new title for the window.
	 */
	void SetTitle(const char* title);
private:
	std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> m_window; /**< A unique pointer to the GLFW window. */
	std::string m_title; /**< The title of the window. */
};

#endif // !WINDOW_HPP