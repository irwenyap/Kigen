/*********************************************************************
 * \file	Rigidbody2D.hpp
 * \brief	Defines the Rigidbody2D component.
 *
 * \author	Wong Woon Li, woonli.wong, 2301308
 * \email	woonli.wong@digipen.edu
 * \date	1 September 2024
 
Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#ifndef RIGIDBODY2D_HPP
#define RIGIDBODY2D_HPP

#include "../Physics/ForcesManager.hpp"

 /**
  * \struct Rigidbody2D
  * \brief Represents a 2D Rigidbody component that can be added to entities.
  *
  * A Rigidbody2D component allows for physics simulations to be applied to entities
  * by calculating forces and velocities to update the positions of the entity.
  */
struct Rigidbody2D {
	Vec2 oldPosition{ 0.f, 0.f };
	Vec2 position{ 0.0f, 0.0f };
	Vec2 velocity{ 0.0f, 0.0f };
	float mass = 1.f;
	float inverseMass = 1.f;
	float drag = 1.f;
	float gravityScale = 1.0f;
	bool isStatic = false; // if true, rigidbody is treated as an immovable (static) object with infinite mass (for walls, platforms etc).
	bool isKinematic = false; // If isKinematic is enabled, forces and collisions will not affect the rigidbody anymore.
	bool isGrounded = false;
	ForcesManager forcesManager;
	float noCollisionDurationThreshold = 0.27f; // How long the rigidbody can have no collisions before it is considered not colliding.
	float noCollisionDuration = 0.f;

	/**
	 * \brief Default constructs a Rigidbody2D.
	 */
	inline Rigidbody2D() : position{}, velocity{}, mass{}, inverseMass{}, drag{}, gravityScale{ 1.0f },
		isStatic{ false }, isKinematic{ false }, isGrounded{ false }, forcesManager{} {}

	/**
	 * \brief Constructs a Rigidbody2D based on the initializers.
	 * 
	 * \param _position Initial position of the Rigidbody2D.
	 * \param _velocity Initial velocity of the Rigidbody2D.
	 * \param _mass Mass of the Rigidbody2D.
	 * \param _drag Drag of the Rigidbody2D.
	 * \param _gravityScale Gravity scale of the Rigidbody2D.
	 * \param _isStatic If static, rigidbody won't respond to collisions.
	 * \param _isKinematic If kinematic, rigidbody won't detect collisions.
	 */
	inline Rigidbody2D(Vec2 _position, Vec2 _velocity, float _mass, float _drag, float _gravityScale, bool _isStatic = false, bool _isKinematic = false) :
		position{ _position }, velocity{ _velocity }, mass{ _mass }, inverseMass{ 1.f / mass }, drag{ _drag }, gravityScale{ _gravityScale },
		isStatic{ _isStatic }, isKinematic{ _isKinematic }, isGrounded{ false }, forcesManager{} {}

	/**
	 * \brief Constructs a Rigidbody2D based on another Rigidbody2D.
	 * \param other Other Rigidbody2D to construct this one based on.
	 */
	inline Rigidbody2D(const Rigidbody2D& other) : position{ other.position }, velocity{ other.velocity }, mass{ other.mass }, inverseMass{ other.inverseMass }, drag{ other.drag },
		gravityScale{ other.gravityScale }, isStatic{ other.isStatic }, isKinematic{ other.isKinematic }, isGrounded{ other.isGrounded }, forcesManager{} {}

	~Rigidbody2D() = default;
};

#endif