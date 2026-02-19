/*********************************************************************
 * \file	FMODWrapper.cpp
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
#include "FMODWrapper.hpp"
#include "Vec.hpp"
#include <iostream>



FMODWrapper::FMODWrapper() = default;

FMODWrapper::~FMODWrapper() { Shutdown(); }

/**
 * @brief Initializes the FMOD audio system.
 *
 * Sets up the FMOD Studio system and its core system for handling audio playback.
 */
//void FMODWrapper::Initialize() {
//    FMOD::Studio::System::create(&studioSystem);
//    studioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, nullptr);
//    studioSystem->getCoreSystem(&coreSystem);
//}
void FMODWrapper::Initialize() {
    // Create the FMOD Studio system
    FMOD::Studio::System::create(&studioSystem);

    // Get the low-level core system
    studioSystem->getCoreSystem(&coreSystem);

    if (coreSystem)
        coreSystem->getMasterChannelGroup(&masterGroup);

    // Initialize FMOD Studio system with default flags
    studioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, nullptr);
}


/**
 * @brief Converts a Vec3 to an FMOD_VECTOR.
 *
 * Maps the x, y, and z components of a Vec3 to an FMOD-compatible format.
 *
 * @param position The Vec3 position to convert.
 * @return The converted FMOD_VECTOR.
 */
FMOD_VECTOR FMODWrapper::ConvertToFMODVector(const Vec3& position) {

    FMOD_VECTOR FVec;
    FVec.x = position.x;
    FVec.y = position.y;
    FVec.z = position.z;

    return FVec;
}

/**
 * @brief Sets the 3D listener attributes for spatial audio.
 *
 * Updates the listener's position, velocity, and orientation in the audio system.
 *
 * @param position The listener's position in 3D space.
 * @param velocity The listener's velocity for Doppler effects.
 * @param forward The listener's forward direction.
 * @param up The listener's up direction.
 */
void FMODWrapper::SetListenerAttributes(const FMOD_VECTOR& position, const FMOD_VECTOR velocity, const FMOD_VECTOR& forward, const FMOD_VECTOR up)
{
    if (!coreSystem)
        return;

    FMOD_RESULT result = coreSystem->set3DListenerAttributes(0, &position, &velocity, &forward, &up);

    if (result != FMOD_OK) {
        std::cerr << "FMODWrapper: Failed to set listener attributes - " << result << std::endl;
    }
}

/**
 * @brief Updates the FMOD audio system and manages active channels.
 *
 * Calls the FMOD Studio system update and removes inactive audio channels.
 */
void FMODWrapper::Update() {
    studioSystem->update(); // FMOD's update call

    // Iterate over all entities' active sounds
    for (auto entityIt = entityChannelMap.begin(); entityIt != entityChannelMap.end();) {
        auto& soundMap = entityIt->second;

        for (auto soundIt = soundMap.begin(); soundIt != soundMap.end();) {
            FMOD::Channel* channel = soundIt->second;
            bool isPlaying = false;

            if (channel) {
                channel->isPlaying(&isPlaying);
            }

            if (!isPlaying) {
                soundIt = soundMap.erase(soundIt); // Remove stopped sound
            }
            else {
                ++soundIt;
            }
        }

        // If entity has no more active sounds, remove it from the map
        if (soundMap.empty()) {
            entityIt = entityChannelMap.erase(entityIt);
        }
        else {
            ++entityIt;
        }
    }
}

/**
 * @brief Loads a sound file into the FMOD audio system.
 *
 * Creates and returns an FMOD sound from the given file path.
 *
 * @param filePath The path to the audio file.
 * @return A pointer to the loaded FMOD sound, or nullptr if loading fails.
 */
FMOD::Sound* FMODWrapper::LoadSound(const std::string& filePath) {
    if (!coreSystem) return nullptr;

    FMOD::Sound* sound = nullptr;
    FMOD_RESULT result = coreSystem->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &sound);
    if (result != FMOD_OK) {
        //std::cerr << "FMODWrapper: Failed to create sound - " << filePath << std::endl;
        return nullptr;
    }
    return sound;
}

/**
 * @brief Updates the volume of a playing sound.
 *
 * Adjusts the volume of the specified sound file if it is currently playing.
 *
 * @param filePath The path to the audio file.
 * @param volume The new volume level (0.0 - 1.0).
 */
void FMODWrapper::UpdateSoundVolume(int entityID, const std::string& clipIdentifier, float volume) {
    std::string uniqueID = clipIdentifier + "_" + std::to_string(entityID); // FIX
    auto entityIt = entityChannelMap.find(entityID);
    if (entityIt != entityChannelMap.end()) {
        auto soundIt = entityIt->second.find(uniqueID);
        if (soundIt != entityIt->second.end()) {
            soundIt->second->setVolume(volume);
        }
    }
}

bool FMODWrapper::GetCurrentVolume(int entityID, const std::string& clipIdentifier, float& volume)
{
    std::string uniqueID = clipIdentifier + "_" + std::to_string(entityID);
    auto entityIt = entityChannelMap.find(entityID);
    if (entityIt != entityChannelMap.end()) {
        auto& soundMap = entityIt->second;
        auto soundIt = soundMap.find(uniqueID);
        if (soundIt != soundMap.end()) {
            FMOD::Channel* channel = soundIt->second;
            if (channel && channel->getVolume(&volume) == FMOD_OK) {
                return true;
            }
        }
    }
    return false;
}

void FMODWrapper::SetGlobalVolume(float volume)
{
    if (masterGroup)
    {
        masterGroup->setVolume(volume);
        std::cout << "[FMODWrapper] Set global volume to " << volume << std::endl;
    }
}


/**
 * @brief Plays a sound at a specified position.
 *
 * Ensures the sound is not already playing, then plays it with the given
 * volume and looping settings.
 *
 * @param sound Pointer to the FMOD sound to play.
 * @param position The 3D position where the sound should be played.
 * @param volume The playback volume (0.0 - 1.0).
 * @param loop Whether the sound should loop.
 * @param clipIdentifier A unique identifier for the sound clip.
 */
void FMODWrapper::PlaySound(FMOD::Sound* sound, int entityID, const Vec3&, float volume, bool loop, const std::string& clipIdentifier, bool isSpatial)
{
    if (!coreSystem) return;

    std::string uniqueID = clipIdentifier + "_" + std::to_string(entityID);
    auto& entitySounds = entityChannelMap[entityID];

    // Avoid duplicate playback
    if (entitySounds.find(uniqueID) != entitySounds.end()) {
        FMOD::Channel* existingChannel = entitySounds[uniqueID];
        bool isPlaying = false;
        existingChannel->isPlaying(&isPlaying);
        if (isPlaying) {
            std::cout << "[DEBUG C++] Already playing: " << uniqueID << std::endl;
            return;
        }
    }

    // Play the sound (paused)
    FMOD::Channel* channel = nullptr;
    if (coreSystem->playSound(sound, nullptr, true, &channel) != FMOD_OK || !channel) return;

    // Apply loop mode to the *sound*
    channel->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

    if (isSpatial) {
        // Treat as 2D + simulate stereo manually
        channel->setMode(FMOD_2D);  // Disable real 3D spatialization
    }
    else {
        channel->setMode(FMOD_2D);  // Standard stereo mode
    }

    // Set volume and unpause
    channel->setVolume(volume);
    channel->setPaused(false);
    

    entitySounds[uniqueID] = channel;
}


/**
 * @brief Sets the stereo panning for a specific audio clip played by an entity.
 *
 * @param entityID The unique identifier of the entity playing the sound.
 * @param clipIdentifier The identifier (name or UUID) of the audio clip.
 * @param pan The stereo panning value (-1.0 = left, 0.0 = center, 1.0 = right).
 */

void FMODWrapper::SetPanning(int entityID, const std::string& clipIdentifier, float pan)
{
    std::string uniqueID = clipIdentifier + "_" + std::to_string(entityID);

    auto entityIt = entityChannelMap.find(entityID);
    if (entityIt == entityChannelMap.end()) return;

    auto& entitySounds = entityIt->second;
    auto soundIt = entitySounds.find(uniqueID);
    if (soundIt == entitySounds.end()) return;

    FMOD::Channel* channel = soundIt->second;
    if (!channel) return;

    // Clamp pan value
    pan = std::max(-1.0f, std::min(1.0f, pan));

    // Force 2D mode to ensure matrix applies correctly
    channel->setMode(FMOD_2D);

    // Get input channel count
    FMOD::Sound* sound = nullptr;
    if (channel->getCurrentSound(&sound) != FMOD_OK || !sound) return;

    int inputChannels = 0;
    sound->getFormat(nullptr, nullptr, &inputChannels, nullptr);

    float left = (1.0f - pan) * 0.5f;
    float right = (1.0f + pan) * 0.5f;

    // Optional: eliminate very low volume bleed
    if (left < 0.01f) left = 0.0f;
    if (right < 0.01f) right = 0.0f;

    FMOD_RESULT result;

    if (inputChannels == 1)
    {
        // Mono → Stereo (input: 1, output: 2)
        float matrix[2] = {
            left, right
        };
        result = channel->setMixMatrix(matrix, 2, 1);
    }
    else if (inputChannels == 2)
    {
        // Stereo → Stereo (input: 2, output: 2)
        float matrix[4] = {
            left,  0.0f,  // Left input → Left output
            0.0f,  right  // Right input → Right output
        };
        result = channel->setMixMatrix(matrix, 2, 2);
    }
}


/**
 * @brief Stops a playing sound.
 *
 * Stops playback of a sound associated with the given identifier and cleans up resources.
 *
 * @param clipIdentifier The unique identifier of the sound clip.
 */
void FMODWrapper::StopSound(int entityID, const std::string& clipIdentifier)
{
    std::string uniqueID = clipIdentifier + "_" + std::to_string(entityID); // ← IMPORTANT

    auto entityIt = entityChannelMap.find(entityID);
    if (entityIt != entityChannelMap.end()) {
        auto soundIt = entityIt->second.find(uniqueID);
        if (soundIt != entityIt->second.end()) {
            FMOD::Channel* channel = soundIt->second;
            bool isPlaying = false;
            channel->isPlaying(&isPlaying);

            if (isPlaying) {
                channel->stop();
                std::cout << "Stopped sound: " << uniqueID << std::endl;
            }

            entityIt->second.erase(soundIt);
            if (entityIt->second.empty()) {
                entityChannelMap.erase(entityIt);
            }
        }
    }
}



/**
 * @brief Shuts down the FMOD audio system.
 *
 * Unloads all loaded audio assets and releases system resources.
 */
void FMODWrapper::Shutdown() {
    if (studioSystem) {
        studioSystem->unloadAll();
        studioSystem->release();
    }
}