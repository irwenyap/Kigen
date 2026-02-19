/*********************************************************************
 * \file	Collision.cs
 * \brief	Defines various structs to describe collisions detected between
            two entities.
 *
 * \author	Wong Woon Li, woonli.wong, 2301308
 * \email	woonli.wong@digipen.edu
 * \date	20 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

using System.Runtime.InteropServices;
using System;
using ScriptCore;

namespace ScriptCore
{
    /**
    * \struct ContactPointCPP
    * \brief A bridging struct that stores all necessary information about a ContactPoint
    *  struct that was passed from C++ to C# for scripting purposes.
    *
    * To be able to pass information about a ContactPoint that was detected in C++ to C#
    * (for usage in the OnCollision or OnTrigger functions), this struct contains all
    * necessary variables about the ContactPoint that was safely converted from C++ to C#.
    * (This struct contains variables that were safely converted from the ContactPointCS
    * struct (defined in C++ in Collision.hpp) to C#).
    */
    [StructLayout(LayoutKind.Sequential)] // Ensure the struct layout is the same as in C++ for safe conversion.
    public struct ContactPointCPP
    {
        public Vec2 point; // The point of collision in world space.
        public Vec2 normal; // normal of the collision relative to aabb1
        public float penetration; // penetration along the collision normal.
        public UInt32 thisEntity, otherEntity;
    }

    /**
    * \struct CollisionCPP
    * \brief A bridging struct that stores all necessary information about a Collision
    *  struct that was passed from C++ to C# for scripting purposes.
    *
    * To be able to pass information about a Collision that was detected in C++ to C#
    * (for usage in the OnCollision or OnTrigger functions), this struct contains all
    * necessary variables about the Collision that was safely converted from C++ to C#.
    * (This struct will contain variables that were safely converted from the CollisionCS
    * struct (defined in C++ in Collision.hpp) to C#).
    */
    [StructLayout(LayoutKind.Sequential)] // Ensure the struct layout is the same as in C++ for safe conversion.
    public struct CollisionCPP
    {
        public UInt32 thisEntity;
        public UInt32 otherEntity; // The other entity hit.
        public Vec2 impulse;   // The resultant impulse applied to this rigidbody to resolve the collision.
        public Vec2 relativeVelocity; // The relative velocity of the two collided objects.
        public ContactPointCPP contactPoint; // Contact point.
    }

    /**
    * \struct ContactPoint
    * \brief Contains scriptable information about the point of contact of a collision.
    *
    * Stores the point, normal and penetration of the collision relative to thisCollider
    * and also the colliders involved.
    */
    public struct ContactPoint
    {
        public Vec2 point; // The point of collision in world space.
        public Vec2 normal; // normal of the collision relative to aabb1
        public float penetration; // penetration along the collision normal.
        public AABBCollider2D thisCollider;
        public AABBCollider2D otherCollider;
    }

    /**
    * \struct Collision
    * \brief Contains scriptable information about a collision between two entities.
    *
    * The Collision struct stores information about a collision event relative to one of the entities,
    * including the involved collider, rigidbody, and the physical properties of the collision.
    */
    public struct Collision
    {
        public AABBCollider2D collider; // The other collider hit.
        public Rigidbody2D rigidbody; // The other rigidbody hit.
        public Vec2 impulse;   // The resultant impulse applied to this rigidbody to resolve the collision.
        public Vec2 relativeVelocity; // The relative velocity of the two collided objects.
        public ContactPoint contactPoint; // Contact point.
    }

    /**
    * \struct ColliderCPP
    * \brief A bridging struct to pass colliders from C++ to C#.
    *
    * Safely passes the entity ID from C++ to C# which can then be used to get the entity's
    * collider component in C#.
    */
    public struct ColliderCPP
    {
        public UInt32 otherEntity; // The other entity hit.
    }
}