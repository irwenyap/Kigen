/*********************************************************************
 * \file		SceneManager.hpp
 * \brief		Handles all scene manipulation
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		16 November 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <set>
#include "Scene.hpp"
#include "../ECS/Entity.hpp"

 /**
  * \class SceneManager
  * \brief Singleton class responsible for managing and manipulating scenes.
  *
  * The SceneManager handles loading, updating, and exiting scenes. It ensures
  * only one scene is active at any given time and provides an interface
  * to interact with the current scene.
  */
class SceneManager {
public:
    /**
     * \brief Get the singleton instance of SceneManager.
     *
     * \return Reference to the singleton SceneManager instance.
     */
    static SceneManager& GetInstance();

    /**
     * \brief Load a scene from the specified file path.
     *
     * This method replaces the currently active scene with a new scene
     * loaded from the given file path.
     *
     * \param scenePath Path to the scene file to be loaded.
     */
    void LoadScene(const std::string& scenePath);

    void UpdateLoadingScreen(float percentDone);

    void ResetLoadingScreen();

    /**
     * \brief Update the currently active scene.
     *
     * This method calls the update logic for the currently active scene,
     * allowing it to process game logic and rendering.
     *
     * \param deltaTime Time elapsed since the last update, in seconds.
     * \param fixedDT Fixed delta time that is used to update time-based systems.
     * \param numOfSteps Number of steps to update time-based systems.
     */
    void UpdateScene(double deltaTime, double fixedDT, int numOfSteps);

    /**
     * \brief Exit the currently active scene.
     *
     * This method ensures proper cleanup of the active scene before
     * transitioning to a new scene or shutting down.
     */
    void ExitScene();

    /**
     * \brief Manual saving of scene
     */
    void SaveScene();

    /**
     * \brief Saves the scene state temporarily
     */
    void SaveTempScene();

    /**
     * \brief Restores the temporary scene state
     */
    void ReloadScene();

    inline std::string& GetCurrentScenePath() { return currentScenePath; }

    // temp for now
    // will move to a loading screen class later
    bool useLoadingScreen = true;
    bool isLoading = false;

    size_t numOfSystemsToLoad{};
    float incrementPerSystemLoaded{};
    size_t numSystemsLoaded{};
    std::set<Entity> loadingScreenEntities{};
    Entity loadingBarEntt = static_cast<Entity>(-1);
    Entity fadeEntt = static_cast<Entity>(-1);

private:
    /**
     * \brief Default constructor.
     *
     * Constructor is private to enforce the singleton pattern.
     */
    SceneManager() = default;

    /**
     * \brief Destructor.
     *
     * Ensures proper cleanup by calling ExitScene() during destruction.
     */
    ~SceneManager() { ExitScene(); }

    std::unique_ptr<IScene> currentScene = nullptr; /*!< Pointer to the currently active scene. */
    std::unique_ptr<IScene> loadingScene = nullptr;
    std::string currentScenePath;                   /*!< Path to the currently loaded scene file. */
};


#endif // !SCENE_MANAGER_HPP