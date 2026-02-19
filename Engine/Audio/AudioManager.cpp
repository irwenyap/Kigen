/*********************************************************************
 * \file	AudioManager.cpp
 * \brief	Defines a AudioManager class that provides a high-level interface for managing audio in a game or application.
 *			It wraps functionality from the FMOD audio engine and provides methods for loading and playing sounds and events,
 *			controlling volume, managing 3D audio positioning, and handling other audio-related tasks.
 *
 * \author	wengkhong.thang, 2301372 (wengkhong.thang@digipen.edu)
 * \author	y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \author  wilmerjunrong.lee, 2200691 (wilmerjunrong.lee@digipen.edu)
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#include "AudioManager.hpp"

#include <filesystem>

#include "../AssetManager.hpp"
#include "../Utility/MetadataHandler.hpp"
#include "AudioClip.hpp"
#include "../Components/AudioSource.hpp"

AudioManager& AudioManager::GetInstance() {
	static AudioManager am;
	return am;
}

/**
 * @brief Initializes the audio manager and loads audio assets.
 *
 * Sets up the audio backend and recursively scans the assets folder
 * to load audio clips with corresponding metadata.
 */
void AudioManager::Initialize() {
	audioBackend = std::make_unique<FMODWrapper>();
	audioBackend->Initialize();

	namespace fs = std::filesystem;
	std::string assetsFolder = "../Assets";
	for (const auto& entry : fs::recursive_directory_iterator(assetsFolder)) {
		const auto& path = entry.path();

		if (entry.is_regular_file() && (path.extension() == ".ogg" || path.extension() == ".wav")) {
			std::string audioClipPath = path.string();
			std::string metaFilePath = audioClipPath + ".meta";

			std::string audioClipUUID;

			// Check if the meta file exists
			if (fs::exists(metaFilePath)) {
				audioClipUUID = MetadataHandler::ParseUUIDFromMeta(metaFilePath);
			}
			else {
				MetadataHandler::GenerateMetaFile(audioClipPath);
				audioClipUUID = MetadataHandler::ParseUUIDFromMeta(metaFilePath);
			}

			AssetManager::GetInstance().Load<AudioClip>(audioClipUUID);
		}
	}
}

/**
 * @brief Updates the audio system.
 *
 * Processes audio updates for the current frame.
 */
void AudioManager::Update() {

	audioBackend->Update();
}

/**
 * @brief Shuts down the audio manager and releases resources.
 *
 * Cleans up the audio backend before shutting down.
 */
void AudioManager::Shutdown() {
	if (audioBackend) {
		audioBackend->Shutdown();
		audioBackend.reset();
	}
}

/**
 * @brief Loads an audio file into the sound system.
 *
 * @param filePath The path to the audio file.
 * @return A pointer to the loaded FMOD sound.
 */
FMOD::Sound* AudioManager::LoadSound(const std::string& filePath) {
	return audioBackend->LoadSound(filePath);
}

/**
 * @brief Plays an audio clip at a specified position.
 *
 * Retrieves the audio clip by UUID and plays it if available.
 *
 * @param clipUUID The unique identifier of the audio clip.
 * @param position The 3D position where the clip should be played.
 * @param volume The playback volume (0.0 - 1.0).
 * @param loop Whether the clip should loop.
 */
void AudioManager::PlayClip(Entity entity, const std::string& clipUUID, const Vec3& position, float volume, bool loop, bool isPanning) {
	auto clip = AssetManager::GetInstance().Get<AudioClip>(clipUUID);

	if (clip && clip->sound) {
		auto& ecs = ECSManager::GetInstance();

		if (!ecs.HasComponent<AudioSource>(entity)) {
			ecs.AddComponent<AudioSource>(entity, { clipUUID, true, loop });
		}
		else {
			auto& audioSource = ecs.GetComponent<AudioSource>(entity);
			audioSource.audioClipUUID = clipUUID;
			audioSource.isPlaying = true;
			audioSource.isLooping = loop;
		}

		activeAudioEntities[clipUUID].push_back(entity);

		//// Replace std::cout with printf
		//printf("[DEBUG] C++: Entity %u started playing clip %s\n", entity, clipUUID.c_str());
		//fflush(stdout); // Ensure immediate print

		//printf("[DEBUG] C++: All entities playing %s: ", clipUUID.c_str());
		//for (Entity e : activeAudioEntities[clipUUID]) {
		//	printf("%u ", e);
		//}
		//printf("\n");
		//fflush(stdout);

		audioBackend->PlaySound(clip->sound, entity, position, volume, loop, clipUUID, isPanning);
	}
	else {
		//printf("[ERROR] C++: Clip not found - %s\n", clipUUID.c_str());
		//fflush(stdout);
	}
}

/**
 * @brief Sets the stereo panning for a specific audio clip played by an entity.
 *
 * @param entity The entity playing the sound.
 * @param clipUUID The unique identifier (UUID) of the audio clip.
 * @param panning A float value representing the stereo panning (-1.0 = left, 0.0 = center, 1.0 = right).
 */
void AudioManager::SetPanning(Entity entity, const std::string& clipUUID, float panning)
{
	if (audioBackend)
	{
		audioBackend->SetPanning(entity, clipUUID, panning);
	}
}

/**
 * @brief Updates the volume of a playing sound.
 *
 * Adjusts the volume of the specified audio clip.
 *
 * @param clipUUID The unique identifier of the audio clip.
 * @param volume The new volume level (0.0 - 1.0).
 */

void AudioManager::UpdateSoundVolume(const std::string& clipUUID, float volume) {
	auto it = activeAudioEntities.find(clipUUID);
	if (it == activeAudioEntities.end()) {
		//printf("[ERROR] C++: No entities found playing %s\n", clipUUID.c_str());
		return;
	}

	// Copy entities list before iterating (prevents modification while iterating)
	std::vector<Entity> entitiesToUpdate = it->second;

	// Debug log
	//printf("[DEBUG] C++: Updating volume for %zu entities playing %s\n",
		//entitiesToUpdate.size(), clipUUID.c_str());

	// Update volume for each entity
	for (Entity entity : entitiesToUpdate) {
		//printf("[DEBUG] C++: Updating volume for entity %u (Clip: %s) to %.2f\n",
			//entity, clipUUID.c_str(), volume);
		UpdateSoundVolume(entity, volume);
	}
}

/**
 * @brief Updates the volume of a playing sound.
 *
 * Adjusts the volume of the specified audio clip.
 *
 * @param entity The entity ID.
 * @param volume The new volume level (0.0 - 1.0).
 */
void AudioManager::UpdateSoundVolume(Entity entity, float volume) {
	auto& ecs = ECSManager::GetInstance();

	if (!ecs.HasComponent<AudioSource>(entity)) return;

	auto& audioSource = ecs.GetComponent<AudioSource>(entity);
	if (audioSource.audioClipUUID.empty()) return;

	std::pair<Entity, std::string> key = { entity, audioSource.audioClipUUID };

	// Cache original volume if not already stored
	if (originalVol.find(key) == originalVol.end()) {
		originalVol[key] = volume;
	}

	audioBackend->UpdateSoundVolume(entity, audioSource.audioClipUUID, volume);
}




 /**
  * @brief Stops a playing audio clip.
  *
  * @param clipIdentifier The unique identifier of the audio clip.
  */
void AudioManager::StopClip(Entity entity) {
	auto& ecs = ECSManager::GetInstance();

	if (!ecs.HasComponent<AudioSource>(entity)) return;

	auto& audioSource = ecs.GetComponent<AudioSource>(entity);

	if (!audioSource.isPlaying) return;  // Prevent stopping twice

	audioBackend->StopSound(entity, audioSource.audioClipUUID);

	auto it = activeAudioEntities.find(audioSource.audioClipUUID);
	if (it != activeAudioEntities.end()) {
		auto& entityList = it->second;
		entityList.erase(std::remove(entityList.begin(), entityList.end(), entity), entityList.end());

		//if (entityList.empty()) {
		//	activeAudioEntities.erase(it);
		//}
	}

	audioSource.isPlaying = false;
	ecs.RemoveComponent<AudioSource>(entity);

}

/**
 * @brief Stops playback of an audio clip for all entities that are playing it.
 *
 * @param clipUUID The unique identifier (UUID) of the audio clip to stop.
 */
void AudioManager::StopClip(const std::string& clipUUID) {
	if (activeAudioEntities.find(clipUUID) == activeAudioEntities.end()) {
		return;
	}

	// Store all entities playing this sound
	std::vector<Entity> entitiesToStop = activeAudioEntities[clipUUID];

	// Stop each entity playing this sound
	for (Entity entity : entitiesToStop) {
		StopClip(entity);
	}
}

void AudioManager::StopClip(Entity entity, const std::string& clipUUID)
{
	auto& ecs = ECSManager::GetInstance();

	if (!ecs.HasComponent<AudioSource>(entity)) return;
	//auto& audioSource = ecs.GetComponent<AudioSource>(entity);

	// Stop from FMOD
	audioBackend->StopSound(entity, clipUUID);

	// Remove from tracking map
	auto it = activeAudioEntities.find(clipUUID);
	if (it != activeAudioEntities.end()) {
		auto& list = it->second;
		list.erase(std::remove(list.begin(), list.end(), entity), list.end());
	}

	// Remove component
	ecs.RemoveComponent<AudioSource>(entity);
}



/**
 * @brief Pauses all currently playing audio clips.
 *
 * Iterates through all active FMOD channels managed by the audio backend
 * and pauses each one. This is typically used when the game is paused or
 * when entering menus to halt all audio playback without stopping the sounds.
 */
void AudioManager::PauseAll() {
	if (!audioBackend) return;

	for (auto& [entity, soundMap] : audioBackend->entityChannelMap) {
		for (auto& [clipID, channel] : soundMap) {
			if (channel) {
				channel->setPaused(true);
				//std::cout << "[Pause] Paused Entity: " << entity << " | Clip: " << clipID << std::endl;
			}
		}
	}
}

/**
 * @brief Resumes all previously paused audio clips.
 *
 * Iterates through all active FMOD channels managed by the audio backend
 * and resumes playback for each one. This is typically used when resuming
 * gameplay after a pause or returning from menus.
 */
void AudioManager::ResumeAll() {
	if (!audioBackend) return;

	for (auto& [entity, soundMap] : audioBackend->entityChannelMap) {
		for (auto& [clipID, channel] : soundMap) {
			if (channel) {
				channel->setPaused(false);
				//std::cout << "[Resume] Resumed Entity: " << entity << " | Clip: " << clipID << std::endl;
			}
		}
	}
}


/**
 * @brief Stops playback of all currently playing audio clips.
 */
void AudioManager::StopAll() {
	for (auto& [clipUUID, entity] : activeAudioEntities) {
		StopClip(clipUUID);
	}
	activeAudioEntities.clear();  // Remove all tracking
}

void AudioManager::DuckAllExcept(int entityID, const std::string& clipIdentifier, float duckVolume)
{
	for (const auto& [clip, entities] : activeAudioEntities)
	{
		for (Entity e : entities)
		{
			// Skip the active playing clip
			if (e == static_cast<Entity>(entityID) && clip == clipIdentifier)
				continue;

			std::pair<Entity, std::string> key = { e, clip };

			if (originalVol.find(key) == originalVol.end())
			{
				float vol = 1.0f;
				if (audioBackend->GetCurrentVolume(e, clip, vol))
					originalVol[key] = vol;
				else
					continue;
			}

			audioBackend->UpdateSoundVolume(e, clip, duckVolume);

			std::cout << "[Duck] Volume lowered for Entity: " << e << " | Clip: " << clip << std::endl;
		}
	}
}


void AudioManager::RestoreAllVolumes()
{
	for (const auto& [key, volume] : originalVol)
	{
		Entity e = key.first;
		const std::string& clip = key.second;

		audioBackend->UpdateSoundVolume(e, clip, volume);
		std::cout << "[Restore] Volume restored: Entity " << e << " | Clip " << clip << " -> " << volume << std::endl;
	}

	originalVol.clear();
}

void AudioManager::SetGlobalVolume(float volume)
{
	if (audioBackend)
	{
		audioBackend->SetGlobalVolume(volume);
	}
}

/**
 * \brief Checks if a specific audio clip is currently playing.
 *
 * \param clipIdentifier Unique identifier for the audio clip.
 * \return True if the clip is playing, false otherwise.
 */
bool AudioManager::ClipIsPlaying(Entity entity, const std::string& clipIdentifier) {
	auto& ecs = ECSManager::GetInstance();

	// Check if the entity has an AudioSource component
	if (!ecs.HasComponent<AudioSource>(entity)) {
		return false; // Entity is not playing any sound
	}

	auto& audioSource = ecs.GetComponent<AudioSource>(entity);

	// Check if the entity is playing THIS specific sound
	if (audioSource.audioClipUUID == clipIdentifier && audioSource.isPlaying) {
		return true;
	}

	return false;
}

/**
 * @brief Checks if an audio clip is currently playing.
 *
 * @param clipIdentifier The unique identifier of the audio clip.
 * @return True if the clip is playing, false otherwise.
 */
bool AudioManager::ClipIsPlaying(const std::string& clipIdentifier) {
	auto it = activeAudioEntities.find(clipIdentifier);
	return it != activeAudioEntities.end();  // Returns true if at least one entity is playing this sound
}


/**
 * @brief Updates the audio listener's position and orientation.
 *
 * Sets the listener's attributes using the provided position, velocity,
 * forward, and up vectors.
 *
 * @param position The listener's 3D position.
 * @param velocity The listener's velocity for Doppler effects.
 * @param forward The listener's forward direction.
 * @param up The listener's up direction.
 */
void AudioManager::UpdateListener(const Vec3& position, const Vec3& velocity, const Vec3& forward, const Vec3& up) {

	AudioManager& instance = AudioManager::GetInstance(); // Ensure we get the singleton instance

	if (!instance.audioBackend) {
		return; // Check if audioBackend is valid
	}

	FMOD_VECTOR fmodPosition = instance.audioBackend->ConvertToFMODVector(position);
	FMOD_VECTOR fmodVelocity = instance.audioBackend->ConvertToFMODVector(velocity);
	FMOD_VECTOR fmodForward = instance.audioBackend->ConvertToFMODVector(forward);
	FMOD_VECTOR fmodUp = instance.audioBackend->ConvertToFMODVector(up);

	// Call the function on the valid instance
	instance.audioBackend->SetListenerAttributes(fmodPosition, fmodVelocity, fmodForward, fmodUp);
}


std::vector<Entity> AudioManager::GetEntitiesPlayingSound(const std::string& clipUUID) {
	if (activeAudioEntities.find(clipUUID) != activeAudioEntities.end()) {
		return activeAudioEntities[clipUUID];
	}
	return {};  // Return an empty vector if no entity is found
}