/*********************************************************************
 * \file		PrefabManager.cpp
 * \brief		Manages the creation, updating, and unlinking of 
 *              prefab entities in the ECS system.
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/
#include "PrefabManager.hpp"

#include "../ECS/ECSManager.hpp"

#include "../Engine/Components/Name.hpp"
#include "../Engine/Components/Transform.hpp"
#include "../Engine/Components/Renderer.hpp"
#include "../Engine/Components/Rigidbody2D.hpp"
#include "../Engine/Components/ScriptComponent.hpp"
#include "../Engine/Components/Animation.hpp"
#include "../Engine/Components/AudioSource.hpp"

PrefabManager& PrefabManager::GetInstance()
{
	static PrefabManager pm;
	return pm;
}

void PrefabManager::UpdatePrefab(const std::string& prefabID, const uint32_t& entity) {
    auto& prefabVec = prefabsMap[prefabID];
    if (prefabVec.empty()) return;

    auto& ecs = ECSManager::GetInstance();

    // Cache source entity's components
    bool hasName = ecs.HasComponent<Name>(entity);
    bool hasTransform = ecs.HasComponent<Transform>(entity);
    bool hasRenderer = ecs.HasComponent<Renderer>(entity);
    bool hasRigidbody = ecs.HasComponent<Rigidbody2D>(entity);
    bool hasAnimation = ecs.HasComponent<Animation>(entity);
    bool hasAudioSource = ecs.HasComponent<AudioSource>(entity);
    bool hasScriptComponent = ecs.HasComponent<ScriptComponent>(entity);

    Name* sourceName = hasName ? &ecs.GetComponent<Name>(entity) : nullptr;
    Transform* sourceTransform = hasTransform ? &ecs.GetComponent<Transform>(entity) : nullptr;
    Renderer* sourceRenderer = hasRenderer ? &ecs.GetComponent<Renderer>(entity) : nullptr;
    Rigidbody2D* sourceRigidbody = hasRigidbody ? &ecs.GetComponent<Rigidbody2D>(entity) : nullptr;
    Animation* sourceAnimation = hasAnimation ? &ecs.GetComponent<Animation>(entity) : nullptr;
    AudioSource* sourceAudioSource = hasAudioSource ? &ecs.GetComponent<AudioSource>(entity) : nullptr;
    ScriptComponent* sourceScript = hasScriptComponent ? &ecs.GetComponent<ScriptComponent>(entity) : nullptr;

    for (auto& entt : prefabVec) {
        if (entt == entity) continue;

        // Name
        if (hasName) {
            if (!ecs.HasComponent<Name>(entt)) {
                ecs.AddComponent(entt, *sourceName);
            } else {
                ecs.GetComponent<Name>(entt) = *sourceName;
            }
        }

        // Transform
        if (hasTransform) {
            if (!ecs.HasComponent<Transform>(entt)) {
                ecs.AddComponent(entt, *sourceTransform);
            } else {
                auto& targetTransform = ecs.GetComponent<Transform>(entt);
                targetTransform.scale = sourceTransform->scale;
                targetTransform.rotation = sourceTransform->rotation;
                targetTransform.updated = true;
            }
        } else if (ecs.HasComponent<Transform>(entt)) {
            ecs.RemoveComponent<Transform>(entt);
        }

        // Renderer
        if (hasRenderer) {
            if (!ecs.HasComponent<Renderer>(entt)) {
                ecs.AddComponent(entt, *sourceRenderer);
            } else {
                auto& r = ecs.GetComponent<Renderer>(entt);

                r.uuid = sourceRenderer->uuid;
                r.mesh = sourceRenderer->mesh;
                r.isAnimated = sourceRenderer->isAnimated;
            }
        } else if (ecs.HasComponent<Renderer>(entt)) {
            ecs.RemoveComponent<Renderer>(entt);
        }

        // Rigidbody2D
        if (hasRigidbody) {
            if (!ecs.HasComponent<Rigidbody2D>(entt)) {
                ecs.AddComponent(entt, *sourceRigidbody);
            } else {
                ecs.GetComponent<Rigidbody2D>(entt) = *sourceRigidbody;
            }
        } else if (ecs.HasComponent<Rigidbody2D>(entt)) {
            ecs.RemoveComponent<Rigidbody2D>(entt);
        }

        // Animation
        if (hasAnimation) {
            if (!ecs.HasComponent<Animation>(entt)) {
                ecs.AddComponent(entt, *sourceAnimation);
            } else {
                ecs.GetComponent<Animation>(entt) = *sourceAnimation;
            }
        } else if (ecs.HasComponent<Animation>(entt)) {
            ecs.RemoveComponent<Animation>(entt);
        }

        // AudioSource
        if (hasAudioSource) {
            if (!ecs.HasComponent<AudioSource>(entt)) {
                ecs.AddComponent(entt, *sourceAudioSource);
            } else {
                ecs.GetComponent<AudioSource>(entt) = *sourceAudioSource;
            }
        } else if (ecs.HasComponent<AudioSource>(entt)) {
            ecs.RemoveComponent<AudioSource>(entt);
        }

        // ScriptComponent
        if (hasScriptComponent) {
            if (!ecs.HasComponent<ScriptComponent>(entt)) {
                ecs.AddComponent(entt, *sourceScript);
            } else {
                ecs.GetComponent<ScriptComponent>(entt) = *sourceScript;
            }
        } else if (ecs.HasComponent<ScriptComponent>(entt)) {
            ecs.RemoveComponent<ScriptComponent>(entt);
        }
    }
}

void PrefabManager::UnlinkPrefab(const std::string& prefabID, const uint32_t& entity) {
	auto& prefabVec = prefabsMap[prefabID];

	for (size_t i = 0; i < prefabVec.size(); ++i) {
		if (prefabVec[i] == entity) {
			std::swap(prefabVec[i], prefabVec.back());
			prefabVec.pop_back();
			break;
		}
	}
}
