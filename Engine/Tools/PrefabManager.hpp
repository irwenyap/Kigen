/*********************************************************************
 * \file		PrefabManager.hpp
 * \brief		Manages the creation, updating, and unlinking of
 *              prefab entities in the ECS system.
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef PREFAB_MANAGER
#define PREFAB_MANAGER

#include <unordered_map>
#include <vector>
#include <string>

class PrefabManager {
public:
	/**
	 * \class PrefabManager
	 * \brief Singleton class responsible for managing prefabs and their entities within the ECS system.
	 */
	static PrefabManager& GetInstance();

	/**
	 * \brief Updates all entities associated with a prefab to match the components of a source entity.
	 *
	 * This method ensures that all entities linked to a prefab have their components synchronized
	 * with the source entity's components. Components not present in the source entity are removed
	 * from the linked entities.
	 *
	 * \param prefabID The identifier of the prefab to update.
	 * \param entity The source entity whose components will be used for the update.
	 */
	void UpdatePrefab(const std::string& prefabID, const uint32_t& entity);

	/**
	 * \brief Unlinks an entity from a prefab.
	 *
	 * Removes the specified entity from the prefab's list of linked entities,
	 * breaking the association between the entity and the prefab.
	 *
	 * \param prefabID The identifier of the prefab.
	 * \param entity The entity to unlink from the prefab.
	 */
	void UnlinkPrefab(const std::string& prefabID, const uint32_t& entity);

	std::unordered_map<std::string, std::vector<uint32_t>> prefabsMap;
};


#endif // !PREFAB_MANAGER