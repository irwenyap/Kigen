/*********************************************************************
 * \file	TransformSystem.hpp
 * \brief
 *      This file declares the TransformSystem class. 
 * 
 *		The TransformSystem class is responsible for updating the 
 *		position, rotation, and scale of entities. It also updates
 *		each entity's world matrix.
 * 
 * \author	t.yongchin, 2301359 (t.yongchin@digipen.edu)
 * \date	26 November 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#pragma once

#include "../ECS/System.hpp"

#include <unordered_map>

struct Vec3;
struct Transform;

/*!*****************************************************************************
\class TransformSystem
\brief
	A class for managing the position, rotation, and scale of entities.

	The `TransformSystem` class is responsible for updating the position, rotation,
	and scale of entities in the game world. It calculates the world matrix for
	each entity based on its transform components and updates the entity's collider
	accordingly.
*******************************************************************************/
class TransformSystem : public System {
public:
	/*
	* \brief Initialize the TransformSystem
	*/
	void Init();

	/*
	* \brief Update the TransformSystem
	* \param dt the delta time
	*/
	void Update(double dt);

	/*
	* \brief Exit the TransformSystem
	*/
	void Exit();

	/*
	* \brief Set the position of an entity to a specific value
	* 
	* \param entity the entity to set the position of
	* \param position the position to set the entity to
	*/
	void SetPosition(Entity entity, Vec3 const& position);
	/*
	* \brief Set the rotation of an entity to a specific value
	* 
	* \param entity the entity to set the rotation of
	* \param rotation the rotation to set the entity to
	*/
	void SetRotation(Entity entity, Vec3 const& rotation);
	/*
	* \brief Set the scale of an entity to a specific value
	* 
	* \param entity the entity to set the scale of
	* \param scale the scale to set the entity to
	*/
	void SetScale(Entity entity, Vec3 const& scale);

	/*
	* \brief Translate an entity by a specific amount
	* 
	* \param entity the entity to translate
	* \param translation the amount to translate the entity by
	*/
	void Translate(Entity entity, Vec3 const& translation);
	/*
	* \brief Rotate an entity by a specific amount
	* 
	* \param entity the entity to rotate
	* \param rotation the amount to rotate the entity by
	*/
	void Rotate(Entity entity, float const& rotation);
	/*
	* \brief Scale an entity by a specific amount
	* 
	* \param entity the entity to scale
	* \param scale the amount to scale the entity by
	*/
	void Scale(Entity entity, float const& scale);

	/*
	* \brief Update's the entity's collider
	* 
	* \param entity the entity to update the collider of
	*/
	void UpdateAABBCollider(Entity entity);

	/*
	* \brief Calculate the collider's position offset
	*
	* \param entity the entity to calculate the collider's position offset of
	*/
	void CalculateColliderPosOffset(Entity entity);

public:
	static std::unordered_map<uint32_t, uint32_t> uuidToTransformMap;

};
