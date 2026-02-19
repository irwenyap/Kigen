/*********************************************************************
 * \file	TransformSystem.cpp
 * \brief
 *      This file defines the TransformSystem class.
 *
 *		The TransformSystem class is responsible for updating the
 *		position, rotation, and scale of entities. It also updates
 *		each entity's world matrix.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	26 November 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#include "TransformSystem.hpp"

#include "../ECS/ECSManager.hpp"
#include "../Components/Transform.hpp"
#include "../Components/Renderer.hpp"
#include "../Components/Camera.hpp"

std::unordered_map<uint32_t, uint32_t> TransformSystem::uuidToTransformMap;

void TransformSystem::Init() 
{
	for (auto& entity : m_entities) {
		auto& transformComponent = ECSManager::GetInstance().GetComponent<Transform>(entity);
		if (transformComponent.parentUUID != 0) {
			auto parentIt = uuidToTransformMap.find(transformComponent.parentUUID);
			if (parentIt != uuidToTransformMap.end()) {
				transformComponent.parent = parentIt->second;
				ECSManager::GetInstance().GetComponent<Transform>(parentIt->second).children.push_back(entity);
			} else {
				transformComponent.parentUUID = 0;
			}
		}
	}

	for (auto const& entity : m_entities) {
		auto& transformComponent = ECSManager::GetInstance().GetComponent<Transform>(entity);

		// Maybe set the initial values of the transform component here

		// Note: Although rotation is saved as a Vec3 in the Transform component, we only use the z value for rotation for now.
		transformComponent.modelToWorldMtx
			= Mat4::BuildTranslation(transformComponent.position)
			* Mat4::BuildZRotation(transformComponent.rotation.z)
			* Mat4::BuildScaling(transformComponent.scale.x, transformComponent.scale.y, transformComponent.scale.z);

		transformComponent.updated = true;
	}
}

void TransformSystem::Update(double)
{
	for (auto const& entity : m_entities) {
		auto& transformComponent = ECSManager::GetInstance().GetComponent<Transform>(entity);

		// Skip over entities that haven't been updated to avoid unnecessary matrix calculations
		if (transformComponent.updated) {
			// Note: Although rotation is saved as a Vec3 in the Transform component, we only use the z value for rotation for now.
			if (transformComponent.parent != MAX_ENTITIES) {
				auto& parent = ECSManager::GetInstance().GetComponent<Transform>(transformComponent.parent);

				transformComponent.modelToWorldMtx
					= parent.modelToWorldMtx
					* Mat4::BuildTranslation(transformComponent.localPosition)
					* Mat4::BuildZRotation(transformComponent.localRotation.z)
					* Mat4::BuildScaling(transformComponent.localScale.x, transformComponent.localScale.y, transformComponent.localScale.z);

				if (!transformComponent.children.empty()) {
					for (auto& child : transformComponent.children) {
						ECSManager::GetInstance().GetComponent<Transform>(child).updated = true;
						//child->updated = true;
					}
				}

				//auto rb = ECSManager::GetInstance().TryGetComponent<Rigidbody2D>(entity);
				//auto collider = ECSManager::GetInstance().TryGetComponent<AABBCollider2D>(entity);
				//if (rb.has_value() && collider.has_value()) {
				//	rb->get().oldPosition = rb->get().position;
				//	//Vec3 localRotatedPos = parent.rotation * transformComponent.localPosition;
				//	rb->get().position = Vec2{ transformComponent.modelToWorldMtx.GetElement(0, 3), transformComponent.modelToWorldMtx.GetElement(1, 3) };

				//	if (!collider->get().isUpdated) {
				//		UpdateAABBCollider(entity);
				//	}
				//}

			} else {
				transformComponent.modelToWorldMtx
					= Mat4::BuildTranslation(transformComponent.position)
					* Mat4::BuildZRotation(transformComponent.rotation.z)
					* Mat4::BuildScaling(transformComponent.scale.x, transformComponent.scale.y, transformComponent.scale.z);
				 
				if (!transformComponent.children.empty()) {
					//int i = 0;
					for (auto& child : transformComponent.children) {
						ECSManager::GetInstance().GetComponent<Transform>(child).updated = true;
						//auto& childT = ECSManager::GetInstance().GetComponent<Transform>(child);
						//childT.updated = true;
						//std::cout << child << std::endl;
						//std::cout << childT.uuid << std::endl;
						//child->updated = true;
					}
				}

			}
			auto rb = ECSManager::GetInstance().TryGetComponent<Rigidbody2D>(entity);
			auto collider = ECSManager::GetInstance().TryGetComponent<AABBCollider2D>(entity);
			if (rb.has_value() && collider.has_value()) {
				rb->get().oldPosition = rb->get().position;
				rb->get().position = Vec2{ transformComponent.position };

				if (!collider->get().isUpdated) {
					UpdateAABBCollider(entity);
				}
			}
			
			auto rendererComponent = ECSManager::GetInstance().TryGetComponent<Renderer>(entity);
			if (rendererComponent.has_value()) rendererComponent->get().isDirty = true;

			transformComponent.updated = false;
		}
		else {
			auto collider = ECSManager::GetInstance().TryGetComponent<AABBCollider2D>(entity);
			if (collider.has_value())
				collider->get().isUpdated = false;
		}
	}

	//for (auto const& entity : m_entities) {
	//	auto collider = ECSManager::GetInstance().TryGetComponent<AABBCollider2D>(entity);
	//	if (collider.has_value()) {
	//		collider->get().isUpdated = false;
	//	}
	//}
}

void TransformSystem::Exit()
{
}

void TransformSystem::SetPosition(Entity entity, Vec3 const& position)
{
	auto& transformComponent = ECSManager::GetInstance().GetComponent<Transform>(entity);
	if (transformComponent.position == position) return; // Skip if the position is the same
	
	if (position.z != transformComponent.position.z) {
		auto& graphicsManager = GraphicsManager::GetInstance();
		auto& rendererComponent = ECSManager::GetInstance().GetComponent<Renderer>(entity);
		size_t& batchID = graphicsManager.meshes[rendererComponent.currentMeshID].batchID;
		if (batchID >= graphicsManager.batches.size()) {
			Logger::Instance().Log(Logger::Level::ERR, "[TransformSystem] SetPosition: Batch ID out of range.");
		}
		else {
			graphicsManager.batches[batchID].isSorted = false;
		}
	}

	transformComponent.position = position;
	transformComponent.updated = true;

	auto camera = ECSManager::GetInstance().TryGetComponent<Camera>(entity);
	if (camera.has_value()) {
		camera->get().hasUpdatedView = true;
	}
}

void TransformSystem::SetRotation(Entity entity, Vec3 const& rotation)
{
	auto& transformComponent = ECSManager::GetInstance().GetComponent<Transform>(entity);
	if (transformComponent.rotation == rotation) return; // Skip if the rotation is the same
	transformComponent.rotation = rotation;
	transformComponent.updated = true;

}

void TransformSystem::SetScale(Entity entity, Vec3 const& scale)
{
	auto& transformComponent = ECSManager::GetInstance().GetComponent<Transform>(entity);
	if (transformComponent.scale == scale) return; // Skip if the scale is the same
	transformComponent.scale = scale;
	transformComponent.updated = true;

}

void TransformSystem::Translate(Entity entity, Vec3 const& translation)
{
	auto& transformComponent = ECSManager::GetInstance().GetComponent<Transform>(entity);
	if (translation == Vec3{ 0.f }) return; // Skip if the translation is zero

	if (translation.z != 0.f) {
		auto& graphicsManager = GraphicsManager::GetInstance();
		auto& rendererComponent = ECSManager::GetInstance().GetComponent<Renderer>(entity);
		size_t& batchID = graphicsManager.meshes[rendererComponent.currentMeshID].batchID;
		if (batchID >= graphicsManager.batches.size()) {
			Logger::Instance().Log(Logger::Level::ERR, "[TransformSystem] Translate: Batch ID out of range.");
		}
		else {
			graphicsManager.batches[batchID].isSorted = false;
		}
	}
	transformComponent.position += translation;
	transformComponent.updated = true;

	auto camera = ECSManager::GetInstance().TryGetComponent<Camera>(entity);
	if (camera.has_value()) {
		camera->get().hasUpdatedView = true;
	}
}

void TransformSystem::Rotate(Entity entity, float const& rotation)
{
	auto& transformComponent = ECSManager::GetInstance().GetComponent<Transform>(entity);
	if (rotation == 0.f) return; // Skip if the rotation is zero
	transformComponent.rotation.z += rotation; // Currently only rotating around the z-axis
	transformComponent.updated = true;
}

void TransformSystem::Scale(Entity entity, float const& scale)
{
	auto& transformComponent = ECSManager::GetInstance().GetComponent<Transform>(entity);
	if (scale == 1.f) return; // Skip if the scale is 1
	transformComponent.scale *= scale;
	transformComponent.updated = true;
}

void TransformSystem::UpdateAABBCollider(Entity entity) {
	// Update the entity's AABBCollider's min and max based on the Rigidbody2D's position.
	auto& rb = ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity);
	auto collider = ECSManager::GetInstance().TryGetComponent<AABBCollider2D>(entity);

	// Update the collider size in case the user resizes it.
	if (collider.has_value()) {
		// If the position of the entity was manually changed
		if (rb.oldPosition != rb.position) {
			// Update the collider's center position based on the new position and the collider's offset.
			collider->get().centerPos = rb.position + collider->get().offset;
			// Update the collider's min and max points based on the collider's updated center position.
			collider->get().min = Vec2{ (collider->get().centerPos.x - (collider->get().sizeX / 2.f)), (collider->get().centerPos.y - (collider->get().sizeY / 2.f)) };
			collider->get().max = Vec2{ (collider->get().centerPos.x + (collider->get().sizeX / 2.f)), (collider->get().centerPos.y + (collider->get().sizeY / 2.f)) };
		}
		// Else if the position of the entity didn't change (either the collider's min/max got manually adjusted, or the rigidbody is stationary).
		else {
			// Update the collider's size in case the collider's min/max got manually adjusted.
			collider->get().sizeX = collider->get().max.x - collider->get().min.x;
			collider->get().sizeY = collider->get().max.y - collider->get().min.y;
			// Calculate the new collider center position and offset based on the new min/max and size.
			CalculateColliderPosOffset(entity);
		}
	}
}

void TransformSystem::CalculateColliderPosOffset(Entity entity) {
	AABBCollider2D& collider = ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity);
	//if (collider.isUpdated) return;

	collider.centerPos = Vec2{ (collider.min.x + collider.sizeX / 2.f),
								(collider.min.y + collider.sizeY / 2.f) };
	auto& transform = ECSManager::GetInstance().GetComponent<Transform>(entity);
	collider.offset = collider.centerPos - Vec2{ transform.position };
}
