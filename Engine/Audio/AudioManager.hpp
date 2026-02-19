/*********************************************************************
 * \file	AudioManager.hpp
 * \brief	Defines the AudioManager class for high-level audio management.
 *
 * The AudioManager wraps functionality from the FMOD audio engine, providing
 * an easy-to-use interface for loading and playing sounds, managing 3D audio,
 * and controlling playback state.
 *
 * \author	wengkhong.thang, 2301372 (wengkhong.thang@digipen.edu)
 * \author	y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#ifndef _AUDIO_MANAGER_H
#define _AUDIO_MANAGER_H

#include "FMODWrapper.hpp"

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "Vec3.hpp"
#include <map>
#include <string>
#include "../ECS/ECSManager.hpp"
#include "../ECS/Component.hpp"
 /**
  * \class AudioManager
  * \brief Provides a high-level interface for managing audio in a game or application.
  *
  * The AudioManager serves as a singleton for managing all audio operations, including
  * loading and playing sounds, controlling playback, and managing 3D audio positioning.
  */
class AudioManager
{
public:


    /**
     * \brief Retrieves the singleton instance of AudioManager.
     *
     * \return Reference to the AudioManager instance.
     */
    static AudioManager& GetInstance();

    /**
     * \brief Initializes the audio system.
     */
    void Initialize();

    /**
     * \brief Updates the audio system, ensuring smooth playback and 3D positioning.
     */
    void Update();

    /**
     * \brief Shuts down the audio system and releases resources.
     */
    void Shutdown();

    /**
     * \brief Updates the volume of the string accordingly
     */
    void UpdateSoundVolume(Entity entity, float volume);
    void UpdateSoundVolume(const std::string& clipUUID, float volume);
    /**
     * \brief updates the position of the listener (Player) to FMOD
     */
    void UpdateListener(const Vec3& position, const Vec3& Velocity, const Vec3& forwards, const Vec3& up);
    /**
     * \brief Loads a sound file into the audio system.
     *
     * \param filePath Path to the sound file.
     * \return A pointer to the loaded FMOD::Sound object.
     */
    FMOD::Sound* LoadSound(const std::string& filePath);

    /**
     * \brief Plays an audio clip with optional 3D positioning and looping.
     *
     * \param clipName Name of the clip to play.
     * \param position 3D position of the sound (default is origin).
     * \param volume Volume level (default is 1.0).
     * \param loop Whether the sound should loop (default is false).
     */
    void PlayClip(Entity entity, const std::string& clipName, const Vec3& position = Vec3(0.f, 0.f, 0.f), float volume = 1.0f, bool loop = false , bool isPanning = false);

    void SetPanning(Entity entity, const std::string& clipUUID, float panning);

    /**
     * \brief Checks if a specific audio clip is currently playing.
     *
     * \param clipIdentifier Unique identifier for the audio clip.
     * \return True if the clip is playing, false otherwise.
     */
    bool ClipIsPlaying(Entity entity, const std::string& clipUUID);
    bool ClipIsPlaying(const std::string& clipIdentifier);
    /**
     * \brief Stops playback of a specific audio clip.
     *
     * \param clipIdentifier Unique identifier for the audio clip to stop.
     */
    void StopClip(Entity entity);  // Stop a sound for a specific entity
    void StopClip(const std::string& clipUUID);
    void StopClip(Entity entity, const std::string& clipUUID);
    /**
     * \brief Pauses all currently playing audio clips.
     */
    void PauseAll();

    /**
     * \brief Resumes all paused audio clips.
     */
    void ResumeAll();

    void StopAll();

    void DuckAllExcept(int entityID, const std::string& clipIdentifier, float duckVolume);
    void RestoreAllVolumes();
    bool GetCurrentVolume(int entityID, const std::string& clipIdentifier, float& volume);
    void SetGlobalVolume(float volume);
    std::vector<Entity>GetEntitiesPlayingSound(const std::string& clipUUID);

    struct pair_hash {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2>& p) const {
            return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second);
        }
    };

private:
    AudioManager() = default; // Private constructor to enforce singleton pattern.
    ~AudioManager() = default; // Private destructor to enforce singleton pattern.

    std::unique_ptr<FMODWrapper> audioBackend; ///< Backend wrapper for FMOD functionality.

    std::unordered_map<std::string, std::vector<Entity>> activeAudioEntities;

    std::unordered_map<std::pair <Entity, std::string>, float, pair_hash> originalVol;

};


#endif // !_AUDIO_MANAGER_H