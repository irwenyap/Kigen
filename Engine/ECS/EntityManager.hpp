/*********************************************************************
 * \file		EntityManager.hpp
 * \brief		Manages all the entities of the ECS architecture
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include <array>
#include <queue>

#include "Entity.hpp"
#include "Signature.hpp"
#include "../Layers/Layer.hpp"

 /**
  * \class EntityManager
  * \brief Handles creation, destruction, and management of entities in the ECS system.
  *
  * The EntityManager maintains a pool of available entities, assigns unique IDs
  * to new entities, tracks entity signatures, and manages entity layers and active states.
  */
class EntityManager {
public:
	/**
	 * \brief Constructs an EntityManager, initializing internal data structures.
	 */
	EntityManager();

	/**
	 * \brief Destructs the EntityManager, releasing resources.
	 */
	~EntityManager();

	/**
	 * \brief Creates a new entity and assigns a unique ID.
	 *
	 * Entities are reused from a pool of available IDs to minimize fragmentation.
	 *
	 * \return The ID of the newly created entity.
	 */
	Entity CreateEntity();

	/**
	 * \brief Destroys an existing entity and recycles its ID.
	 *
	 * The destroyed entity is removed from active management and its ID is
	 * added back to the pool of available entities.
	 *
	 * \param entity The ID of the entity to destroy.
	 */
	void DestroyEntity(Entity entity);

	/**
	 * \brief Retrieves the signature associated with an entity.
	 *
	 * A signature defines the components associated with an entity.
	 *
	 * \param entity The ID of the entity.
	 * \return The signature of the specified entity.
	 */
	Signature GetSignature(Entity entity);

	/**
		 * \brief Sets the signature for a specific entity.
		 *
		 * This method is used to define the components associated with an entity.
		 *
		 * \param entity The ID of the entity.
		 * \param signature The signature to assign to the entity.
		 */
	void SetSignature(Entity entity, Signature signature);

	/**
	 * \brief Retrieves the total number of living entities.
	 *
	 * \return The number of currently active entities.
	 */
	uint32_t GetEntities() const;

	/**
	 * \brief Destroys all entities managed by the EntityManager.
	 */
	void DestroyAllEntities();

	/**
	 * \brief Assigns a layer to a specific entity.
	 *
	 * Layers are used to organize entities logically (e.g., rendering order).
	 *
	 * \param entity The ID of the entity.
	 * \param layer The layer to assign to the entity.
	 */
	void SetLayer(Entity entity, const Layer layer);

	/**
	 * \brief Retrieves the layer assigned to a specific entity.
	 *
	 * \param entity The ID of the entity.
	 * \return The layer assigned to the entity.
	 */
	Layer GetLayer(Entity entity) const;

	/**
	 * \brief Sets the active state of an entity.
	 *
	 * An active entity is considered alive and will be processed by the ECS system.
	 *
	 * \param entity The ID of the entity.
	 * \param active The active state to set for the entity.
	 */
	void SetActive(Entity entity, bool active);

	/**
	 * \brief Checks whether an entity is active.
	 *
	 * \param entity The ID of the entity.
	 * \return True if the entity is active, false otherwise.
	 */
	bool GetActive(Entity entity);

private:
	static constexpr Layer NO_LAYER = MAX_LAYERS; ///< Default value indicating no layer assigned.
	std::array<Layer, MAX_ENTITIES> entityLayers; ///< Stores the layer of each entity.

	std::queue<Entity> m_availableEntities{}; ///< Pool of available entity IDs.
	std::bitset<MAX_ENTITIES> m_activeEntities; ///< Tracks active entities.

	std::array<Signature, MAX_ENTITIES> m_signatures{}; ///< Signatures for all entities.

	uint32_t m_livingEntityCount{}; ///< Total count of currently active entities.
};

#endif // !ENTITY_MANAGER_HPP
