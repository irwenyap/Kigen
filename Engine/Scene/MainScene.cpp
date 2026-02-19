/*********************************************************************
 * \file		MainScene.cpp
 * \brief		The main scene
 *
 * \author		y.ziyangirwen, 2301345, (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/
#include "MainScene.hpp"

#include <vector>
#include <utility>
#include <iostream>

#include "../ECS/ECSManager.hpp"
#include "Vec.hpp"
#include "../Input/InputManager.hpp"

// Components
#include "../Components/Renderer.hpp"
#include "../Components/Transform.hpp"
#include "../Components/Name.hpp"
#include "../Components/ScriptComponent.hpp"

#include "../EventManager.hpp"

#include "../Utility/Serializer.hpp"
#include "../Tools/Gui.hpp"

#include "../Tools/Panels/HierachyPanel.hpp"
#include "../Utility/EngineState.hpp"

//Scripts
#include "../Tools/Scripting/ScriptEngine.hpp"
#include "../Audio/AudioManager.hpp"

//TSET
#include "../AssetManager.hpp"
#include "../Video/VideoClip.hpp"
#include "../Audio/AudioClip.hpp"
#include "SceneManager.hpp"

extern HierachyPanel hp{};
extern EngineState engineState;
extern GameWindowMode gameWindowMode;

extern bool onStart; //for scripting logic (WIP)
extern bool onSwitch;

void MainScene::Initialize()
{
	auto [width, height] = Application::GetWindowSize();

	GraphicsManager::GetInstance().camera.SetPosition(width / 2.f, height / 2.f);

	hp.Refresh();

	SceneManager& sm = SceneManager::GetInstance();

	ECSManager::GetInstance().transformSystem->Init();
	// Visually updates the loading screen once a major system has been initialized.
	sm.UpdateLoadingScreen((float)(++sm.numSystemsLoaded) * sm.incrementPerSystemLoaded);

	ECSManager::GetInstance().renderSystem->Init();
	sm.UpdateLoadingScreen((float)(++sm.numSystemsLoaded) * sm.incrementPerSystemLoaded);

	ECSManager::GetInstance().uiSystem->Init();
	sm.UpdateLoadingScreen((float)(++sm.numSystemsLoaded) * sm.incrementPerSystemLoaded);

	ECSManager::GetInstance().physicsSystem->Init();
	sm.UpdateLoadingScreen((float)(++sm.numSystemsLoaded) * sm.incrementPerSystemLoaded);

	ECSManager::GetInstance().audioSystem->Init();
	sm.UpdateLoadingScreen((float)(++sm.numSystemsLoaded) * sm.incrementPerSystemLoaded);

	ECSManager::GetInstance().animationSystem->Init();
	sm.UpdateLoadingScreen((float)(++sm.numSystemsLoaded) * sm.incrementPerSystemLoaded);

	ECSManager::GetInstance().videoPlayerSystem->Init();
	sm.UpdateLoadingScreen((float)(++sm.numSystemsLoaded) * sm.incrementPerSystemLoaded);

	ECSManager::GetInstance().stateMachineSystem->Init();
	sm.UpdateLoadingScreen((float)(++sm.numSystemsLoaded) * sm.incrementPerSystemLoaded);

	ECSManager::GetInstance().cameraSystem->Init();
	sm.UpdateLoadingScreen((float)(++sm.numSystemsLoaded) * sm.incrementPerSystemLoaded);
	//glfwMakeContextCurrent(nullptr);

	//scripting test
	ScriptEngine::OnRuntimeStart(&ECSManager::GetInstance(), 
		ECSManager::GetInstance().physicsSystem, 
		ECSManager::GetInstance().renderSystem,
		ECSManager::GetInstance().uiSystem,
		ECSManager::GetInstance().transformSystem,
		ECSManager::GetInstance().animationSystem,
		ECSManager::GetInstance().cameraSystem
	);

	onStart = true;

	//instantiate all entity with script component
	ScriptEngine::PopulateEntityInstance();
	auto view = ECSManager::GetInstance().GetEntityManager();
	for (Entity i{}; i < view.GetEntities(); i++) {
		if (ECSManager::GetInstance().HasComponent<ScriptComponent>(i))
			ScriptEngine::OnCreateEntity(i);
	}

	// Temp location
	for (auto& batch : GraphicsManager::GetInstance().batches) {
		batch.isSorted = false;
		batch.isUpdated = false;
	}

#ifdef INSTALLER
	//auto view = ECSManager::GetInstance().GetEntityManager();
	for (Entity i{}; i < view.GetEntities(); i++) {
		if (ECSManager::GetInstance().HasComponent<ScriptComponent>(i))
			ScriptEngine::OnStartEntity(i); //initial onStart Run
	}
#endif
}

void MainScene::Update(double dt, double fixedDt, int numOfSteps)
{
	auto& ECSManager = ECSManager::GetInstance();
	static bool scriptRunning = false;
	if (engineState == EngineState::PLAYING) {
		if (scriptRunning) ScriptEngine::OnRuntimeStop();
#ifndef INSTALLER
		if (onStart) {
			auto view = ECSManager.GetEntityManager();
			for (Entity i{}; i < view.GetEntities(); i++) {
				if (ECSManager.HasComponent<ScriptComponent>(i))
					ScriptEngine::OnStartEntity(i); //initial onStart Run
			}
			scriptRunning = false;
			onStart = false;
		}
		//if (onStart) onStart = false; //turn off onStart

#endif // !INSTALLER
		
		for (int i = 0; i < numOfSteps; ++i) {
			ECSManager.physicsSystem->Update(fixedDt);
			ECSManager.audioSystem->Update(fixedDt);
		}
		
		ECSManager.videoPlayerSystem->Update(dt);

		//Scripting
		auto view = ECSManager.GetEntityManager();
		for (Entity i{}; i < view.GetEntities(); i++) {
			if (ECSManager.HasComponent<ScriptComponent>(i)) {
				ScriptEngine::OnUpdateEntity(i, static_cast<float>(dt)); // (using script engine to move)
			
				if (onSwitch) {
					onSwitch = false;
					return;
				}
			}
		}
	}

	if (engineState == EngineState::STOPPED) {
		//Scripting
		if (scriptRunning) {
			if (onStart) {
				auto view = ECSManager::GetInstance().GetEntityManager();
				for (Entity i{}; i < view.GetEntities(); i++) {
					if (ECSManager::GetInstance().HasComponent<ScriptComponent>(i))
						ScriptEngine::OnStartEntity(i); //initial onStart Run
				}
				onStart = false;
			}
			//if (onStart) onStart = false; //turn off onStart

			//auto view = ECSManager::GetInstance().GetEntityManager();
			//for (Entity i{}; i < view.GetEntities(); i++) {
			//	if (ECSManager::GetInstance().HasComponent<ScriptComponent>(i)) {
			//		ScriptEngine::OnUpdateEntity(i, (float)dt); // (using script engine to move)
			//	}
			//}
			auto view = ECSManager.GetEntityManager();
			for (Entity i{}; i < view.GetEntities(); i++) {
				if (ECSManager.HasComponent<ScriptComponent>(i)) {
					ScriptEngine::OnUpdateEntity(i, static_cast<float>(dt)); // (using script engine to move)

					if (onSwitch) {
						onSwitch = false;
						return;
					}
				}
			}
		}
#ifndef INSTALLER
		if (InputManager::GetInstance().GetKeyDown(GLFW_KEY_SEMICOLON)) {
			if (scriptRunning) {
				ScriptEngine::OnRuntimeStop();
				AudioManager::GetInstance().StopAll();
			}
			scriptRunning = !scriptRunning;
		}
#endif
	}
	ECSManager.cameraSystem->Update();
	ECSManager.stateMachineSystem->Update(dt);

	//for (int i = 0; i < numOfSteps; ++i) {
		//ECSManager.transformSystem->Update(fixedDt);
	//}
	ECSManager.transformSystem->Update(dt);
	ECSManager.uiSystem->Update(dt);
	ECSManager.renderSystem->Update();

	//for (int i = 0; i < numOfSteps; ++i) {
		ECSManager.animationSystem->Update(dt);
	//}
#ifndef INSTALLER
	if (InputManager::GetInstance().GetKeyDown('K')) {
		ECSManager.renderSystem->SetDebugMode(true);
	} else if (InputManager::GetInstance().GetKeyDown('L')) {
		ECSManager.renderSystem->SetDebugMode(false);
	}
	if (InputManager::GetInstance().GetKeyDown(GLFW_KEY_EQUAL)) {
		GraphicsManager::GetInstance().camera.SetFullScreenProjectionMatrix();
		gameWindowMode = gameWindowMode == GameWindowMode::ENGINE ? GameWindowMode::WINDOWED : GameWindowMode::ENGINE;
	}
#endif
}

void MainScene::Exit()
{
	ScriptEngine::OnRuntimeStop();
	ECSManager::GetInstance().transformSystem->Exit();
	ECSManager::GetInstance().uiSystem->Exit();
	ECSManager::GetInstance().renderSystem->Exit();
	ECSManager::GetInstance().cameraSystem->Exit();

	ECSManager::GetInstance().physicsSystem->Exit();
	ECSManager::GetInstance().audioSystem->Exit();

	ECSManager::GetInstance().animationSystem->Exit();
	ECSManager::GetInstance().stateMachineSystem->Exit();
	
	AssetManager::GetInstance().UnloadAllOfType<Texture>();
	AssetManager::GetInstance().UnloadAllOfType<VideoClip>();
	AssetManager::GetInstance().UnloadAllOfType<AudioClip>();
	AssetManager::GetInstance().UnloadAllOfType<Font>();

	auto& gm = GraphicsManager::GetInstance();

	gm.FreeTextureArrays();

	for (auto& frameBuffer : gm.frameBuffers) {
		frameBuffer.Exit();
	}

	gm.frameBuffers.clear();

	for (size_t i = 0; i < GraphicsManager::FrameBufferIndex::MAX_FRAMEBUFFERS; ++i) {
		gm.frameBuffers.emplace_back();
		gm.frameBuffers[i].frameTexture = AssetManager::GetInstance().CreateTexture("fbo" + std::to_string(i)).get();
	}

	// Initialize the framebuffers
	for (size_t i = 0; i < gm.frameBuffers.size(); ++i) {
		gm.frameBuffers[i].Init();
	}

	// Attach the textures to the framebuffers
	// Note: It is important to reattach the texture after the framebuffer has been initialized
	// as reallocation of the texture array will cause the previously attached texture to be invalid
	for (auto& fb : gm.frameBuffers) {
		fb.Attach();
	}

	SceneManager::GetInstance().ResetLoadingScreen();
}