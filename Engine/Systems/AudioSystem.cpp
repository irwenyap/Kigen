/*********************************************************************
 * \file		AudioSytem.cpp
 * \brief		System to handle all the AudioSource components
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		17 November 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#include "AudioSystem.hpp"

#include "../ECS/ECSManager.hpp"
#include "../Components/AudioSource.hpp"
#include "../Audio/AudioManager.hpp"



void AudioSystem::Init()
{
}

void AudioSystem::Update(double) {
    for (auto entity : m_entities) {
        if (!ECSManager::GetInstance().GetEntityManager().GetActive(entity)) continue;

        auto& audioSource = ECSManager::GetInstance().GetComponent<AudioSource>(entity);

        // Ensure the entity has a valid sound clip
        if (!audioSource.audioClipUUID.empty() && audioSource.isPlaying) {
            // Check if THIS entity's sound is playing
            if (!AudioManager::GetInstance().ClipIsPlaying(entity, audioSource.audioClipUUID)) {
                // Play the sound for this entity
                AudioManager::GetInstance().PlayClip(entity, audioSource.audioClipUUID);

                // If not looping, stop playing after one playthrough
                if (!audioSource.isLooping) audioSource.isPlaying = false;
            }
        }
    }
    AudioManager::GetInstance().Update();
}

void AudioSystem::Exit()
{
}
