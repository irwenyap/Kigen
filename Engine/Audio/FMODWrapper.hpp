/*********************************************************************
 * \file	FMODWrapper.hpp
 * \brief	This file defines the FMODWrapper class, which provides a
            high-level interface for managing audio using the FMOD audio engine.
            The wrapper simplifies handling sound playback, 3D audio positioning,
            istener attributes, and volume control within a game or application.
 *
 * \author	wengkhong.thang, 2301372 (wengkhong.thang@digipen.edu)
 * \author	y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \author  wilmerjunrong.lee, 2200691 (wilmerjunrong.lee@digipen.edu)
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#ifndef FMOD_WRAPPER_HPP
#define FMOD_WRAPPER_HPP

#include <string>
#include <unordered_map>

#include "fmod_studio.hpp"
#include "fmod.hpp"

struct Vec3;

/**
 * \class FMODWrapper
 * \brief Provides a low-level wrapper around the FMOD audio engine for managing audio playback.
 *
 * FMODWrapper handles the initialization, updating, and shutdown of the FMOD system.
 * It also provides methods for loading, playing, and stopping sounds with 3D positioning support.
 */
class FMODWrapper {
public:
    /**
     * \brief Constructs the FMODWrapper instance.
     */
    FMODWrapper();

    /**
     * \brief Destructs the FMODWrapper instance and ensures proper cleanup.
     */
    ~FMODWrapper();

    /**
     * \brief Initializes the FMOD systems for use.
     */
    void Initialize();

    /**
     * \brief Updates the FMOD system state, necessary for managing audio playback and 3D positioning.
     */
    void Update();

    /**
     * \brief Shuts down the FMOD systems and releases resources.
     */
    void Shutdown();

    /**
     * \brief Loads a sound file into the FMOD system.
     *
     * \param filePath Path to the sound file.
     * \return A pointer to the loaded FMOD::Sound object.
     */
    FMOD::Sound* LoadSound(const std::string& filePath);

    /**
     * \brief Plays a sound at the specified 3D position with volume and looping options.
     *
     * \param sound Pointer to the FMOD::Sound to be played.
     * \param position 3D position of the sound source.
     * \param volume Volume level (0.0 to 1.0).
     * \param loop Whether the sound should loop.
     * \param clipIdentifier A unique identifier for the sound clip.
     */
    void PlaySound(FMOD::Sound* sound, int entityID, const Vec3& position, float volume, bool loop, const std::string& clipIdentifier, bool isPanning = false);

    /**
     * @brief Sets the stereo panning for a specific audio clip played by an entity.
     *
     * @param entityID The unique identifier of the entity playing the sound.
     * @param clipIdentifier The identifier (name or UUID) of the audio clip.
     * @param pan The stereo panning value (-1.0 = left, 0.0 = center, 1.0 = right).
     */
    void SetPanning(int entityID, const std::string& clipIdentifier, float pan);

    /**
     * \brief Stops playback of a sound identified by the given clip identifier.
     *
     * \param clipIdentifier Unique identifier of the sound clip to stop.
     */
    void StopSound(int EntityID, const std::string& clipIdentifier);

    /**
     * \brief Retrieves the FMOD core system instance.
     *
     * \return Pointer to the FMOD::System object.
     */
    FMOD::System* GetCoreSystem() const { return coreSystem; }

    /**
     * \brief Stores active FMOD channels mapped by their IDs.
     */
     //std::unordered_map<int, FMOD::Channel*> activeChannels;

     /**
      * \brief Maps clip identifiers to their associated channel IDs.
      */
    std::unordered_map<int, std::unordered_map<std::string, FMOD::Channel*>> entityChannelMap;

    /**
    * \brief Converts a Vec3 to an FMOD_VECTOR for 3D audio positioning.
    *
    * \param position The Vec3 to convert.
    * \return The corresponding FMOD_VECTOR.
    */
    FMOD_VECTOR ConvertToFMODVector(const Vec3& position);

    /**
     * \Function is used to update the sound so that the volume can be decreased/increased using spatial audio
     *
     * \param std::string& clipIdentifier : Path to the audio file
     * \param float volume : float of volume to be adjusted
     */
    void UpdateSoundVolume(int entityID, const std::string& clipIdentifier, float volume);

    bool GetCurrentVolume(int entityID, const std::string& clipIdentifier, float& volume);

    void SetGlobalVolume(float volume);
    /**
    * \Function is used to update the position of the listener (Player) so that it can be decreased/increased
       using spatial audio
    * \param FMOD_Vector: position, velocity, forwards and Upwards vector
    */
    void SetListenerAttributes(const FMOD_VECTOR& position, const FMOD_VECTOR velocity, const FMOD_VECTOR& forwards, const FMOD_VECTOR up);

private:
    FMOD::ChannelGroup* masterGroup = nullptr;

    FMOD::Studio::System* studioSystem = nullptr; ///< Pointer to the FMOD Studio system.
    FMOD::System* coreSystem = nullptr; ///< Pointer to the FMOD core system.

    int channelIdCounter = 0; ///< Counter to assign unique IDs to channels.
};

#endif // !FMOD_WRAPPER_HPP
