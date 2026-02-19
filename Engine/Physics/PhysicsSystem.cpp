/*********************************************************************
 * \file		PhysicsSystem.cpp
 * \brief		Contains definitions of the functions of the Physics system
				to simulate gravity, movement, collision detection and
				collision response.

 * \author		Wong Woon Li, woonli.wong, 2301308
 * \email		woonli.wong@digipen.edu
 * \date		1 September 2024

 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
				Reproduction or disclosure of this file or its contents without the
				prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

 /* REFERENCES
 https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics4collisiondetection/2017%20Tutorial%204%20-%20Collision%20Detection.pdf
 https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/5collisionresponse/Physics%20-%20Collision%20Response.pdf
 */

#include <array>
#include "PhysicsSystem.hpp"
#include "../ECS/ECSManager.hpp"
#include <iostream>
#include "../Components/Transform.hpp"
#include "../Components/Renderer.hpp"
#include "../Tools/Scripting/ScriptEngine.hpp"
#include "../Components/Name.hpp"
#include "../Input/InputManager.hpp"
#include "../Components/Camera.hpp"
#include "../Layers/LayerManager.hpp"

const float Collision::edgeCollisionThreshold = 5.f;
const float Collision::noCollisionDurationThreshold = 0.07f;

void PhysicsSystem::Init() {
	// By default, add drag and gravity force to all Rigidbodies.
	for (auto const& entity : m_entities) {
		Rigidbody2D& rb = ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity);
		AddDragAndGravity(rb);
		//auto& transform = ECSManager::GetInstance().GetComponent<Transform>(entity);
		//transform.position = Vec3(rb.position);
	}
}

void PhysicsSystem::AddDragAndGravity(Rigidbody2D& rb) {
	AddAlwaysActiveForce(rb, LinearForceIDs::GRAVITY_FORCEID, Vec2{ 0.f, -1.f }, 0.f);
	AddAlwaysActiveForce(rb, LinearForceIDs::DRAG_FORCEID, Vec2{}, 0.f);
}

void PhysicsSystem::Update(double dt) {
	// Press M to toggle physics step by step mode (for debugging purposes)
	if (InputManager::GetInstance().GetKeyDown('M')) {
		ECSManager::GetInstance().physicsSystem->SetStepByStepMode(!ECSManager::GetInstance().physicsSystem->IsStepByStepMode());
	}
	// When physics step by step mode is enabled, press N to update Physics for one frame
	bool physicsUpdate = true;
	if (ECSManager::GetInstance().physicsSystem->IsStepByStepMode()) {
		if (!InputManager::GetInstance().GetKeyDown('N')) {
			physicsUpdate = false;
		}
	}

	if (physicsUpdate) {
		// Loop through all entities.
		for (auto it = m_entities.begin(); it != m_entities.end(); ++it) {
			Rigidbody2D& rb = ECSManager::GetInstance().GetComponent<Rigidbody2D>(*it);
			AABBCollider2D& aabb = ECSManager::GetInstance().GetComponent<AABBCollider2D>(*it);
			entityToRBMap.emplace(*it, std::ref(rb));
			entityToAABBMap.emplace(*it, std::shared_ptr<AABBCollider2D>(&aabb, [](AABBCollider2D*) {}));

			if (!IsRBKinematic(rb)) {
				// Add object to grid if it is not kinematic.
				if (IsBroadPhaseMode()) {
					// Insert the object into the Bitset Grid for broadphase collision detection.
					if (bitsetGrid.InsertToGrid(aabb)) {
						entityToGridIDMap[*it] = aabb.idInGrid;

						// Update rigidbody & collider position if it is not kinematic and static.
						if (!IsRBStatic(rb)) {
							// Update the RB position.
							UpdateRBPosition(rb, (float)dt, *it);
							// Update the AABBCollider based on the Rigidbody's position.
							UpdateAABBCollider(*it);
						}
					}
				}
				else {
					// Update rigidbody & collider position if it is not kinematic and static.
					if (!IsRBStatic(rb)) {
						// Update the RB position.
						UpdateRBPosition(rb, (float)dt, *it);
						// Update the AABBCollider based on the Rigidbody's position.
						UpdateAABBCollider(*it);
					}
				}

				auto& transform = ECSManager::GetInstance().GetComponent<Transform>(*it);
				transform.position = Vec3(GetRBPosition(*it), transform.position.z);
			}
		}

		// Make the bitset grid follow the active camera.
		auto camera = ECSManager::GetInstance().TryGetComponent<Camera>(GraphicsManager::GetInstance().activeCamera);
		if (camera.has_value()) {
			GetBitsetGrid().gridPos = Vec2(ECSManager::GetInstance().GetComponent<Transform>(GraphicsManager::GetInstance().activeCamera).position.x, 
				ECSManager::GetInstance().GetComponent<Transform>(GraphicsManager::GetInstance().activeCamera).position.y);
		}
		// Else, just follow the engine camera.
		else {
			GetBitsetGrid().gridPos = Vec2(GraphicsManager::GetInstance().camera.position.x, GraphicsManager::GetInstance().camera.position.y);
		}

		// Broad-phase collision detection enabled (optimized).
		if (IsBroadPhaseMode()) {
			// Loop through the rows and columns of the grid.
			for (size_t row = 0; row < bitsetGrid.numRows; ++row) {
				for (size_t col = 0; col < bitsetGrid.numCols; ++col) {
					std::vector<Entity> entitiesInCell{};
					// For this cell, find out which entities are in the cell by doing bitwise AND between the rowsBitArray and colsBitArray.
					// E.g. if collider with idInGrid 25 is in the current cell, rowsBitArray and colsBitArray's bit at position 25 should be set to 1.
					std::bitset<BitsetGrid::BITSET_SIZE> cellCollisionBitset = bitsetGrid.rowsBitArray[row] & bitsetGrid.colsBitArray[col];

					// Loop through the colliders to find out which colliders are in the current cell.
					for (auto it = entityToAABBMap.begin(); it != entityToAABBMap.end(); ++it) {
						AABBCollider2D& aabb = *(it->second);
						// Skip the AABB if it is not in the grid.
						if (!aabb.inGrid) {
							continue;
						}

						// Check if the current collider lies in the current cell, and
						// if the bit at position idInGrid of the collider is set to 1 (means collider is in this cell).
						if ((row >= aabb.rowsOccupied.first && row <= aabb.rowsOccupied.second && col >= aabb.colsOccupied.first && col <= aabb.colsOccupied.second) &&
							cellCollisionBitset.test(it->second->idInGrid))
						{
							entitiesInCell.push_back(static_cast<Entity>(it->first));
						}
					}

					// Loop through the entities in the current cell and perform narrow-phase collision detection and resolution.
					for (size_t i = 0; i < entitiesInCell.size(); ++i) {
						for (size_t j = i + 1; j < entitiesInCell.size(); ++j) {
							Entity entity1 = entitiesInCell[i];
							Entity entity2 = entitiesInCell[j];
							
							// Check the entities' collision matrix to see if they should be able to collide.
							Layer layer1 = ECSManager::GetInstance().GetEntityManager().GetLayer(entity1);
							Layer layer2 = ECSManager::GetInstance().GetEntityManager().GetLayer(entity2);
							if (LayerManager::GetInstance().CanLayersCollide(layer1, layer2)) {
								std::shared_ptr<AABBCollider2D>& aabb1 = entityToAABBMap[entity1];
								Rigidbody2D& rb1 = ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity1);
								std::shared_ptr<AABBCollider2D>& aabb2 = entityToAABBMap[entity2];
								Rigidbody2D& rb2 = ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity2);
								DetectAndResolveCollision(entity1, entity2, aabb1, aabb2, rb1, rb2, (float)dt);
								continue;
							}
						}
					}
				}
			}

			ResetBitsetGrid();
		}
		else {
			//Rigidbody2D& rb = ECSManager::GetInstance().GetComponent<Rigidbody2D>(*it);

			// Non-broad-phase collision detection (no optimization).
			// loop through all entities.
			for (auto it1 = m_entities.begin(); it1 != m_entities.end(); ++it1) {
				std::shared_ptr<AABBCollider2D> aabb1 = entityToAABBMap[*it1];
				//AABBCollider2D& aabb1 = ECSManager::GetInstance().GetComponent<AABBCollider2D>(*it1);
				Rigidbody2D& rb1 = ECSManager::GetInstance().GetComponent<Rigidbody2D>(*it1);

				//if (!IsRBStatic(rb1)) {
				//	UpdateRBPosition(rb1, (float)fixedDeltaTime, *it1);

				//	//aabb.isUpdated = false;
				//	// Update the AABBCollider based on the Rigidbody's position.
				//	UpdateAABBCollider(*it1);
				//}

				if (IsRBKinematic(rb1)) continue;

				// check for collision between entities.
				for (auto it2 = std::next(it1, 1); it2 != m_entities.end(); ++it2) {
					std::shared_ptr<AABBCollider2D> aabb2 = entityToAABBMap[*it2];
					//AABBCollider2D& aabb2 = ECSManager::GetInstance().GetComponent<AABBCollider2D>(*it2);
					Rigidbody2D& rb2 = ECSManager::GetInstance().GetComponent<Rigidbody2D>(*it2);
					DetectAndResolveCollision(*it1, *it2, aabb1, aabb2, rb1, rb2, (float)dt);
				}
			}
		}

		CleanupCollisions((float)dt);
	}

	for (auto it = m_entities.begin(); it != m_entities.end(); ++it) {
		// Update the AABBCollider based on the Rigidbody's position.
		UpdateAABBCollider(*it);
	}

	entityToAABBMap.clear();
	entityToRBMap.clear();
}

void PhysicsSystem::Exit() {
	playerEntity = std::nullopt;
}

bool PhysicsSystem::IsStepByStepMode() {
	return stepByStepMode;
}

void PhysicsSystem::SetStepByStepMode(bool _bool) {
	stepByStepMode = _bool;
}

bool PhysicsSystem::IsBroadPhaseMode() {
	return broadPhaseMode;
	//return false;
}

void PhysicsSystem::SetBroadPhaseMode(bool _bool) {
	broadPhaseMode = _bool;
}

// Add AABBCollider component with automatic size based on mesh size.
void PhysicsSystem::AddAABBColliderComponent(Entity entity) {
	// Get the mesh's min and max points.
	size_t meshID = ECSManager::GetInstance().GetComponent<Renderer>(entity).currentMeshID;
	Vec2 min = Vec2(GraphicsManager::GetInstance().meshes[meshID].vertices[0].position.x, GraphicsManager::GetInstance().meshes[meshID].vertices[0].position.y);
	Vec2 max = Vec2(GraphicsManager::GetInstance().meshes[meshID].vertices[0].position.x, GraphicsManager::GetInstance().meshes[meshID].vertices[0].position.y);

	for (const auto& vertex : GraphicsManager::GetInstance().meshes[meshID].vertices) {
		// Update min and max (ignoring z-axis)
		if (vertex.position.x < min.x) min.x = vertex.position.x;
		if (vertex.position.y < min.y) min.y = vertex.position.y;
		if (vertex.position.x > max.x) max.x = vertex.position.x;
		if (vertex.position.y > max.y) max.y = vertex.position.y;
	}

	ECSManager::GetInstance().AddComponent(entity, AABBCollider2D{ 0.0f, min, max, false });
	CalculateColliderPosOffset(entity);
	//entityToAABBMap.emplace(entity, std::ref(ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity)));
}

void PhysicsSystem::AddAABBColliderComponent(Entity entity, float bounciness, const Vec2& min, const Vec2& max, bool isTrigger) {
	ECSManager::GetInstance().AddComponent(entity, AABBCollider2D{ bounciness, min, max, isTrigger });
	CalculateColliderPosOffset(entity);
	//entityToAABBMap.emplace(entity, std::ref(ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity)));
}

float PhysicsSystem::GetAABBColliderBounciness(Entity entity) {
	return ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).bounciness;
}

void PhysicsSystem::SetAABBColliderBounciness(Entity entity, float val) {
	ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).bounciness = val;
}

bool PhysicsSystem::IsAABBColliderTrigger(Entity entity) {
	return ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).isTrigger;
}

void PhysicsSystem::SetAABBColliderTrigger(Entity entity, bool val) {
	ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).isTrigger = val;
}

Vec2 PhysicsSystem::GetAABBColliderMin(Entity entity) {
	return ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).min;
}

void PhysicsSystem::SetAABBColliderMin(Entity entity, const Vec2& min) {
	ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).min = min;
}

Vec2 PhysicsSystem::GetAABBColliderMax(Entity entity) {
	return ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).max;
}

void PhysicsSystem::SetAABBColliderMax(Entity entity, const Vec2& max) {
	ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).max = max;
}

float PhysicsSystem::GetAABBColliderSizeX(Entity entity) {
	return ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).sizeX;
}

float PhysicsSystem::GetAABBColliderSizeY(Entity entity) {
	return ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).sizeY;
}

Vec2 PhysicsSystem::GetAABBColliderPos(Entity entity) {
	return ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).centerPos;
}

void PhysicsSystem::SetAABBColliderPos(Entity entity, const Vec2& pos) {
	ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).centerPos = pos;
}

std::map<Entity, std::shared_ptr<AABBCollider2D>>& PhysicsSystem::GetEntityToAABBMap() {
	return entityToAABBMap;
}

std::vector<Collision>& PhysicsSystem::GetCollisions(Entity entity) {
	return ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).collisions;
}

void PhysicsSystem::AddOrUpdateCollisions(Entity entity, const Collision& collision) {
	std::vector<Collision>& collisions = GetCollisions(entity);
	auto it = std::find(collisions.begin(), collisions.end(), collision);
	// If this is a new collision
	if (it == collisions.end()) {
		// Add it to the collisions vector.
		collisions.push_back(collision);
	}
	// Else if this is an existing collision
	else {
		// Update the existing collision info.
		*it = collision;
	}

	// Call the OnCollisionStay or OnTriggerStay functions respectively.
	if (!collision.isTrigger) {
		CollisionCS collisionCS = ConvertCollisionToCS(entity, collision);
		ScriptEngine::OnEntityCollisionStay(entity, collisionCS);
	}
	else {
		ColliderCS colliderCS{ collision.entity };
		ScriptEngine::OnEntityTriggerStay(entity, colliderCS);
	}
}

bool PhysicsSystem::IsColliderEnter(Entity entity, const Collision& collision) {
	std::vector<Collision>& collisions = GetCollisions(entity);
	return (std::find(collisions.begin(), collisions.end(), collision) == collisions.end());
}

void PhysicsSystem::CleanupCollisions(float dt) {
	// Loop through the entities.
	for (auto& entityAABB : entityToAABBMap) {
		entityAABB.second->staticCollisions.clear();

		auto& collisions = entityAABB.second->collisions;
		// Loop through the current entity's collisions vector.
		for (auto it = collisions.begin(); it != collisions.end();) {
			// Reset the current collision's noCollisionDuration and resolved bool.
			it->noCollisionDuration += dt;
			it->resolved = false;

			// If the current collision has had no collisions for longer than the threshold duration
			if (it->noCollisionDuration >= Collision::noCollisionDurationThreshold) {
				// Call the OnCollisionExit or OnTriggerExit functions respectively.
				if (!it->isTrigger) {
					CollisionCS collisionCS = ConvertCollisionToCS(entityAABB.first, *it);
					ScriptEngine::OnEntityCollisionExit(entityAABB.first, collisionCS);
				}
				else {
					ColliderCS colliderCS{ it->entity };
					ScriptEngine::OnEntityTriggerExit(entityAABB.first, colliderCS);
				}

				it = collisions.erase(it);
			}
			else {
				++it;
			}
		}
	}
}

//void PhysicsSystem::UpdateCollidedEntitiesTracker(Entity entity, Entity collidedEntity) {
//	std::unordered_map<Entity, bool>& collidedEntitiesTracker = ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).collidedEntitiesTracker;
//	// If the collided entity was already colliding with the entity
//	if (collidedEntitiesTracker.find(collidedEntity) != collidedEntitiesTracker.end()) {
//		// Set the value to false.
//		collidedEntitiesTracker[collidedEntity] = false;
//	}
//	// Else if the collided entity just started colliding with the entity
//	else {
//		// Add it to the collidedEntitiesTracker with its value as true (true means OnCollisionEnter).
//		collidedEntitiesTracker.insert({collidedEntity, true});
//	}
//}

bool PhysicsSystem::DetectCollisionEnterAABB_AABB(std::shared_ptr<AABBCollider2D> aabb1, const Vec2& vel1, std::shared_ptr<AABBCollider2D> aabb2, const Vec2& vel2, Collision& col1, Collision& col2, float fixedDt)
{
	// Check static collision
	bool staticCollision = true;

	// If any of these are true, there is no collision.
	if (aabb1->min.x > aabb2->max.x) staticCollision = false;
	if (aabb2->min.x > aabb1->max.x) staticCollision = false;
	if (aabb1->max.y < aabb2->min.y) staticCollision = false;
	if (aabb2->max.y < aabb1->min.y) staticCollision = false;

	// Get the relative velocity
	Vec2 vRel{};
	vRel.x = vel2.x - vel1.x;
	vRel.y = vel2.y - vel1.y;

	// Check dynamic collision
	if (!staticCollision) {
		float tFirst = 0, tLast = fixedDt;
		float dFirst{}, dLast{};

		// X-axis dynamic collision check
		if (vRel.x < 0) {
			// Case 1: object 2 moving away from object 1 (to the left) (no collision)
			if (aabb1->min.x > aabb2->max.x) {
				return false;
			}
			// Case 4: object 2 moving towards object 1 (to the left)
			if (aabb1->max.x < aabb2->min.x) {
				dFirst = aabb1->max.x - aabb2->min.x;
				tFirst = fmax(dFirst / vRel.x, tFirst);
			}
			if (aabb1->min.x < aabb2->max.x) {
				dLast = aabb1->min.x - aabb2->max.x;
				tLast = fmin(dLast / vRel.x, tLast);
			}
		}
		else if (vRel.x > 0) {
			// Case 2: object 2 moving towards object 1 (to the right)
			if (aabb1->min.x > aabb2->max.x) {
				dFirst = aabb1->min.x - aabb2->max.x;
				tFirst = fmax(dFirst / vRel.x, tFirst);
			}
			if (aabb1->max.x > aabb2->min.x) {
				dLast = aabb1->max.x - aabb2->min.x;
				tLast = fmin(dLast / vRel.x, tLast);
			}
			// Case 3: object 2 moving away from object 1 (to the right) (no collision)
			if (aabb1->max.x < aabb2->min.x) {
				return false;
			}
		}
		else if (vRel.x == 0) {
			// Case 5: object 2 is moving perpendicular to object 1 (no collision)
			if (aabb1->max.x < aabb2->min.x) {
				return false;
			}
			else if (aabb1->min.x > aabb2->max.x) {
				return false;
			}
		}

		// Case 6: no intersection
		if (tFirst > tLast) {
			return false;
		}

		// Y-axis check
		if (vRel.y < 0) {
			// Case 1: object 2 moving away from object 1 (downwards) (no collision)
			if (aabb1->min.y > aabb2->max.y) {
				return false;
			}
			// Case 4: object 2 moving towards object 1 (downwards)
			if (aabb1->max.y < aabb2->min.y) {
				dFirst = aabb1->max.y - aabb2->min.y;
				tFirst = fmax(dFirst / vRel.y, tFirst);
			}
			if (aabb1->min.y < aabb2->max.y) {
				dLast = aabb1->min.y - aabb2->max.y;
				tLast = fmin(dLast / vRel.y, tLast);
			}
		}
		else if (vRel.y > 0) {
			// Case 2: object 2 moving towards object 1 (upwards)
			if (aabb1->min.y > aabb2->max.y) {
				dFirst = aabb1->min.y - aabb2->max.y;
				tFirst = fmax(dFirst / vRel.y, tFirst);
			}
			if (aabb1->max.y > aabb2->min.y) {
				dLast = aabb1->max.y - aabb2->min.y;
				tLast = fmin(dLast / vRel.y, tLast);
			}
			// Case 3: object 2 moving away from object 1 (upwards) (no collision)
			if (aabb1->max.y < aabb2->min.y) {
				return false;
			}
		}
		else if (vRel.y == 0) {
			// Case 5: object 2 is moving perpendicular to object 1 (no collision)
			if (aabb1->max.y < aabb2->min.y) {
				return false;
			}
			else if (aabb1->min.y > aabb2->max.y) {
				return false;
			}
		}

		// Case 6: no intersection
		if (tFirst > tLast) {
			return false;
		}
	}

	// collision detected, get the contact normal and penetration and update the ContactPoint.
	const int numSides = 4;
	// sides (and eventually contact point normal) vector is relative to aabb1
	std::array<Vec2, numSides> sides = { Vec2{-1.f, 0.f}, Vec2{1.0f, 0.f}, Vec2{0.f, -1.f}, Vec2{0.f, 1.f} };
	std::array<float, numSides> distances = { (aabb2->max.x - aabb1->min.x), (aabb1->max.x - aabb2->min.x), (aabb2->max.y - aabb1->min.y), (aabb1->max.y - aabb2->min.y) };

	float penetration = std::numeric_limits<float>::max();
	Vec2 normal{};

	// get the contact normal and penetration based on the distance between the 2 AABBs.
	for (int i = 0; i < numSides; ++i) {
		if (distances[i] < penetration) {
			penetration = distances[i];
			normal = sides[i];
		}
	}

	//// Calculate the overlap of the 2 AABBs.
	//float overlapX = std::max(0.0f, std::min(aabb1.max.x, aabb2.max.x) - std::max(aabb1.min.x, aabb2.min.x)); 
	//float overlapY = std::max(0.0f, std::min(aabb1.max.y, aabb2.max.y) - std::max(aabb1.min.y, aabb2.min.y)); 

	// The contact point will be the midpoint of the overlap.
	col1.contactPoint.point.x = (std::max(aabb1->min.x, aabb2->min.x) + std::min(aabb1->max.x, aabb2->max.x)) / 2.f;
	col1.contactPoint.point.y = (std::max(aabb1->min.y, aabb2->min.y) + std::min(aabb1->max.y, aabb2->max.y)) / 2.f;
	col2.contactPoint.point = col1.contactPoint.point;

	if ((normal.y < 0.f && aabb1->min.y < aabb2->max.y && vel1.y > 0.f) || 
		(normal.y > 0.f && aabb2->min.y < aabb1->max.y && vel2.y > 0.f))
	{
		normal.x = -normal.y;
		normal.y = 0.f;
	}

	col1.contactPoint.normal = normal;
	//if (col1.contactPoint.normal.x != 0.f && (std::min(aabb1.max.y, aabb2.max.y) - col1.contactPoint.point.y) < Collision::edgeCollisionThreshold)
	//	return false;

	col1.contactPoint.penetration = penetration;
	col1.contactPoint.thisCollider = aabb1;
	col1.contactPoint.otherCollider = aabb2;
	col1.collider = col1.contactPoint.otherCollider;

	col2.contactPoint.normal = -normal;
	col2.contactPoint.penetration = penetration;
	col2.contactPoint.thisCollider = aabb2;
	col2.contactPoint.otherCollider = aabb1;
	col2.collider = col2.contactPoint.otherCollider;

	return true;
}

void PhysicsSystem::ResolveCollision(Entity entity1, Rigidbody2D& rb1, float bounciness1, Entity entity2, Rigidbody2D& rb2, float bounciness2, Collision& col1, Collision& col2, float dt) {
	// calculate the position correction to prevent clipping.
	// define the penetration percentage correction per frame.
	const float percent = 0.2f;
	// define the penetration allowance.
	const float slop = 0.01f;

	// calculate the correction vector needed to move the rigidbodies so that they eventually do not clip past the penetration allowance.
	// correction vector is based on the inverse masses of the rigidbodies so that they get pushed back realistically based on their mass.
	Vec2 correction = fmax(col1.contactPoint.penetration - slop, 0.0f) / (GetRBInverseMass(rb1) + GetRBInverseMass(rb2)) * percent * col1.contactPoint.normal;
	SetRBPosition(entity1, GetRBPosition(entity1) - correction / GetRBMass(rb1));
	SetRBPosition(entity2, GetRBPosition(entity2) + correction / GetRBMass(rb2));

	// get the magnitude of the velocity relative to the collision normal.
	float relativeVelocity = ((GetRBVelocity(rb2) - (GetRBVelocity(rb1))).Dot(col1.contactPoint.normal));
	// calculate average bounciness of the 2 objects.
	float avgBounciness = (bounciness1 + bounciness2) / 2.f;
	// impulse scalar of the collision (j) is based on the objects' average bounciness and the inverse masses of the objects
	// to simulate realistic collision response.
	float j = -(1.f + avgBounciness) * relativeVelocity / (GetRBInverseMass(rb1) + GetRBInverseMass(rb2));

	// set the object's velocity based on the resultant impulse of the collision.
	Vec2 impulse = j * col1.contactPoint.normal;
	Vec2 newVel1 = GetRBVelocity(rb1) - (impulse / GetRBMass(rb1));
	Vec2 acceleration1 = ((newVel1 - GetRBVelocity(rb1)) / dt);
	AddImpulseForce(rb1, acceleration1, GetRBMass(rb1));

	Vec2 newVel2 = GetRBVelocity(rb2) + (impulse / GetRBMass(rb2));
	Vec2 acceleration2 = ((newVel2 - GetRBVelocity(rb2)) / dt);
	AddImpulseForce(rb2, acceleration2, GetRBMass(rb2));

	//Vec2 rb1Vel = GetRBVelocity(rb1) - (impulse / GetRBMass(rb1));
	//Vec2 rb2Vel = GetRBVelocity(rb2) + (impulse / GetRBMass(rb2));

	//SetRBVelocity(rb1, rb1Vel);
	//SetRBVelocity(rb2, rb2Vel);

	rb1.noCollisionDuration = 0.f;
	rb2.noCollisionDuration = 0.f;

	col1.impulse = -impulse;
	col1.relativeVelocity = GetRBVelocity(rb1) - GetRBVelocity(rb2);
	col2.impulse = impulse;
	col2.relativeVelocity = GetRBVelocity(rb2) - GetRBVelocity(rb1);
}

void PhysicsSystem::ResolveCollision(Entity entity, Rigidbody2D& rb, float bounciness1, float bounciness2, Collision& col1, Collision& col2, float dt) {
	//if (col1.contactPoint.normal.x != 0.f &&
	//	(fabsf(col1.contactPoint.normal.x * col1.contactPoint.penetration) < Collision::edgeCollisionThreshold))
	//{
	//	std::cout << "Resolve collision: false" << std::endl;
	//	return;
	//}

	bool resolve = false;
	for (auto& collision : ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).staticCollisions) {
		if (col1 == collision || collision.contactPoint.normal != col1.contactPoint.normal) {
			ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).staticCollisions.push_back(col1);
			resolve = true;
		}
	}

	if (ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).staticCollisions.empty()) {
		ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).staticCollisions.push_back(col1);
		resolve = true;
	}

	Vec2 impulse{};
	if (resolve) {
		if (auto aabb1 = col1.contactPoint.thisCollider.lock()) {
			if (auto aabb2 = col1.contactPoint.otherCollider.lock()) {
				if (col1.contactPoint.normal.x != 0.f && (std::min(aabb1->max.y, aabb2->max.y) - col1.contactPoint.point.y) < Collision::edgeCollisionThreshold) {
					SetRBPosition(entity, GetRBPosition(entity).x, GetRBPosition(entity).y + std::min(aabb1->max.y, aabb2->max.y) - col1.contactPoint.point.y);
					return;
				}
			}
		}
		// calculate the position correction to prevent clipping.
		// define the penetration percentage correction per frame.
		const float percent = 0.75f;
		// define the penetration allowance.
		const float slop = 0.01f;

		// calculate the correction vector needed to move the rigidbody so that it eventually does not clip past the penetration allowance.
		Vec2 correction = fmax(col1.contactPoint.penetration - slop, 0.0f) * percent * col1.contactPoint.normal;

		if (correction.x != 0.f || correction.y != 0.f)
			SetRBPosition(entity, GetRBPosition(entity) - correction);
		else if (correction == Vec2{})
			SetRBPosition(entity, GetRBPosition(entity) + (fabsf(col1.contactPoint.penetration) * col1.contactPoint.normal));

		// get the magnitude of the velocity relative to the collision normal.
		Vec2 currVelocity = GetRBVelocity(rb);
		//std::cout << "velocity: " << velocity.x << ", " << velocity.y << std::endl;
		float relativeVelocity = (-GetRBVelocity(rb).Dot(col1.contactPoint.normal));
		// calculate average bounciness of the 2 objects.
		float avgBounciness = (bounciness1 + bounciness2) / 2.f;
		// impulse scalar of the collision (j) is based on the object's bounciness to simulate realistic collision response.
		float j = -(1.f + avgBounciness) * relativeVelocity;

		// set the object's velocity based on the resultant impulse of the collision.
		impulse = j * col1.contactPoint.normal;
		Vec2 newVel = currVelocity - impulse;
		Vec2 acceleration = ((newVel - currVelocity) / dt);

		AddImpulseForce(rb, acceleration, GetRBMass(rb));
	}

	//Vec2 rb1Vel = GetRBVelocity(rb) - (impulse / GetRBMass(rb));

	//SetRBVelocity(rb, rb1Vel);
	//std::cout << "resolution vel: " << GetRBVelocity(rb).x << ", " << GetRBVelocity(rb).y << std::endl;

	if (col1.contactPoint.normal.y < 0.f && col1.contactPoint.normal.x == 0.f) {
		SetRBGrounded(rb, true);
	}

	rb.noCollisionDuration = 0.f;

	col1.impulse = -impulse;
	col1.relativeVelocity = GetRBVelocity(rb);
	col2.impulse = impulse;
	col2.relativeVelocity = -GetRBVelocity(rb);
}

void PhysicsSystem::DetectAndResolveCollision(Entity entity1, Entity entity2, std::shared_ptr<AABBCollider2D> aabb1, std::shared_ptr<AABBCollider2D> aabb2, Rigidbody2D& rb1, Rigidbody2D& rb2, float dt) {
	std::vector<Collision>& collisions1 = GetCollisions(entity1);
	//std::vector<Collision>& collisions2 = GetCollisions(entity2);

	Vec2 vel1{}, vel2{};

	bool aabb1RbResponse = !rb1.isStatic;
	bool aabb2RbResponse = !rb2.isStatic;

	if (aabb1RbResponse) vel1 = rb1.velocity;
	if (aabb2RbResponse) vel2 = rb2.velocity;

	Collision col1, col2;
	// if collision between the 2 entities is detected,
	if (DetectCollisionEnterAABB_AABB(aabb1, vel1, aabb2, vel2, col1, col2, dt)) {
		bool isTrigger = (IsAABBColliderTrigger(entity1) || IsAABBColliderTrigger(entity2));
		col1.isTrigger = isTrigger;
		col2.isTrigger = isTrigger;

		col1.rigidbody = std::make_shared<Rigidbody2D>(rb2);
		col1.entity = entity2;
		col1.noCollisionDuration = 0.f;
		col1.resolved = true;
		col2.rigidbody = std::make_shared<Rigidbody2D>(rb1);
		col2.entity = entity1;
		col2.noCollisionDuration = 0.f;
		col2.resolved = true;

		// Only respond to collisions if both colliders are not triggers.
		bool collisionResponse = !isTrigger;
		// Only respond to collisions if it has not been resolved yet.
		if (collisionResponse) {
			for (auto it = collisions1.begin(); it != collisions1.end(); ++it) {
				if (it->entity == entity2 && it->resolved) {
					collisionResponse = false;
					break;
				}
			}
		}

		// If the collision between the entities is 'new'
		if (IsColliderEnter(entity1, col1)) {
			// Call the OnCollisionEnter or OnTriggerEnter functions respectively.
			if (!isTrigger) {
				CollisionCS col1cs = ConvertCollisionToCS(entity1, col1);
				ScriptEngine::OnEntityCollisionEnter(entity1, col1cs);
			}
			else {
				ColliderCS colliderCS{ col1.entity };
				ScriptEngine::OnEntityTriggerEnter(entity1, colliderCS);
			}
		}
		if (IsColliderEnter(entity2, col2)) {
			if (!isTrigger) {
				CollisionCS col2cs = ConvertCollisionToCS(entity2, col2);
				ScriptEngine::OnEntityCollisionEnter(entity2, col2cs);
			}
			else {
				ColliderCS colliderCS{ col2.entity };
				ScriptEngine::OnEntityTriggerEnter(entity2, colliderCS);
			}
		}

		AddOrUpdateCollisions(entity1, col1);
		AddOrUpdateCollisions(entity2, col2);

		if (collisionResponse) {
			// Resolve collision between entities.
			if (aabb1RbResponse && aabb2RbResponse) {
				ResolveCollision(entity1, rb1, aabb1->bounciness, entity2, rb2, aabb2->bounciness, col1, col2, dt);
			}
			else if (aabb1RbResponse)
				ResolveCollision(entity1, rb1, aabb1->bounciness, aabb2->bounciness, col1, col2, dt);
			else if (aabb2RbResponse) {
				ResolveCollision(entity2, rb2, aabb2->bounciness, aabb1->bounciness, col2, col1, dt);
			}
		}
	}
	else {
		if (!rb1.isStatic) {
			rb1.noCollisionDuration += (float)dt;
			if (rb1.noCollisionDuration >= rb1.noCollisionDurationThreshold) {
				SetRBGrounded(rb1, false);
			}
		}
	}
}

CollisionCS PhysicsSystem::ConvertCollisionToCS(Entity thisEntity, const Collision& collision) {
	CollisionCS colCS;
	colCS.thisEntity = thisEntity;
	colCS.otherEntity = collision.entity;
	colCS.impulse = collision.impulse;
	colCS.relativeVelocity = collision.relativeVelocity;
	colCS.contactPoint.point = collision.contactPoint.point;
	colCS.contactPoint.normal = collision.contactPoint.normal;
	colCS.contactPoint.penetration = collision.contactPoint.penetration;

	return colCS;
}

//void PhysicsSystem::CheckContactsResolveCollision(AABBCollider2D& aabb1, AABBCollider2D& aabb2, const ContactPoint& cp) {
//	std::shared_ptr<Rigidbody2D> rb1 = GetAttachedRB2D(aabb1);
//	std::shared_ptr<Rigidbody2D> rb2 = GetAttachedRB2D(aabb2);
//
//	if (aabb1.contacts.size() == 1 && aabb2.contacts.size() == 1) {
//		if (rb1 != nullptr && rb2 != nullptr) ResolveCollision(*rb1, *rb2, cp);
//		else if (rb1 != nullptr) ResolveCollision(*rb1, cp);
//		else if (rb2 != nullptr) ResolveCollision(*rb2, cp);
//	}
//	else if (aabb1.contacts.size() > 1) {
//
//	}
//}

//const std::unordered_set<Collider2D, Collider2D::Collider2DHash>& PhysicsSystem::GetContacts(const Collider2D& collider) {
//	return collider.contacts;
//}

void PhysicsSystem::AddRigidbodyComponent(Entity entity, Rigidbody2D&) {
	//ECSManager::GetInstance().AddComponent(entity, rb);
	auto& rigidbody = ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity);
	//entityToRBMap.emplace(entity, std::ref<Rigidbody2D>(ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity)));
	//entityToRBMap.emplace(entity, rigidbody);
	AddDragAndGravity(rigidbody);
}

void PhysicsSystem::AddRigidbodyComponent(Entity entity, Vec2 position, Vec2 velocity, float mass, float drag, float gravityScale, bool isStatic, bool isKinematic) {
	ECSManager::GetInstance().AddComponent(entity, Rigidbody2D{ position, velocity, mass, drag, gravityScale, isStatic, isKinematic });
	auto& rb = ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity);
	//entityToRBMap.emplace(entity, rb);
	AddDragAndGravity(rb);
}

inline Vec2 PhysicsSystem::GetRBPosition(Entity entity) {
	return ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity).position;
}

void PhysicsSystem::SetRBPosition(Entity entity, const Vec2& newPosition) {
	Rigidbody2D& rb = ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity);
	rb.oldPosition = GetRBPosition(entity);
	rb.position = newPosition;
	Transform& transform = ECSManager::GetInstance().GetComponent<Transform>(entity);
	transform.position = Vec3{ newPosition, transform.position.z };
	transform.modelToWorldMtx
		= Mat4::BuildTranslation(transform.position)
		* Mat4::BuildZRotation(transform.rotation.z)
		* Mat4::BuildScaling(transform.scale.x, transform.scale.y, transform.scale.z);
	UpdateAABBCollider(entity);
}

void PhysicsSystem::SetRBPosition(Entity entity, float _x, float _y) {
	ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity).oldPosition = GetRBPosition(entity);
	ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity).position.x = _x;
	ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity).position.y = _y;
	Transform& transform = ECSManager::GetInstance().GetComponent<Transform>(entity);
	transform.position = Vec3{ _x, _y, ECSManager::GetInstance().GetComponent<Transform>(entity).position.z };
	transform.modelToWorldMtx
		= Mat4::BuildTranslation(transform.position)
		* Mat4::BuildZRotation(transform.rotation.z)
		* Mat4::BuildScaling(transform.scale.x, transform.scale.y, transform.scale.z);
	UpdateAABBCollider(entity);
}

inline Vec2 PhysicsSystem::GetRBVelocity(const Rigidbody2D& rb) {
	return rb.velocity;
}

void PhysicsSystem::SetRBVelocity(Rigidbody2D& rb, const Vec2& newVelocity) {
	rb.velocity = newVelocity;
}

void PhysicsSystem::SetRBVelocity(Rigidbody2D& rb, float _x, float _y) {
	rb.velocity.x = _x; rb.velocity.y = _y;
}

inline float PhysicsSystem::GetRBMass(const Rigidbody2D& rb) {
	return rb.mass;
}

void PhysicsSystem::SetRBMass(Rigidbody2D& rb, float newMass) {
	rb.mass = newMass;
}

inline float PhysicsSystem::GetRBInverseMass(const Rigidbody2D& rb) {
	return 1.f / rb.mass;
}

inline float PhysicsSystem::GetRBDrag(const Rigidbody2D& rb) {
	return rb.drag;
}

void PhysicsSystem::SetRBDrag(Rigidbody2D& rb, float drag) {
	rb.drag = drag;
}

inline float PhysicsSystem::GetRBGravityScale(const Rigidbody2D& rb) {
	return rb.gravityScale;
}

void PhysicsSystem::SetRBGravityScale(Rigidbody2D& rb, float newScale) {
	rb.gravityScale = newScale;
}

inline bool PhysicsSystem::IsRBStatic(const Rigidbody2D& rb) {
	return rb.isStatic;
}

void PhysicsSystem::SetRBStatic(Rigidbody2D& rb, bool _bool) {
	rb.isStatic = _bool;
}

inline bool PhysicsSystem::IsRBKinematic(const Rigidbody2D& rb) {
	return rb.isKinematic;
}

void PhysicsSystem::SetRBKinematic(Rigidbody2D& rb, bool _isKinematic) {
	rb.isKinematic = _isKinematic;
}

bool PhysicsSystem::IsRBGrounded(const Rigidbody2D& rb) {
	return rb.isGrounded;
}

void PhysicsSystem::SetRBGrounded(Rigidbody2D& rb, bool _grounded) {
	rb.isGrounded = _grounded;
}

inline ForcesManager& PhysicsSystem::GetRBForcesManager(Rigidbody2D& rb) {
	return rb.forcesManager;
}

BitsetGrid& PhysicsSystem::GetBitsetGrid() {
	return bitsetGrid;
}

void PhysicsSystem::ResetBitsetGrid() {
	bitsetGrid.ClearGrid();
	for (auto it = entityToAABBMap.begin(); it != entityToAABBMap.end(); ++it) {
		it->second->idInGrid = static_cast<size_t>(-1);
		it->second->rowsOccupied = std::pair<size_t, size_t>{};
		it->second->colsOccupied = std::pair<size_t, size_t>{};
		it->second->inGrid = false;
	}
}

//void PhysicsSystem::RemoveAABBFromMap(Entity entity) {
//	entityToAABBMap.erase(entity);
//}
//
//void PhysicsSystem::RemoveRBFromMap(Entity entity) {
//	entityToRBMap.erase(entity);
//}

std::map<Entity, std::reference_wrapper<Rigidbody2D>>& PhysicsSystem::GetEntityToRBMap() {
	return entityToRBMap;
}

// Add a force with an auto-assigned ID and return the ID.
size_t PhysicsSystem::AddForce(Rigidbody2D& rb, const LinearForce& force) {
	std::queue<size_t>& freeIDs = GetRBForcesManager(rb).freeIDs;
	std::map<size_t, LinearForce>& linearForces = GetRBForcesManager(rb).linearForces;
	if (!freeIDs.empty()) {
		// Assign an ID from the freeIDs queue if there is any.
		linearForces[freeIDs.front()] = force;
		size_t id = freeIDs.front();
		freeIDs.pop();
		return id;
	}
	else {
		// Else, assign the next smallest possible ID.
		linearForces[GetRBForcesManager(rb).currLinearForceID] = force;
		return GetRBForcesManager(rb).currLinearForceID++;
	}
}

// Add a force with a specified ID and return the ID.
size_t PhysicsSystem::AddForce(Rigidbody2D& rb, size_t forceID, const LinearForce& force) {
	std::map<size_t, LinearForce>& linearForces = GetRBForcesManager(rb).linearForces;
	// First check if forceID doesn't already exist in linearForces.
	if (linearForces.find(forceID) != linearForces.end()) {
		std::cout << "ERROR: ForceID to be added already exists." << std::endl;
		return static_cast<size_t>(-1);
	}
	else {
		linearForces[forceID] = force;
		return forceID;
	}
}

// Add a force with an auto-assigned ID and return the ID.
size_t PhysicsSystem::AddForce(Rigidbody2D& rb, const Vec2& direction, float magnitude, bool isForever = false, float activeLifetime = 0.f, bool isActive = true, bool isAlwaysActive = false) {
	LinearForce force{ direction, magnitude, isForever, activeLifetime, isActive, isAlwaysActive };
	std::queue<size_t>& freeIDs = GetRBForcesManager(rb).freeIDs;
	std::map<size_t, LinearForce>& linearForces = GetRBForcesManager(rb).linearForces;
	if (!freeIDs.empty()) {
		// Assign an ID from the freeIDs queue if there is any.
		linearForces[freeIDs.front()] = force;
		size_t id = freeIDs.front();
		freeIDs.pop();
		return id;
	}
	else {
		// Else, assign the next smallest possible ID.
		linearForces[GetRBForcesManager(rb).currLinearForceID] = force;
		return GetRBForcesManager(rb).currLinearForceID++;
	}
}

// Add a force with a specified ID and return the ID.
size_t PhysicsSystem::AddForce(Rigidbody2D& rb, size_t forceID, const Vec2& direction, float magnitude, bool isForever = false, float activeLifetime = 0.f, bool isActive = true, bool isAlwaysActive = false) {
	LinearForce force{ direction, magnitude, isForever, activeLifetime, isActive, isAlwaysActive };
	std::map<size_t, LinearForce>& linearForces = GetRBForcesManager(rb).linearForces;
	// First check if forceID doesn't already exist in linearForces.
	if (linearForces.find(forceID) != linearForces.end()) {
		std::cout << "ERROR: ForceID to be added already exists." << std::endl;
		return static_cast<size_t>(-1);
	}
	else {
		linearForces[forceID] = force;
		return forceID;
	}
}

// Helper function to add an instantaneous force to the RB less verbosely.
size_t PhysicsSystem::AddImpulseForce(Rigidbody2D& rb, const Vec2& direction, float magnitude) {
	return AddForce(rb, direction, magnitude);
}

// Helper function to add a force over time to the RB less verbosely.
size_t PhysicsSystem::AddForceOverTime(Rigidbody2D& rb, const Vec2& direction, float magnitude, float duration) {
	return AddForce(rb, direction, magnitude, false, duration, true);
}

size_t PhysicsSystem::AddAlwaysActiveForce(Rigidbody2D& rb, size_t forceID, const Vec2& direction, float magnitude) {
	LinearForce force{ direction, magnitude, true, 0.f, true, true };
	std::map<size_t, LinearForce>& alwaysActiveForces = GetRBForcesManager(rb).alwaysActiveForces;

	// First check if forceID doesn't already exist in alwaysActiveForces.
	if (alwaysActiveForces.find(forceID) != alwaysActiveForces.end()) {
		//std::cout << "ERROR: ForceID to be added to alwaysActiveForces already exists." << std::endl;
		return static_cast<size_t>(-1);
	}
	else {
		alwaysActiveForces[forceID] = force;
		return forceID;
	}
}

size_t PhysicsSystem::AddForeverImpulseForce(Rigidbody2D& rb, const Vec2& direction, float magnitude, bool startActive, size_t forceID = static_cast<size_t>(-1)) {
	if (forceID == static_cast<size_t>(-1)) {
		return AddForce(rb, direction, magnitude, true, 0.f, startActive);
	}
	else {
		return AddForce(rb, forceID, direction, magnitude, true, 0.f, startActive);
	}
}

void PhysicsSystem::ActivateForce(Rigidbody2D& rb, size_t forceID, bool activate = true) {
	std::map<size_t, LinearForce>& linearForces = GetRBForcesManager(rb).linearForces;
	if (linearForces.find(forceID) == linearForces.end()) {
		std::cout << "ERROR: ForceID to be activated is invalid." << std::endl;
	}
	else {
		linearForces[forceID].isActive = activate;
	}
}

void PhysicsSystem::UpdateLinearForceLifetime(ForcesManager& forcesManager, size_t forceID, float fixedDt) {
	std::map<size_t, LinearForce>& linearForces = forcesManager.linearForces;
	// Only update the force lifetime if it is not always active.
	if (!linearForces[forceID].isAlwaysActive) {
		linearForces[forceID].currentLifetime += fixedDt;
		if (linearForces[forceID].currentLifetime >= linearForces[forceID].activeLifetime) {
			// If the force's lifetime is up and it is forever, reset the lifetime and set it to inactive.
			if (linearForces[forceID].isForever) {
				linearForces[forceID].currentLifetime = 0;
				linearForces[forceID].isActive = false;
			}
			// Else if the force's lifetime is up and it is not forever, remove it from the map and push the id to freeIDs.
			else {
				forcesManager.freeIDs.push(forceID);
				linearForces.erase(forceID);
			}
		}
	}
}

void PhysicsSystem::CalculateDragForce(Rigidbody2D& rb, const Vec2& currResultantForce, float fixedDt) {
	// For drag, calculate the drag force first based on the velocity before drag.
	Vec2 currAcceleration = currResultantForce / GetRBMass(rb);
	Vec2 currVelocity = GetRBVelocity(rb) + (currAcceleration * fixedDt);
	float speed = currVelocity.Length();
	Vec2 dragDirection = -(currVelocity.Normalized());

	GetRBForcesManager(rb).alwaysActiveForces[LinearForceIDs::DRAG_FORCEID].unitDirection = dragDirection;
	GetRBForcesManager(rb).alwaysActiveForces[LinearForceIDs::DRAG_FORCEID].magnitude = 0.5f * speed * GetRBDrag(rb);
}

void PhysicsSystem::ApplyGravityForce(Rigidbody2D& rb) {
	float gravityForce = GetRBGravityScale(rb) * gravity * GetRBMass(rb);
	GetRBForcesManager(rb).alwaysActiveForces[LinearForceIDs::GRAVITY_FORCEID].magnitude = gravityForce;
}

void PhysicsSystem::UpdateRBPosition(Rigidbody2D& rb, float fixedDt, Entity entity) {
	if (fixedDt < 0.2f) {
		// Calculate resultant force of all active forces acting on the Rigidbody.
		ForcesManager& forcesManager = GetRBForcesManager(rb);
		forcesManager.resultantForce = 0;
		std::map<size_t, LinearForce> linearForces = forcesManager.linearForces;
		for (auto& linearForce : linearForces) {
			if (linearForce.second.isActive) {
				forcesManager.resultantForce += (linearForce.second.unitDirection * linearForce.second.magnitude);
				UpdateLinearForceLifetime(GetRBForcesManager(rb), linearForce.first, fixedDt);
			}
		}

		// Update resultant force after applying all always active forces.
		std::map<size_t, LinearForce>& alwaysActiveForces = GetRBForcesManager(rb).alwaysActiveForces;
		for (auto& alwaysActiveForce : alwaysActiveForces) {
			if (alwaysActiveForce.first == LinearForceIDs::DRAG_FORCEID)
				CalculateDragForce(rb, forcesManager.resultantForce, fixedDt);
			else if (alwaysActiveForce.first == LinearForceIDs::GRAVITY_FORCEID) {
				ApplyGravityForce(rb);
			}

			forcesManager.resultantForce += (alwaysActiveForce.second.unitDirection * alwaysActiveForce.second.magnitude);
		}

		// Apply the resultant force on the Rigidbody's velocity.
		Vec2 acceleration = forcesManager.resultantForce / GetRBMass(rb);
		SetRBVelocity(rb, GetRBVelocity(rb) + (acceleration * fixedDt));
		//if (IsRBGrounded(rb) && GetRBVelocity(rb).y < 0.f && GetRBGravityScale(rb) > 0.f) {
		//	SetRBVelocity(rb, GetRBVelocity(rb).x, 0);
		//}
	}

	rb.oldPosition = GetRBPosition(entity);
	// update the position of the rigidbody based on its final velocity.
	SetRBPosition(entity, GetRBPosition(entity) + (GetRBVelocity(rb) * fixedDt));
	//// Update the collider based on the rigidbody position.
	//SetAABBColliderPos(entity, GetRBPosition(entity) + ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity).offset);
}

//void PhysicsSystem::AddContact(Collider2D& thisCollider, Collider2D& otherCollider) {
//	thisCollider.contacts.insert(otherCollider);
//}
//
//void PhysicsSystem::RemoveContact(Collider2D& thisCollider, Collider2D& otherCollider) {
//	if (thisCollider.contacts.find(otherCollider) != thisCollider.contacts.end())
//		thisCollider.contacts.erase(otherCollider);
//}

void PhysicsSystem::UpdateAABBCollider(Entity entity) {
	AABBCollider2D& collider = ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity);
	Rigidbody2D& rb = ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity);

	// If the position of the entity changed
	if (rb.oldPosition != rb.position) {
		// Update the collider's center position based on the rigidbody position and the collider's offset.
		collider.centerPos = rb.position + collider.offset;
		// Update the collider's min and max points based on the collider's updated center position.
		collider.min = Vec2{ (collider.centerPos.x - (collider.sizeX / 2.f)), (collider.centerPos.y - (collider.sizeY / 2.f)) };
		collider.max = Vec2{ (collider.centerPos.x + (collider.sizeX / 2.f)), (collider.centerPos.y + (collider.sizeY / 2.f)) };

		collider.isUpdated = true;
	}
	//// Else if the position of the entity didn't change (either the collider's min/max got manually adjusted, or the rigidbody is stationary).
	//else {
	//	// Update the collider's size in case the collider's min/max got manually adjusted.
	//	collider.sizeX = collider.max.x - collider.min.x;
	//	collider.sizeY = collider.max.y - collider.min.y;
	//	// Calculate the new collider center position and offset based on the new min/max and size.
	//	CalculateColliderPosOffset(entity);
	//}
}

void PhysicsSystem::CalculateColliderPosOffset(Entity entity) {
	AABBCollider2D& collider = ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity);
	collider.centerPos = Vec2{ (collider.min.x + collider.sizeX / 2.f),
								(collider.min.y + collider.sizeY / 2.f) };
	auto& transform = ECSManager::GetInstance().GetComponent<Transform>(entity);
	collider.offset = collider.centerPos - Vec2{ transform.position };
}