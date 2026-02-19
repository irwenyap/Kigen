/*********************************************************************
 * \file		ComponentManager.hpp
 * \brief		Manages all the components of the ECS architecture
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

#ifndef COMPONENT_MANAGER_HPP
#define COMPONENT_MANAGER_HPP

#include <memory>
#include <optional>
#include <assert.h>
#include <string>
#include <functional>

#include "Component.hpp"
#include "ComponentArray.hpp"

namespace {
	template <typename T>
	std::string GetReadableTypeName() {
		const char* rawName = typeid(T).name();
		std::string typeName = rawName;

		if (typeName.find("struct ") == 0) {
			typeName = typeName.substr(7);
		} else if (typeName.find("class ") == 0) {
			typeName = typeName.substr(6);
		}
		return typeName;
	}
}

/**
 * \class ComponentManager
 * \brief Manages the registration, addition, removal, and access of components for entities.
 *
 * The ComponentManager stores component arrays for different component types and provides
 * methods to register new component types, add/remove components for entities, and access components.
 */
class ComponentManager {
public:

	/**
	 * \brief Registers a new component type with the manager.
	 *
	 * \tparam T The type of the component to register.
	 */
	template<typename T>
	void RegisterComponent() {
		std::string typeName = GetReadableTypeName<T>();

		assert(componentTypes.find(typeName) == componentTypes.end() && "Registering component type more than once.");

		// Add this component type to the component type map
		componentTypes.insert({ typeName, nextComponentType });

		// Create a ComponentArray pointer and add it to the component arrays map
		componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

		// Increment the value so that the next component registered will be different
		++nextComponentType;
	}

	/**
	 * \brief Retrieves the component type ID for a given component type.
	 *
	 * \tparam T The type of the component.
	 * \return The ComponentType ID for the component.
	 */
	template<typename T>
	ComponentType GetComponentType() {
		std::string typeName = GetReadableTypeName<T>();

		assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered before use.");

		// Return this component's type - used for creating signatures
		return componentTypes[typeName];
	}

	/**
	 * \brief Adds a component to an entity.
	 *
	 * \tparam T The type of the component.
	 * \param entity The entity to which the component is added.
	 * \param component The component instance to add.
	 */
	template<typename T>
	void AddComponent(Entity entity, T component) {
		// Add a component to the array for an entity
		GetComponentArray<T>()->InsertData(entity, component);
	}

	/**
	 * \brief Removes a component from an entity.
	 *
	 * \tparam T The type of the component.
	 * \param entity The entity from which the component is removed.
	 */
	template<typename T>
	void RemoveComponent(Entity entity) {
		// Remove a component from the array for an entity
		GetComponentArray<T>()->RemoveData(entity);
	}


	/**
	 * \brief Retrieves a reference to a component attached to an entity.
	 *
	 * \tparam T The type of the component.
	 * \param entity The entity to which the component is attached.
	 * \return A reference to the component.
	 */
	template<typename T>
	T& GetComponent(Entity entity) {
		// Get a reference to a component from the array for an entity
		return GetComponentArray<T>()->GetData(entity);
	}

	template <typename T>
	std::optional<std::reference_wrapper<T>> TryGetComponent(Entity entity) {
		return GetComponentArray<T>()->TryGetData(entity);
	}

	/**
	 * \brief Notifies the manager that an entity has been destroyed, removing all of its components.
	 *
	 * \param entity The entity that was destroyed.
	 */
	void EntityDestroyed(Entity entity) {
		// Notify each component array that an entity has been destroyed
		// If it has a component for that entity, it will remove it
		for (auto const& pair : componentArrays) {
			auto const& component = pair.second;

			component->EntityDestroyed(entity);
		}
	}

	void AllEntitiesDestroyed() {
		for (auto const& pair : componentArrays) {
			auto const& component = pair.second;
			component->AllEntitiesDestroyed();
		}
	}

private:
	std::unordered_map<std::string, ComponentType> componentTypes{}; /**< Maps component type names to ComponentType IDs. */
	std::unordered_map<std::string, std::shared_ptr<IComponentArray>> componentArrays{}; /**< Maps component type names to their corresponding component arrays. */
	ComponentType nextComponentType{}; /**< The next available component type ID to assign. */

	/**
	 * \brief Retrieves the component array for a specific component type.
	 *
	 * \tparam T The type of the component array.
	 * \return A shared pointer to the ComponentArray for the specified component type.
	 */
	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray() {
		std::string typeName = GetReadableTypeName<T>();

		assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered before use.");

		return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
	}
};

#endif // !COMPONENT_MANAGER_H