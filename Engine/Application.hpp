/*********************************************************************
 * \file		Application.hpp
 * \brief		Entry point of the application
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Graphics/Window.hpp"
#include "StreamRedirector.hpp"
#include <utility>
#include <vector>
#include <mutex>
#include <functional>

 /**
  * \class Application
  * \brief Manages the main lifecycle of the application, including initialization, execution, and exit.
  *
  * The Application class is implemented as a singleton and provides methods for initializing the
  * application, running the main loop, and exiting the program.
  */
class Application {
public:
	/**
	 * \brief Retrieves the singleton instance of the Application class.
	 * \return A reference to the Application instance.
	 */
	static Application& GetInstance();

	/**
	 * \brief Initializes the application window with a specified title, width, and height.
	 */
	void Init();

	/**
	 * \brief Starts the main loop of the application.
	 */
	void Run();

	/**
	 * \brief Exits the application and performs any necessary cleanup.
	 */
	void Exit();

	/**
	 * \brief Retrieves the current size of the application window.
	 * \return A pair representing the width and height of the window.
	 */
	static std::pair<int, int> GetWindowSize();

	/**
	* \brief Adds a function to the main thread execution queue.
	* \param function A callable function to be executed on the main thread.
	*/
	void SubmitToMainThread(const std::function<void()>& function);

	std::unique_ptr<Window>& GetWindow();

	GLenum GetInternalFormat() const;
	void HideCursor() const;
	void UnhideCursor() const;
private:
	/**
	* \struct ChannelState
	* \brief Represents the state of a channel, including its unique identifier and current position.
	*/
	struct ChannelState {
		int channelId;
		unsigned int position;
	};

	std::vector<ChannelState> pausedChannels; /**< Stores the state of paused channels, including ID and position. */

	std::unique_ptr<Window> m_context; /**< A unique pointer to the Window instance representing the application window. */

	std::vector<std::function<void()>> mMainThreadQueue; /**< Queue of functions to be executed on the main thread. */
	std::mutex mMainThreadQueueMutex; /**< Mutex to synchronize access to the main thread queue. */

	StreamRedirector* m_streamRedirector = nullptr; /** <For redirecting std::cout. */

	Application() = default; /**< Private default constructor for the singleton pattern. */
	~Application() = default; /**< Default destructor. */

	/**
	* \brief Handles changes in the focus state of the application window.
	* \param window The GLFW window that triggered the callback.
	* \param focused Focus state of the window, where 1 means focused and 0 means not focused.
	*/
	void HandleFocusChange(GLFWwindow* window, int focused);

	/**
	* \brief Executes all functions queued for execution on the main thread.
	*/
	void ExecuteMainThreadQueue();

};

#endif // !APPLICATION_HPP