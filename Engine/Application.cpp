/*********************************************************************
 * \file		Application.cpp
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
#include <Windows.h>
#include <glad/glad.h>
#include "Application.hpp"
#include "Input/InputManager.hpp"
#include "Logger.hpp"


#include "Scene/SceneManager.hpp"

#include "Timer.hpp"
#include "Utility/EngineConfig.hpp"
#include "Utility/Serializer.hpp"

#include "Tools/Gui.hpp"
#include "Tools/Scripting/ScriptEngine.hpp"

 //Process Time Test
#include "Tools/ProcessTime.hpp"
ProcessTime processTime;
#include "Audio/AudioManager.hpp"
#include "Utility/EngineState.hpp"

#include "EventManager.hpp"
#include "Utility/MetadataHandler.hpp"

// TEMPORARY
static Timer TIMER;
EngineConfig config;
extern EngineState engineState;
extern GameWindowMode gameWindowMode;

extern bool appIsRunning;

Application& Application::GetInstance()
{
	static Application app;
	return app;
}

void Application::Init()
{
	// Redirect std::cout to Logger
	m_streamRedirector = new StreamRedirector(std::cout);

	MetadataHandler::InitializeUUIDMap("../Assets");
	appIsRunning = true;
	TIMER.Start();

	Serializer::GetInstance().LoadEngineConfig(config);
	m_context = std::make_unique<Window>(config.windowTitle, config.windowWidth, config.windowHeight, config.isFullscreen);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		//std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	GraphicsManager::GetInstance().SetInternalFormat(config.graphicsQuality);

	ScriptEngine::Init();
	processTime.CreateDelta();

	// Register focus change callback
	glfwSetWindowFocusCallback(m_context->GetWindow(),
		[](GLFWwindow* window, int focused) {
			Application::GetInstance().HandleFocusChange(window, focused);
		});

#ifndef INSTALLER
	Gui::Init(m_context->GetWindow());
#endif
	InputManager::GetInstance().Initialise(m_context->GetWindow());
	AudioManager::GetInstance().Initialize();

	SceneManager::GetInstance().LoadScene(config.sceneName);
	//SceneManager::GetInstance().LoadScene("../Assets/Scenes/Main Menu.scene");
#ifdef INSTALLER
	engineState = EngineState::PLAYING;
#endif
}

void Application::Run()
{
	while (!glfwWindowShouldClose(m_context->GetWindow())) {


#ifdef INSTALLER
		// Skip updates when window is not focused
		if (!glfwGetWindowAttrib(m_context->GetWindow(), GLFW_FOCUSED)) {
			glfwPollEvents();
			continue;
		}
#endif
		if (!appIsRunning) {
			break;
		}
		TIMER.Update();
		SceneManager::GetInstance().UpdateScene(TIMER.GetDeltaTime(), TIMER.GetFixedDT(), TIMER.GetNumOfSteps());
		//glfwSetWindowTitle(m_context.get()->GetWindow(), std::to_string(TIMER.GetFPS()).c_str());
#ifndef INSTALLER
		ExecuteMainThreadQueue();
		processTime.StartDelta();//audio start timer

		processTime.EndDelta(ProcessTime::AUDIO);//audio end timer
		processTime.deltaTime.Update();//delta stop timer
		ProcessTime::CalculatePercentageTime();//calculate percentage and prints
		ProcessTime::UpdateTime();//fps calculator, WIP
		if (gameWindowMode == GameWindowMode::ENGINE)
			Gui::Update(static_cast<int>(GraphicsManager::GetInstance().frameBuffers[0].frameTexture->id));
#endif // INSTALLER

		m_context->SwapBuffers();
		InputManager::GetInstance().Update();
		EventManager::GetInstance().ProcessEvents();
	}

	SceneManager::GetInstance().ExitScene();
}

void Application::Exit()
{
	delete m_streamRedirector;
	m_streamRedirector = nullptr;

#ifndef INSTALLER
	Gui::Exit();
#endif
	ScriptEngine::Shutdown();
	glfwTerminate();
}

std::pair<int, int> Application::GetWindowSize()
{
	static int lastWidth = 1920, lastHeight = 1080;

	Application& app = Application::GetInstance();
	int width, height;
	glfwGetWindowSize(app.m_context->GetWindow(), &width, &height);

	if (width > 0 && height > 0) {
		lastWidth = width;
		lastHeight = height;
	}

	// Returns the last valid window size
	// When window is minimized, the width and height are 0, which will cause issues
	return std::make_pair(lastWidth, lastHeight);
}

// Focus change callback
void Application::HandleFocusChange(GLFWwindow* window, int focused) {
#ifdef INSTALLER
	if (!focused) {
		// Minimize the application when focus is lost
		glfwIconifyWindow(window);
		Logger::Instance().Log(Logger::Level::INFO, "Window lost focus, minimizing...");
		AudioManager::GetInstance().PauseAll();
	} else {

		Logger::Instance().Log(Logger::Level::INFO, "Window regained focus.");
		AudioManager::GetInstance().ResumeAll();
	}
#else
	window; focused;
#endif
}

void Application::SubmitToMainThread(const std::function<void()>& function)
{
	std::scoped_lock<std::mutex> lock(mMainThreadQueueMutex); //locks fn, prevents simultaneous calling

	mMainThreadQueue.emplace_back(function); //fifo
}

std::unique_ptr<Window>& Application::GetWindow()
{
	return m_context;
}

void Application::ExecuteMainThreadQueue()
{
	std::scoped_lock<std::mutex> lock(mMainThreadQueueMutex);

	for (auto& func : mMainThreadQueue)
		func();

	mMainThreadQueue.clear();
}

GLenum Application::GetInternalFormat() const
{
	return GraphicsManager::GetInstance().GetInternalFormat();
}

void Application::HideCursor() const {
	glfwSetInputMode(m_context->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Application::UnhideCursor() const {
	glfwSetInputMode(m_context->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
