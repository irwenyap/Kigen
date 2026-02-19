/*********************************************************************
 * \file		SceneManager.cpp
 * \brief		Handles all scene related manipulation
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		16 November 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#include "SceneManager.hpp"

#include "../Utility/Serializer.hpp"
#include "MainScene.hpp"
#include "../ECS/ECSManager.hpp"
#include "../Components/Name.hpp"
#include "../Components/UI.hpp"

// TEMPORARY
#include "../Utility/EngineState.hpp"

//Scripts
#include "../Tools/Scripting/ScriptEngine.hpp"
#include "../Components/ScriptComponent.hpp"

extern EngineState engineState;
extern bool onStart;
static bool onFirstLoad = true;

SceneManager& SceneManager::GetInstance()
{
	static SceneManager inputManager;
	return inputManager;
}

void SceneManager::LoadScene(const std::string& scenePath) {
#ifndef INSTALLER
    useLoadingScreen = false;
#endif
    // Exit and clean up the current scene first.
    if (currentScene) {
        engineState = EngineState::STOPPED;
        currentScene->Exit();
#ifndef INSTALLER
        Serializer::GetInstance().SerializeScene(currentScenePath);
#endif
        ECSManager::GetInstance().ClearEntities();
        onFirstLoad = false;
    }

    if (scenePath == "../Assets/Scenes/Main Menu.scene") {
        useLoadingScreen = false;
    }
    if (useLoadingScreen) {
#ifdef INSTALLER
        if (!onFirstLoad) {
            // Load in the loading scene.
            Serializer::GetInstance().DeserializeScene("../Assets/Scenes/Loading Screen.scene");
            auto view = ECSManager::GetInstance().GetEntityManager();
            for (Entity i{}; i < view.GetEntities(); i++) {
                loadingScreenEntities.insert(i);
                const auto& enttName = ECSManager::GetInstance().TryGetComponent<Name>(i);
                if (enttName.has_value() && enttName->get().name == "Loading Bar") {
                    loadingBarEntt = i;
                } else if (enttName.has_value() && enttName->get().name == "Fade") {
                    fadeEntt = i;
                }
            }

            ECSManager::GetInstance().renderSystem->Init();
            ECSManager::GetInstance().uiSystem->Init();

            // Render the loading screen with 0% completion.
            UpdateLoadingScreen(0.f);

            numOfSystemsToLoad = ECSManager::GetInstance().GetNumOfSystems();
            incrementPerSystemLoaded = 1.f / static_cast<float>(numOfSystemsToLoad);
            numSystemsLoaded = 0;
        }
#endif

        // Start loading the new scene.
        isLoading = true;

        currentScene = std::make_unique<MainScene>();
        currentScenePath = scenePath;
        Serializer::GetInstance().DeserializeScene(scenePath);

        if (currentScene) {
            currentScene->Initialize();
            if (scenePath != "../Assets/Scenes/Main Menu.scene")
                UpdateLoadingScreen(1.f);

#ifdef INSTALLER
            if (!onFirstLoad && scenePath != "../Assets/Scenes/Main Menu.scene") {
                // Transition from loading scene to new scene using a 'glitch-like' effect.
                float transitionDuration = 0.08f;
                float currTransitionDur = transitionDuration;
                UI& fadeUI = ECSManager::GetInstance().GetComponent<UI>(fadeEntt);
                ECSManager::GetInstance().GetEntityManager().SetActive(fadeEntt, true);
                ECSManager::GetInstance().uiSystem->SetVisibility(fadeEntt, true);
                while (currTransitionDur > 0.f) {
                    fadeUI.isUpdated = false;
                    UpdateLoadingScreen(1.f);
                    currTransitionDur -= 0.02f;
                }

                // move to top right
                fadeUI.position = Vec3{ 0.3f, 0.5f, 0.f };
                currTransitionDur = transitionDuration;
                while (currTransitionDur > 0.f) {
                    fadeUI.isUpdated = false;
                    UpdateLoadingScreen(1.f);
                    currTransitionDur -= 0.02f;
                }

                // move to bottom
                fadeUI.position = Vec3{ 0.f };
                fadeUI.size = Vec2{ 1.f, 0.75f };
                currTransitionDur = transitionDuration;
                while (currTransitionDur > 0.f) {
                    fadeUI.isUpdated = false;
                    UpdateLoadingScreen(1.f);
                    currTransitionDur -= 0.02f;
                }

                // move to top left
                fadeUI.position = Vec3{ 0.f, 0.3f };
                fadeUI.size = Vec2{ 0.75f, 0.75f };
                currTransitionDur = transitionDuration;
                while (currTransitionDur > 0.f) {
                    fadeUI.isUpdated = false;
                    UpdateLoadingScreen(1.f);
                    currTransitionDur -= 0.02f;
                }

                // almost full
                fadeUI.position = Vec3{ 0.03f, 0.f };
                fadeUI.size = Vec2{ 0.97f, 0.97f };
                currTransitionDur = transitionDuration;
                while (currTransitionDur > 0.f) {
                    fadeUI.isUpdated = false;
                    UpdateLoadingScreen(1.f);
                    currTransitionDur -= 0.02f;
                }

                // full
                fadeUI.position = Vec3{ 0.f };
                fadeUI.size = Vec2{ 1.f, 1.f };
                currTransitionDur = transitionDuration;
                while (currTransitionDur > 0.f) {
                    fadeUI.isUpdated = false;
                    UpdateLoadingScreen(1.f);
                    currTransitionDur -= 0.02f;
                }

                for (Entity entity : loadingScreenEntities) {
                    ECSManager::GetInstance().GetEntityManager().SetActive(entity, false);
                    ECSManager::GetInstance().uiSystem->SetVisibility(entity, false);
                }

                engineState = EngineState::PLAYING;
            }
#endif
        }

        isLoading = false;

#ifdef INSTALLER
        // If there is a loading screen for the current scene, all entities are set to invisible initially
        // so that only the loading screen is visible while initializing the rest of the entities.
        // Hence, we must set each entity's visibility to its correct value once the scene is fully initialized.
        ECSManager::GetInstance().renderSystem->UpdateEntitiesVisibility();
#endif
    }
    else {
        // Start loading the new scene.
        isLoading = true;

        currentScene = std::make_unique<MainScene>();
        currentScenePath = scenePath;
        Serializer::GetInstance().DeserializeScene(scenePath);

        if (currentScene) {
            currentScene->Initialize();
        }

        isLoading = false;

#ifdef INSTALLER
        engineState = EngineState::PLAYING;
        ECSManager::GetInstance().renderSystem->UpdateEntitiesVisibility();
#endif
    }

#ifdef INSTALLER
    useLoadingScreen = true;
#endif
}

void SceneManager::UpdateLoadingScreen(float percentDone) {
#ifdef INSTALLER
    if (useLoadingScreen && !onFirstLoad) {
        UI& loadingBar = ECSManager::GetInstance().GetComponent<UI>(loadingBarEntt);
        float fullBarSize = 0.68f;
        loadingBar.size.x = percentDone * fullBarSize;
        loadingBar.isUpdated = false;

        // Manually call the UI & render system update to render the loading screen.
        ECSManager::GetInstance().uiSystem->Update(0.0);
        ECSManager::GetInstance().renderSystem->Update();
        glfwSwapBuffers(Application::GetInstance().GetWindow()->GetWindow());
    }
#else
    percentDone;
#endif
}

void SceneManager::ResetLoadingScreen() {
    loadingScreenEntities.clear();
    loadingBarEntt = static_cast<Entity>(-1);
    fadeEntt = static_cast<Entity>(-1);
}

void SceneManager::UpdateScene(double deltaTime, double fixedDT, int numOfSteps) {
    if (currentScene) {
        currentScene->Update(deltaTime, fixedDT, numOfSteps);
    }
}

void SceneManager::ExitScene() {
    if (currentScene) {
        Serializer::GetInstance().SerializeScene(currentScenePath);
        currentScene->Exit();
        ScriptEngine::OnRuntimeStop();
        currentScene.reset();
    }
}

void SceneManager::SaveScene()
{
    Serializer::GetInstance().SerializeScene(currentScenePath);
}

void SceneManager::SaveTempScene()
{
    std::string tempFile = currentScenePath + ".temp";
    Serializer::GetInstance().SerializeScene(tempFile);
}

void SceneManager::ReloadScene()
{
    ScriptEngine::OnRuntimeStop();

    std::string tempFile = currentScenePath + ".temp";
    if (std::filesystem::exists(tempFile)) {
        Serializer::GetInstance().ReloadScene(tempFile);
    } else {
        // Handle the case where the temp file doesn't exist
        std::cerr << "Temp file does not exist: " << tempFile << std::endl;
        return; // Early exit if needed
    }

    //Serializer::GetInstance().ReloadScene(tempFile);

    //scripting test

    onStart = true;

    ScriptEngine::OnRuntimeStart(&ECSManager::GetInstance(), 
        ECSManager::GetInstance().physicsSystem, 
        ECSManager::GetInstance().renderSystem,
        ECSManager::GetInstance().uiSystem,
        ECSManager::GetInstance().transformSystem,
        ECSManager::GetInstance().animationSystem,
		ECSManager::GetInstance().cameraSystem
    );

    //instantiate all entity with script component
    ScriptEngine::PopulateEntityInstance();
    auto view = ECSManager::GetInstance().GetEntityManager();
    for (Entity i{}; i < view.GetEntities(); i++) {
        if (ECSManager::GetInstance().HasComponent<ScriptComponent>(i))
            ScriptEngine::OnCreateEntity(i);
    }
}
