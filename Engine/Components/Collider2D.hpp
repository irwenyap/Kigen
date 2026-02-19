/*********************************************************************
 * \file	Collider2D.hpp
 * \brief	Defines the base Collider2D and AABBCollider2D component.
 *
 * \author	Wong Woon Li, woonli.wong, 2301308
 * \email	woonli.wong@digipen.edu
 * \date	1 September 2024
 
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef COLLIDER2D_HPP
#define COLLIDER2D_HPP

#include <vector>
#include <cassert>
#include "../Physics/Collision.hpp"

/**
 * \enum ColliderType
 * \brief An enum for the different collider types the engine has.
 *
 * Currently, only AABB is implemented in the engine. Other types will be added on as and
 * when necessary.
 */
enum ColliderType {
	AABB,
	CIRCLE // Not used yet.
};

/**
 * \struct Collider2D
 * \brief Base class for all 2D collider components in the engine.
 *
 * This struct contains the basic variables that all derived collider types will inherit from.
 */
struct Collider2D {
	float bounciness{}; // range from 0 to 1 with 0 being no bounciness, 1 being perfectly bouncy.
	bool isTrigger = false;
	Vec2 centerPos;
	Vec2 offset;
	bool isUpdated = false;

	size_t idInGrid = static_cast<size_t>(-1);
	std::pair<size_t, size_t> rowsOccupied, colsOccupied;
	bool inGrid = false;

	std::vector<Collision> collisions;
	std::vector<Collision> staticCollisions;

	Collider2D() = default;

	/**
	 * \brief Constructs a Collider2D based on the bounciness.
	 * \param _bounciness Bounciness of the collider (0 = no bounciness, 1 = perfectly bouncy).
	 * \param _isTrigger Whether the collider is a trigger.
	 */
	Collider2D(float _bounciness, bool _isTrigger) : 
		bounciness{ _bounciness }, isTrigger{ _isTrigger } {
		assert((bounciness >= 0.f && bounciness <= 1.f) && "Bounciness must be between 0 to 1.");
	}

	virtual ~Collider2D() = default;
};

/**
 * \struct AABBCollider2D
 * \brief Defined an axis-aligned bounding box 2D collider component.
 *
 * This struct represents a AABB collider that can be attached to entities. It is defined by
 * the min and max bounds of the box, its size, as well as its center position.
 */
struct AABBCollider2D : Collider2D {
	Vec2 min{}; // minimum points of the AABBCollider2D.
	Vec2 max{}; // maximum points of the AABBCollider2D.
	float sizeX{}, sizeY{}; // x-size and y-size of the collider.

	AABBCollider2D() = default;

	/**
	 * \brief Constructs a AABBCollider2D based on the bounciness, min and max.
	 * \param _bounciness Bounciness of the collider (0 = no bounciness, 1 = perfectly bouncy).
	 * \param _min World minimum position of the AABBCollider2D.
	 * \param _max World maximum position of the AABBCollider2D.
	 */
	AABBCollider2D(float _bounciness, const Vec2& _min, const Vec2& _max, bool _isTrigger) :
		min{ _min }, max{ _max }, sizeX{ max.x - min.x }, sizeY{ max.y - min.y }, Collider2D(_bounciness, _isTrigger) {}

	virtual ~AABBCollider2D() = default;
};

#endif