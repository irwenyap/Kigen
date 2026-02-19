/*********************************************************************
 * \file		SystemManager.hpp
 * \brief		Manages all the systems of the ECS architecture
 *
 * \author		Yap Zi Yang Irwen, y.ziyangirwen, 2301345
 * \email		y.ziyangirwen@digipen.edu
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef SYSTEM_MANAGER_HPP
#define SYSTEM_MANAGER_HPP

#include <memory>
#include <unordered_map>

#include "Signature.hpp"
#include "System.hpp"

/**
 * \class SystemManager
 * \brief Manages the registration, signatures, and entity associations of all systems in the ECS framework.
 */
class SystemManager {
public:
	/**
	 * \brief Registers a new system of type T.
	 *
	 * Creates and stores a system of the specified type, ensuring it can be accessed and managed.
	 * Systems should be registered only once.
	 *
	 * \tparam T The type of the system to register.
	 * \return A shared pointer to the newly created system.
	 */
	template<typename T>
	std::shared_ptr<T> RegisterSystem() {
		const char* typeName = typeid(T).name();

		assert(systems.find(typeName) == systems.end() && "Registering system more than once.");

		// Create a pointer to the system and return it so it can be used externally
		auto system = std::make_shared<T>();
		systems.insert({ typeName, system });
		return system;
	}

    /**
     * \brief Assigns a signature to a registered system.
     * 
     * The signature defines the component requirements for entities to be managed by the system.
     * 
     * \tparam T The type of the system.
     * \param signature The signature associated with the system.
     */
	template<typename T>
	void SetSignature(Signature signature) {
		const char* typeName = typeid(T).name();

		assert(systems.find(typeName) != systems.end() && "System used before registered.");

		// Set the signature for this system
		signatures.insert({ typeName, signature });
	}

	/**
	 * \brief Handles the destruction of an entity.
	 *
	 * Removes the entity from all system-managed entity sets.
	 *
	 * \param entity The entity that was destroyed.
	 */
	void EntityDestroyed(Entity entity) {
		// Erase a destroyed entity from all system lists
		// mEntities is a set so no check needed
		for (auto const& pair : systems) {
			auto const& system = pair.second;

			system->m_entities.erase(entity);
		}
	}

	/**
	 * \brief Clears all entities from all systems.
	 *
	 * Used to handle scenarios where all entities are destroyed at once.
	 */
	void AllEntitiesDestroyed() {
		for (auto const& pair : systems) {
			auto const& system = pair.second;

			system->m_entities.clear();
		}
	}

	/**
	 * \brief Updates systems when an entity's signature changes.
	 *
	 * Notifies all systems of an entity's signature change and adjusts their entity sets accordingly.
	 *
	 * \param entity The entity whose signature changed.
	 * \param entitySignature The new signature of the entity.
	 */
	void EntitySignatureChanged(Entity entity, Signature entitySignature) {
		// Notify each system that an entity's signature changed
		for (auto const& pair : systems) {
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = signatures[type];

			// Entity signature matches system signature - insert into set
			if ((entitySignature & systemSignature) == systemSignature) {
				system->m_entities.insert(entity);
			}
			// Entity signature does not match system signature - erase from set
			else {
				system->m_entities.erase(entity);
			}
		}
	}

	size_t GetNumOfSystems() {
		return systems.size();
	}

private:
	/**
	 * \brief Map from system type name to its signature.
	 *
	 * Signatures define the component requirements for each system.
	 */
	std::unordered_map<const char*, Signature> signatures{};

	/**
	 * \brief Map from system type name to the system instance.
	 *
	 * Stores all registered systems, allowing them to be managed collectively.
	 */
	std::unordered_map<const char*, std::shared_ptr<System>> systems{};
};

#endif // !SYSTEM_MANAGER_H