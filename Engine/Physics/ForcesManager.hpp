/*********************************************************************
 * \file	ForcesManager.hpp
 * \brief	Contains the definitions of structs to manage forces acting
 *			on Rigidbodies.
 *
 * \author	Wong Woon Li, woonli.wong, 2301308
 * \email	woonli.wong@digipen.edu
 * \date	20 October 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#ifndef FORCES_MANAGER_HPP
#define FORCES_MANAGER_HPP

#include <map>
#include <queue>
#include "Vec2.hpp"

/**
 * \struct LinearForce
 * \brief Linear directional force that applies to the entire Rigidbody.
 *
 * Contains variables that define how a linear force should act on a Rigidbody, such as
 * the unit direction, magnitude and lifetime.
 */
struct LinearForce {
	Vec2 unitDirection; // Direction of the force.
	float magnitude; // Magnitude of the force.
	bool isAlwaysActive; // Whether the force will always be active (if true, the active lifetime of the force will be disregarded).
	bool isForever; // If true, this force will exist in ForcesManager for the entire lifetime of the entity.
					// Else, this force will be removed from ForcesManager once the lifetime is up.
	float activeLifetime; // How long this force will be active.
	float currentLifetime; // How long this force has been active.
	bool isActive;	// Whether the force is active and will be applied to the Rigidbody.

	LinearForce() = default;

	/**
	 * \brief Constructs a LinearForce with the parameters provided.
	 *
	 * \param _unitDirection Unit direction of the force.
	 * \param _magnitude Magnitude of the force.
	 * \param _isForever Whether the force will exist for the entire lifetime of the rigidbody.
	 * \param _activeLifetime How long the force will be active.
	 * \param _isActive Whether the force will start off active.
	 * \param _isAlwaysActive Whether the force is always active.
	 */
	LinearForce(Vec2 _unitDirection, float _magnitude, bool _isForever = false, float _activeLifetime = 0.f, bool _isActive = true, bool _isAlwaysActive = false) :
		unitDirection{ _unitDirection }, magnitude{ _magnitude }, activeLifetime{ _activeLifetime }, isForever{ _isForever },
		currentLifetime{ 0.f }, isActive{ _isActive }, isAlwaysActive{ _isAlwaysActive } {}
};

// 
/**
 * \enum LinearForceIDs
 * \brief Reserve the smallest size_t values for the ID (key) of common linear forces that typically have an infinite lifetime.
 *  Drag will have the largest possible key as it should be the last force applied.
 */
enum LinearForceIDs : size_t {
	DRAG_FORCEID = static_cast<size_t>(-1),
	GRAVITY_FORCEID = 1,
	MOVERIGHT_FORCEID,
	MOVELEFT_FORCEID,
	JUMP_FORCEID,
	STARTING_FORCEID
};

/**
 * \struct ForcesManager
 * \brief Manages the forces acting on a Rigidbody.
 *
 * Contains the linearForces and alwaysActiveForces map that stores the forces acting on
 * a Rigidbody. Also has a queue to store freeIDs that can be re-used by newly added forces.
 * Stores the resultant force applied to a Rigidbody after applying all forces acting on it.
 */
struct ForcesManager {
	size_t currLinearForceID = LinearForceIDs::STARTING_FORCEID;
	std::queue<size_t> freeIDs;
	std::map<size_t, LinearForce> linearForces;
	std::map<size_t, LinearForce> alwaysActiveForces;
	Vec2 resultantForce;
};

#endif