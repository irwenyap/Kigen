/*********************************************************************
 * \file	PhysicsSystem.hpp
 * \brief	Contains declaration of the functions of the Physics system.
 *
 * \author	Wong Woon Li, woonli.wong, 2301308
 * \email	woonli.wong@digipen.edu
 * \date	1 September 2024
 
Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#ifndef PHYSICS_SYSTEM_HPP
#define PHYSICS_SYSTEM_HPP

#include <vector>
#include <functional>
#include "BitsetGrid.hpp"
#include "../ECS/System.hpp"
#include "../Components/Collider2D.hpp"
#include "../Components/Rigidbody2D.hpp"

/**
 * \class PhysicsSystem
 * \brief System that handles all the physics calculations for entities in the engine.
 *
 * This class contains all the functionalities for physics-related components, namely the
 * Rigidbody2D and AABBCollider2D. Also responsible for handling the detection and resolution
 * of collisions between entities with such components.
 */
class PhysicsSystem : public System {
public:
	/**
	 * \brief Initializes the Physics system.
	 */
	void Init();

	/**
	 * \brief Adds drag and gravity force as an always active force to the Rigidbody.
	 *
	 * \param rb Reference to the Rigidbody.
	 */
	void AddDragAndGravity(Rigidbody2D& rb);

	/**
	 * \brief Updates the Physics components of the physics objects in the game.
	 * 
	 * \param dt Time between current frame and previous frame.
	 */
	void Update(double dt);

	/**
	 * \brief Terminates the Physics system.
	 */
	void Exit();

	/**
	 * \brief Return true if step by step mode is enabled and false otherwise.
	 * \return True if step by step mode is enabled and false otherwise.
	 */
	bool IsStepByStepMode();

	/**
	 * \brief Sets step by step mode to the boolean value.
	 * 
	 * \param _bool Boolean value to set step by step mode to.
	 */
	void SetStepByStepMode(bool _bool);

	/**
	 * \brief Returns whether broad-phase collision detection optimisation is on or off.
	 * \return True if enabled and false otherwise.
	 */
	bool IsBroadPhaseMode();

	/**
	 * \brief Sets broad-phase collision detection optimisation on or off.
	 * 
	 * \param _bool Boolean value to set broad-phase collision detection mode to.
	 */
	void SetBroadPhaseMode(bool _bool);

#pragma region COLLIDER2D FUNCTIONS
	/**
	 * \brief Adds an AABBCollider component with automatic size based on mesh size to the entity.
	 * 
	 * \param entity Entity to add the collider component to.
	 */
	void AddAABBColliderComponent(Entity entity);

	/**
	 * \brief Adds an AABBCollider component with custom initializers to the entity.
	 * 
	 * \param entity Entity to add the collider component to.
	 */
	void AddAABBColliderComponent(Entity entity, float bounciness, const Vec2& min, const Vec2& max, bool isTrigger = false);

	/**
	 * \brief Returns the bounciness value of the entity's AABBCollider.
	 * 
	 * \param entity Entity to check the collider bounciness value.
	 * \return Bounciness value of the entity's AABBCollider.
	 */
	float GetAABBColliderBounciness(Entity entity);

	/**
	 * \brief Sets the bounciness value of the entity's AABBCollider.
	 * 
	 * \param entity Entity to set the bounciness value.
	 * \param val New bounciness value to set to.
	 */
	void SetAABBColliderBounciness(Entity entity, float val);

	/**
	 * \brief Returns whether the entity's collider is a Trigger.
	 * 
	 * \param entity Entity to check the isTrigger boolean.
	 * \return True if collider isTrigger and false otherwise.
	 */
	bool IsAABBColliderTrigger(Entity entity);

	/**
	 * \brief Sets the entity's collider isTrigger status.
	 * 
	 * \param entity Entity to set the isTrigger status.
	 * \param val New boolean value to set the isTrigger status to.
	 */
	void SetAABBColliderTrigger(Entity entity, bool val);

	/**
	 * \brief Returns the entity's AABBCollider min position.
	 * 
	 * \param entity Entity to get the collider min position.
	 * \return Vec2 of the collider min position.
	 */
	Vec2 GetAABBColliderMin(Entity entity);

	/**
	 * \brief Sets the entity's AABBCollider min position.
	 * 
	 * \param entity Entity to set the collider min position.
	 * \param min New min of the collider to set to.
	 */
	void SetAABBColliderMin(Entity entity, const Vec2& min);

	/**
	 * \brief Returns the entity's AABBCollider max position.
	 * 
	 * \param entity Entity to get the collider max position.
	 * \return Vec2 of the collider max position.
	 */
	Vec2 GetAABBColliderMax(Entity entity);

	/**
	 * \brief Sets the entity's AABBCollider max position.
	 * 
	 * \param entity Entity to set the collider max position.
	 * \param min New max of the collider to set to.
	 */
	void SetAABBColliderMax(Entity entity, const Vec2& max);

	/**
	 * \brief Returns the entity's AABBCollider horizontal size (x-size).
	 *
	 * \param entity Entity to get the collider x-size.
	 * \return X-size of the collider.
	 */
	float GetAABBColliderSizeX(Entity entity);

	/**
	 * \brief Returns the entity's AABBCollider vertical size (y-size).
	 *
	 * \param entity Entity to get the collider y-size.
	 * \return Y-size of the collider.
	 */
	float GetAABBColliderSizeY(Entity entity);

	/**
	 * \brief Returns the entity's AABBCollider center position.
	 *
	 * \param entity Entity to get the collider center position.
	 * \return Center position of the collider.
	 */
	Vec2 GetAABBColliderPos(Entity entity);

	/**
	 * \brief Sets the entity's AABBCollider center position.
	 *
	 * \param entity Entity to set the collider center position.
	 * \param pos New position to set to.
	 */
	void SetAABBColliderPos(Entity entity, const Vec2& pos);

	/**
	 * \brief Calculates the offset of the collider relative to the entity's transform.
	 *
	 * \param entity Entity to calculate offset of collider.
	 */
	void CalculateColliderPosOffset(Entity entity);

	/**
	 * \brief Returns a reference to the Entity-To-AABBCollider map.
	 * \return Reference to the Entity-To-AABBCollider map.
	 */
	std::map<Entity, std::shared_ptr<AABBCollider2D>>& GetEntityToAABBMap();

	/**
	 * \brief Returns a reference to the entity's collisions vector.
	 * 
	 * \param entity Entity to get the collisions vector.
	 * \return Reference to the entity's collision vector.
	 */
	std::vector<Collision>& GetCollisions(Entity entity);

	/**
	 * \brief Adds the Collision info to the entity's collisions vector if it is a new
	 *  collision, or update the existing Collision info otherwise. Also invokes the
	 *  OnCollisionEnter, OnTriggerEnter, OnCollisionStay or OnTriggerStay functions on
	 *	any Script component attached to the entity.
	 * 
	 * \param entity Entity to add or update the collision info.
	 * \param collision The new Collision info detected by the physics system on the entity.
	 */
	void AddOrUpdateCollisions(Entity entity, const Collision& collision);

	/**
	 * \brief Returns whether the collision on the entity is a collision with a new entity.
	 * 
	 * \param entity Entity to check whether the collision is a new collision.
	 * \param collision Collision info that was detected by the physics system.
	 * \return True if the collision is with a new entity and false otherwise.
	 */
	bool IsColliderEnter(Entity entity, const Collision& collision);

	/**
	 * \brief Resets all entities' collisions vector and checks for any collision exits.
	 *  If a collision exit is detected, invokes the OnCollisionExit or OnTriggerExit
	 *  function on any Script component attached to the entity.
	 * 
	 * \param dt Time since the last frame.
	 */
	void CleanupCollisions(float dt);

	/**
	 * \brief Checks for collision between 2 rectangular-AABB colliders using the
	 *	Separation-Axis Theorem. Returns true if they are colliding and false
	 *	if they are not. Also returns the firstTimeOfCollision parameter.
	 *
	 * \param aabb1 Reference to the first AABBCollider2D to test for collision. 
	 * \param vel1 Velocity of the first object.
	 * \param aabb2 Reference to the second AABBCollider2D to test for collision.
	 * \param vel2 Velocity of the second object.
	 * \param col1 Reference to the Collision struct for aabb1 that is to be updated
	 *  if collision is detected.
	 * \param dt Time between previous and current frame.
	 * \return True if collision detected and false otherwise.
	 */
	bool DetectCollisionEnterAABB_AABB(std::shared_ptr<AABBCollider2D> aabb1, const Vec2& vel1, std::shared_ptr<AABBCollider2D> aabb2, const Vec2& vel2, Collision& col1, Collision& col2, float dt);

	/**
	 * \brief Resolves collision between two physics objects with non-static
		Rigidbodies.
	 * \param entity1 The first entity involved in the collision.
	 * \param rb1 Reference to the first Rigidbody2D to respond to the collision.
	 * \param bounciness1 Bounciness of the first object.
	 * \param entity2 The second entity involved in the collision.
	 * \param rb2 Reference to the second Rigidbody2D to respond to the collision.
	 * \param bounciness2 Bounciness of the second object.
	 * \param col1 Collision struct containing information about the collision for entity1.
	 * \param col2 Collision struct containing information about the collision for entity2.
	 * \param dt Time between previous and current frame.
	*************************************************************************/
	void ResolveCollision(Entity entity1, Rigidbody2D& rb1, float bounciness1, Entity entity2, Rigidbody2D& rb2, float bounciness2, Collision& col1, Collision& col2, float dt);

	/**
	 * \brief Resolves collision between a non-static and static physics object.
	 * 
	 * \param rb Reference to the Rigidbody2D of the non-static object to respond to
	 *	the collision.
	 * \param bounciness1 Bounciness of the first object.
	 * \param bounciness2 Bounciness of the second object.
	 * \param col1 Collision struct containing information about the collision for entity1.
	 * \param col2 Collision struct containing information about the collision for entity2.
	 * \param dt Time between previous and current frame.
	*************************************************************************/
	void ResolveCollision(Entity entity, Rigidbody2D& rb, float bounciness1, float bounciness2, Collision& col1, Collision& col2, float dt);

	/**
	 * \brief Helper function to coordinate the calling of functions to detect and resolve
	 *  collisions between two entities.
	 * 
	 * \param entity1 The first entity to detect and resolve collision.
	 * \param entity2 The second entity to detect and resolve collision.
	 * \param aabb1 Reference to the first entity's AABBCollider component.
	 * \param aabb2 Reference to the second entity's AABBCollider component.
	 * \param rb1 Reference to the first entity's Rigidbody2D component.
	 * \param rb2 Reference to the second entity's Rigidbody2D component.
	 * \param dt Time between previous and current frame.
	 */
	void DetectAndResolveCollision(Entity entity1, Entity entity2, std::shared_ptr<AABBCollider2D> aabb1, std::shared_ptr<AABBCollider2D> aabb2, Rigidbody2D& rb1, Rigidbody2D& rb2, float dt);

	/**
	 * \brief Helper function to convert the C++ Collision struct into a struct that is
	 *  able to be passed to C# scripts.
	 *
	 * \param thisEntity The entity that the Collision info is relative to.
	 * \param collision The C++ Collision struct to be converted.
	 * \return CollisionCS struct that is able to be passed to C# scripts.
	 */
	CollisionCS ConvertCollisionToCS(Entity thisEntity, const Collision& collision);
#pragma endregion
	 
#pragma region RIGIDBODY FUNCTIONS
	/**
	 * \brief Add a Rigidbody2D component to an entity.
	 *
	 * \param entity The entity to add the Rigidbody2D component to.
	 * \param rb Reference to a Rigidbody2D struct instance to be added to the entity.
	 */
	void AddRigidbodyComponent(Entity entity, Rigidbody2D& rb);

	/**
	 * \brief Add a Rigidbody2D component to an entity based on the parameters.
	 *
	 * \param entity The entity to add the Rigidbody2D component to.
	 * \param _position World position of the Rigidbody.
	 * \param _velocity Starting velocity of the Rigidbody.
	 * \param _mass Mass of the Rigidbody.
	 * \param _drag Drag of the Rigidbody.
	 * \param _gravityScale Gravity multiplier for the Rigidbody.
	 * \param _isStatic Whether the Rigidbody is static (immovable).
	 * \param _isKinematic Whether the Rigidbody is kinematic (no forces and collisions).
	 */
	void AddRigidbodyComponent(Entity entity, Vec2 _position, Vec2 _velocity, float _mass, float _drag, float _gravityScale, bool _isStatic = false, bool _isKinematic = false);
	 
	/**
	 * \brief Return the position of the Rigidbody.
	 *
	 * \param entity The entity to get the Rigidbody position.
	 * \return Position of the Rigidbody.
	 */
	inline Vec2 GetRBPosition(Entity entity);

	/**
	 * \brief Set the entity's Rigidbody position.
	 *
	 * \param entity The entity to set the position of the Rigidbody of.
	 * \param pos New position of the Rigidbody to set to.
	 */
	void SetRBPosition(Entity entity, const Vec2& pos);

	/**
	 * \brief Set the entity's Rigidbody position.
	 *
	 * \param entity The entity to set the position of the Rigidbody of.
	 * \param _x, _y New x and y positions of the Rigidbody to set to.
	 */
	void SetRBPosition(Entity entity, float _x, float _y);

	/**
	 * \brief Return the velocity of the Rigidbody.
	 *
	 * \param entity The entity to get the Rigidbody velocity.
	 * \return Velocity of the Rigidbody.
	 */
	inline Vec2 GetRBVelocity(const Rigidbody2D& rb);

	/**
	 * \brief Set the entity's Rigidbody velocity.
	 *
	 * \param rb The rigidbody to set the velocity of.
	 * \param pos New velocity of the Rigidbody to set to.
	 */
	void SetRBVelocity(Rigidbody2D& rb, const Vec2& vel);

	/**
	 * \brief Set the entity's Rigidbody velocity.
	 *
	 * \param entity The entity to set the velocity of the Rigidbody of.
	 * \param _x, _y New x and y velocity of the Rigidbody to set to.
	 */
	void SetRBVelocity(Rigidbody2D& rb, float _x, float _y);

	/**
	 * \brief Return the mass of the Rigidbody.
	 *
	 * \param rb The rigidbody to get the mass of.
	 * \return Mass of the Rigidbody.
	 */
	inline float GetRBMass(const Rigidbody2D& rb);

	/**
	 * \brief Set the Rigidbody mass.
	 *
	 * \param rb The rigidbody to set the mass of.
	 * \param pos New mass of the Rigidbody to set to.
	 */
	void SetRBMass(Rigidbody2D& rb, float newMass);

	/**
	 * \brief Return the inverse mass of the Rigidbody.
	 *
	 * \param rb The rigidbody to get the inverse mass of.
	 * \return Inverse mass of the Rigidbody.
	 */
	inline float GetRBInverseMass(const Rigidbody2D& rb);

	/**
	 * \brief Return the drag of the Rigidbody.
	 *
	 * \param rb The rigidbody to get the drag of.
	 * \return Drag of the Rigidbody.
	 */
	inline float GetRBDrag(const Rigidbody2D& rb);

	/**
	 * \brief Set the Rigidbody drag.
	 *
	 * \param entity The rigidbody to set the drag of.
	 * \param pos New drag of the Rigidbody to set to.
	 */
	void SetRBDrag(Rigidbody2D& rb, float);

	/**
	 * \brief Return the gravity scale of the Rigidbody.
	 *
	 * \param rb The rigidbody to get the gravity scale of.
	 * \return Gravity scale of the Rigidbody.
	 */
	inline float GetRBGravityScale(const Rigidbody2D& rb);

	/**
	 * \brief Set the Rigidbody gravity scale.
	 *
	 * \param entity The rigidbody to set the gravity scale of.
	 * \param pos New gravity scale of the Rigidbody to set to.
	 */
	void SetRBGravityScale(Rigidbody2D& rb, float newScale);

	/**
	 * \brief Return whether the Rigidbody is static.
	 *
	 * \param rb The rigidbody to check whether is static.
	 * \return Whether the Rigidbody is static.
	 */
	inline bool IsRBStatic(const Rigidbody2D& rb);

	/**
	 * \brief Set the Rigidbody is static status.
	 *
	 * \param entity The rigidbody to set the is static status.
	 * \param pos New is static status of the Rigidbody to set to.
	 */
	void SetRBStatic(Rigidbody2D& rb, bool _bool);

	/**
	 * \brief Return whether the Rigidbody is kinematic.
	 *
	 * \param rb The rigidbody to check whether is kinematic.
	 * \return Whether the Rigidbody is kinematic.
	 */
	bool IsRBKinematic(const Rigidbody2D& rb);

	/**
	 * \brief Set the Rigidbody is kinematic status.
	 *
	 * \param entity The rigidbody to set the is kinematic status.
	 * \param pos New is kinematic status of the Rigidbody to set to.
	 */
	void SetRBKinematic(Rigidbody2D& rb, bool);

	/**
	 * \brief Return whether the Rigidbody is grounded.
	 *
	 * \param rb The rigidbody to check whether is grounded.
	 * \return Whether the Rigidbody is grounded.
	 */
	bool IsRBGrounded(const Rigidbody2D& rb);

	/**
	 * \brief Set the Rigidbody is grounded status.
	 *
	 * \param entity The rigidbody to set the is grounded status.
	 * \param pos New is grounded status of the Rigidbody to set to.
	 */
	void SetRBGrounded(Rigidbody2D& rb, bool);

	/**
	 * \brief Return a reference to the Rigidbody's ForcesManager.
	 *
	 * \param rb The rigidbody to get the ForcesManager.
	 * \return Reference to the Rigidbody's ForcesManager.
	 */
	inline ForcesManager& GetRBForcesManager(Rigidbody2D& rb);

	/**
	 * \brief Return a reference to the Entity-To-Rigidbody map.
	 *
	 * \return Reference to the Entity-To-Rigidbody map.
	 */
	std::map<Entity, std::reference_wrapper<Rigidbody2D>>& GetEntityToRBMap();

	// Functionalities
	/**
	 * \brief Add a force with an auto-assigned ID and return the ID.
	 * 
	 * \param rb Reference to the Rigidbody to add a force to.
	 * \param force LinearForce to add to the Rigidbody ForcesManager.
	 * \return The auto-assigned ID of the force.
	 */
	size_t AddForce(Rigidbody2D& rb, const LinearForce& force);

	/**
	 * \brief Add a force with a specified ID and return the ID.
	 * 
	 * \param rb Reference to the Rigidbody to add a force to.
	 * \param forceID The specified ID to add the force to ForcesManager.
	 * \param force LinearForce to add to the Rigidbody ForcesManager.
	 * \return The auto-assigned ID of the force.
	 */
	size_t AddForce(Rigidbody2D& rb, size_t forceID, const LinearForce& force);	// Add a force with a specified ID and return the ID.

	/**
	 * \brief Add a force with the specified parameters with an auto-assigned ID and return the ID.
	 * 
	 * \param rb Reference to the Rigidbody to add a force to.
	 * \param direction Direction of the force
	 * \param magnitude Magnitude of the force.
	 * \param isForever Whether the force will exist in ForcesManager for the entire lifetime of the Rigidbody.
	 * \param activeLifetime How long the force will be active once activated.
	 * \param isActive Whether the force added will be active straightaway.
	 * \param isAlwaysActive Whether the force added will always be active for the entire lifetime of the Rigidbody.
	 * \return The auto-assigned ID of the force.
	 */
	size_t AddForce(Rigidbody2D& rb, const Vec2& direction, float magnitude, bool isForever, float activeLifetime, bool isActive, bool isAlwaysActive);

	/**
	 * \brief Add a force with the specified parameters with an auto-assigned ID and return the ID.
	 *
	 * \param rb Reference to the Rigidbody to add a force to.
	 * \param forceID The specified ID of the force.
	 * \param direction Direction of the force
	 * \param magnitude Magnitude of the force.
	 * \param isForever Whether the force will exist in ForcesManager for the entire lifetime of the Rigidbody.
	 * \param activeLifetime How long the force will be active once activated.
	 * \param isActive Whether the force added will be active straightaway.
	 * \param isAlwaysActive Whether the force added will always be active for the entire lifetime of the Rigidbody.
	 * \return The ID of the force.
	 */
	size_t AddForce(Rigidbody2D& rb, size_t forceID, const Vec2& direction, float magnitude, bool isForever, float activeLifetime, bool isActive, bool isAlwaysActive);

	/**
	 * \brief Helper function to easily add an impulse force to the Rigidbody.
	 *
	 * \param rb Reference to the Rigidbody to add a force to.
	 * \param direction Direction of the force
	 * \param magnitude Magnitude of the force.
	 * \return The auto-assigned ID of the force.
	 */
	size_t AddImpulseForce(Rigidbody2D& rb, const Vec2& direction, float magnitude);

	/**
	 * \brief Helper function to easily add a force over time to the Rigidbody.
	 *
	 * \param rb Reference to the Rigidbody to add a force to.
	 * \param direction Direction of the force
	 * \param magnitude Magnitude of the force.
	 * \param duration Duration to add the force to the Rigidbody.
	 * \return The auto-assigned ID of the force.
	 */
	size_t AddForceOverTime(Rigidbody2D& rb, const Vec2& direction, float magnitude, float duration);

	/**
	 * \brief Helper function to easily add an always active force to the Rigidbody.
	 *
	 * \param rb Reference to the Rigidbody to add a force to.
	 * \param forceID The specified ID of the force.
	 * \param direction Direction of the force
	 * \param magnitude Magnitude of the force.
	 * \return The ID of the force.
	 */
	size_t AddAlwaysActiveForce(Rigidbody2D& rb, size_t forceID, const Vec2& direction, float magnitude);

	/**
	 * \brief Helper function to easily add an impulse force that exists forever.
	 *
	 * \param rb Reference to the Rigidbody to add a force to.
	 * \param direction Direction of the force
	 * \param magnitude Magnitude of the force.	 
	 * \param startActive Whether the force starts active.
	 * \param forceID The specified ID of the force.
	 * \return The ID of the force.
	 */
	size_t AddForeverImpulseForce(Rigidbody2D& rb, const Vec2& direction, float magnitude, bool startActive, size_t forceID);

	/**
	 * \brief Activates the force with the specified ID.
	 *
	 * \param rb Reference to the Rigidbody to add a force to.
	 * \param forceID The ID of the force to activate.
	 * \param activate Whether to activate or deactivate the force.
	 */
	void ActivateForce(Rigidbody2D& rb, size_t forceID, bool activate);

	/**
	 * \brief Updates the lifetime of the force in the forcesManager.
	 *
	 * \param rb Reference to the ForcesManager to update the forces' lifetime.
	 * \param forceID The ID of the force to update the lifetime.
	 * \param dt Time since the last frame.
	 */
	void UpdateLinearForceLifetime(ForcesManager& forcesManager, size_t forceID, float dt);

	/**
	 * \brief Calculate the Rigidbody's drag force based on the current resultant force acting
	 *  on the body.
	 *
	 * \param rb Reference to the Rigidbody.
	 * \param currResultantForce Current resultant force acting on the body before drag.
	 * \param dt Time since the last frame.
	 */
	void CalculateDragForce(Rigidbody2D& rb, const Vec2& currResultantForce, float dt);

	/**
	 * \brief Calculates the gravity force acting on the Rigidbody and applies it.
	 *
	 * \param rb Reference to the Rigidbody.
	 */
	void ApplyGravityForce(Rigidbody2D& rb);

	/**
	 * \brief Updates the Rigidbody2D position based on gravity, drag and the final
	 *	velocity.
	 * \param rb Reference to the Rigidbody2D to update.
	 * \param dt Time between previous and current frame.
	 */
	void UpdateRBPosition(Rigidbody2D& rb, float dt, Entity entity);
#pragma endregion

	/**
	 * \brief Return a reference to the physics system's BitsetGrid.
	 *
	 * \return Reference to the physics system's BitsetGrid.
	 */
	BitsetGrid& GetBitsetGrid();

	/**
	 * \brief Reset the bits in the BitsetGrid to 0.
	 */
	void ResetBitsetGrid();

	//void RemoveAABBFromMap(Entity entity);

	//void RemoveRBFromMap(Entity entity);

private:
	float gravity = 900.f; // Base gravity value.
	bool stepByStepMode = false;
	bool broadPhaseMode = true;

	float edgeCollisionThreshold = 2.0f; // Threshold to ignore collision on edges to prevent collision bugs at the expense of collision accuracy.
	BitsetGrid bitsetGrid{}; // Grid of bitsets used for broad-phase collision detection optimisation.
	std::map<Entity, std::reference_wrapper<Rigidbody2D>> entityToRBMap; // Maps each entity to a reference to its Rigidbody.
	std::map<Entity, std::shared_ptr<AABBCollider2D>> entityToAABBMap; // Maps each entity to a shared_ptr to its AABBCollider2D.
	std::map<Entity, size_t> entityToGridIDMap;

	// TEMP
	std::optional<Entity> playerEntity = std::nullopt;

	/**
	 * \brief Updates the entity's AABBCollider bounds and size based on the Rigidbody's position.
	 * \param entity Entity to update the AABBCollider.
	 */
	void UpdateAABBCollider(Entity entity);
};

#endif // !PHYSICSSYSTEM_HPP