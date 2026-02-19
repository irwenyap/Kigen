/*********************************************************************
 * \file		VideoPlayerSystem.hpp
 * \brief		System to handle all the VideoPlayer components
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		30 January 2025
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#include "VideoPlayerSystem.hpp"

#include "../ECS/ECSManager.hpp"
#include "../Components/VideoPlayer.hpp"
#include "../Components/Renderer.hpp"
#include "../Graphics/GraphicsManager.hpp"
#include "../AssetManager.hpp"

void VideoPlayerSystem::Init()
{
	for (auto const& entity : m_entities) {
		auto& renderer = ECSManager::GetInstance().GetComponent<Renderer>(entity);
		auto& videoPlayer = ECSManager::GetInstance().GetComponent<VideoPlayer>(entity);

		videoPlayer.meshID = renderer.currentMeshID;
		if (videoPlayer.videoClipUUID != "")
			videoPlayer.videoClip = *AssetManager::GetInstance().Get<VideoClip>(videoPlayer.videoClipUUID);
	}
}

//static double timer = 0.0;
void VideoPlayerSystem::Update(double dt)
{
	for (auto& entity : m_entities) {
		if (!ECSManager::GetInstance().GetEntityManager().GetActive(entity)) continue;

		auto& videoPlayer = ECSManager::GetInstance().GetComponent<VideoPlayer>(entity);
		
		if (videoPlayer.isPlaying) {
			videoPlayer.timer += dt;

			if (videoPlayer.timer >= 0.0334) {
				videoPlayer.timer -= 0.0334;

				videoPlayer.currentFrame++;
				static size_t totalFrames = videoPlayer.videoClip.texLayerEndIndex - videoPlayer.videoClip.texLayerStartIndex + 1;
				if (videoPlayer.currentFrame >= totalFrames) {
					videoPlayer.currentFrame = 0;
					if (!videoPlayer.isLooping) 
						videoPlayer.isPlaying = false;
				}

				GraphicsManager::GetInstance().SetTextureToMesh(videoPlayer.meshID,
					static_cast<int>(videoPlayer.videoClip.texArrayIndex),
					static_cast<int>(videoPlayer.videoClip.texLayerStartIndex + videoPlayer.currentFrame));
			}
		}
	}
}

void VideoPlayerSystem::Exit()
{
}
