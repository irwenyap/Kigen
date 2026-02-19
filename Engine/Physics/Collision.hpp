/*********************************************************************
 * \file	Collision.hpp
 * \brief	Defines various structs that contain information about a collision
 *			between two entities.
 *
 * \author	Wong Woon Li, woonli.wong, 2301308
 * \email	woonli.wong@digipen.edu
 * \date	20 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "../Core/Vec.hpp"
#include "../Components/Rigidbody2D.hpp"
#include "../ECS/Entity.hpp"

struct AABBCollider2D;

/**
 * \struct ContactPoint
 * \brief Contains information about the point of contact of a collision.
 *
 * Stores the point, normal and penetration of the collision relative to thisCollider
 * and also stores pointers to the colliders involved.
 */
struct ContactPoint {
	Vec2 point; // The point of collision in world space.
	Vec2 normal; // normal of the collision relative to aabb1
	float penetration; // penetration along the collision normal.
	std::weak_ptr<AABBCollider2D> thisCollider;
	std::weak_ptr<AABBCollider2D> otherCollider;
};

/**
 * \struct Collision
 * \brief Contains information about a collision between two entities.
 *
 * The Collision struct stores information about a collision event,
 * including the entities involved, their colliders, rigidbodies, and
 * the physical properties of the collision.
 */
struct Collision {
	static const float edgeCollisionThreshold; // Threshold to ignore collision on edges to prevent collision bugs at the expense of collision accuracy.
	static const float noCollisionDurationThreshold; // How long the collider can have no collisions before it is considered not colliding.
	float noCollisionDuration = 0.f;

	Entity entity; // The other entity hit.
	std::weak_ptr<AABBCollider2D> collider; // The other collider hit.
	std::weak_ptr<Rigidbody2D> rigidbody; // The other rigidbody hit.
	Vec2 impulse;	// The resultant impulse applied to this rigidbody to resolve the collision.
	Vec2 relativeVelocity; // The relative velocity of the two collided objects.
	ContactPoint contactPoint; // Contact point.
	bool resolved = false; // Whether the collision between the 2 entities have been resolved.
	bool isTrigger; // Whether the collision involves an IsTrigger collider.

	/**
	 * \brief Overload == operator to compare between two collisions.
	 * \param rhs The other collision to compare against.
	 * \return True if the entities are the same.
	 */
	bool operator==(const Collision& rhs) const {
		return entity == rhs.entity;
	}

	/**
	 * \brief Overload == operator to compare between two collisions.
	 * \param rhs The other collision to compare against.
	 * \return True if the entities are the same.
	 */
	void operator=(const Collision& rhs) {
		entity = rhs.entity;
		noCollisionDuration = rhs.noCollisionDuration;
		impulse = rhs.impulse;
		relativeVelocity = rhs.relativeVelocity;
		contactPoint = rhs.contactPoint;
		resolved = rhs.resolved;
		isTrigger = rhs.isTrigger;
	}

	/**
	 * \brief Overload < operator.
	 * \param rhs The other collision to compare against.
	 * \return True if this collision is less than the other.
	 */
	bool operator<(const Collision& rhs) const {
		return entity < rhs.entity;
	}
};

/**
 * \struct ContactPointCS
 * \brief A bridging struct that contains all necessary information about a ContactPoint
 *  struct to be able to safely pass to C# for scripting purposes.
 *
 * To be able to pass information about a ContactPoint that was detected in C++ to C#
 * (for usage in the OnCollision or OnTrigger functions), this struct contains all
 * necessary variables about the ContactPoint that can be safely converted to C#.
 * (This struct will be safely converted to the ContactPointCPP struct in Collision.cs).
 */
struct ContactPointCS {
	Vec2 point; // The point of collision in world space.
	Vec2 normal; // normal of the collision relative to aabb1
	float penetration; // penetration along the collision normal.
	Entity thisEntity, otherEntity;
};

/**
 * \struct CollisionCS
 * \brief A bridging struct that contains all necessary information about a Collision
 *  struct to be able to safely pass to C# for scripting purposes.
 *
 * To be able to pass information about a Collision that was detected in C++ to C#
 * (for usage in the OnCollision or OnTrigger functions), this struct contains all
 * necessary variables about the Collision that can be safely converted to C#.
 * (This struct will be safely converted to the CollisionCPP struct in Collision.cs).
 */
struct CollisionCS {
	Entity thisEntity, otherEntity;
	Vec2 impulse;	// The resultant impulse applied to this rigidbody to resolve the collision.
	Vec2 relativeVelocity; // The relative velocity of the two collided objects.
	ContactPointCS contactPoint; // Contact point.
};

/**
 * \struct ColliderCS
 * \brief A bridging struct to pass colliders from C++ to C#.
 *
 * Safely passes the entity ID from C++ to C# which can then be used to get the entity's
 * collider component in C#. (This struct will be safely converted to the ColliderCPP struct
 * in Collision.cs).
 */
struct ColliderCS {
	Entity otherEntity; // The other collider involved in the collision.
};

#endif