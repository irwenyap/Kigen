/*********************************************************************
 * \file		ComponentArray.hpp
 * \brief		Manages the storage and retrieval of components for entities.
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

#ifndef COMPONENT_ARRAY_HPP
#define COMPONENT_ARRAY_HPP

#include "Entity.hpp"

#include <unordered_map>
#include <assert.h>
#include <optional>
#include <functional>

#include "Logger.hpp"

/**
 * \class IComponentArray
 * \brief Abstract base class that defines the interface for a component array.
 *
 * This class provides a virtual method to handle the removal of components when an entity is destroyed.
 */
class IComponentArray {
public:
    /**
     * \brief Virtual destructor.
     */
    virtual ~IComponentArray() = default;

    /**
     * \brief Handles the removal of a component for a destroyed entity.
     * \param entity The entity that was destroyed.
     */
    virtual void EntityDestroyed(Entity entity) = 0;
    virtual void AllEntitiesDestroyed() = 0;
};

/**
 * \class ComponentArray
 * \brief Manages the storage and retrieval of components for entities.
 *
 * The ComponentArray class stores components of a specific type for all entities that have that component.
 * It provides methods for adding, removing, and accessing components, and manages the mappings between
 * entities and component array indices.
 *
 * \tparam T The type of the component stored in the array.
 */
template<typename T>
class ComponentArray : public IComponentArray {
public:
    /**
     * \brief Inserts a new component for the specified entity.
     *
     * \param entity The entity for which the component is added.
     * \param component The component instance to be added.
     */
    inline void InsertData(Entity entity, T component) {
        //assert(entityToIndexMap.find(entity) == entityToIndexMap.end() && "Component added to same entity more than once.");
        if (entityToIndexMap.find(entity) != entityToIndexMap.end()) {
            Logger::Instance().Log(Logger::Level::ERR,
                "Attempting to add component to the same entity more than once!");
            return;
        }

        size_t newIndex = size;
        entityToIndexMap[entity] = newIndex;
        indexToEntityMap[newIndex] = entity;
        componentArray[newIndex] = component;
        ++size;
    }

    /**
     * \brief Removes the component for the specified entity.
     *
     * \param entity The entity for which the component is removed.
     */
    inline void RemoveData(Entity entity) {
        //assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Removing non-existent component.");
        //if (entityToIndexMap.find(entity) != entityToIndexMap.end()) {
        //    Logger::Instance().Log(Logger::Level::ERR, "Removing non-existent component from the entity!");
        //    return;
        //}
        if (entityToIndexMap.find(entity) == entityToIndexMap.end()) {
            Logger::Instance().Log(Logger::Level::ERR,
                "Attempting to remove non-existent component of the entity!");
            return; // Skip adding the component
        }

        size_t indexOfRemovedEntity = entityToIndexMap[entity];
        size_t indexOfLastElement = size - 1;
        componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement];

        Entity entityOfLastElement = indexToEntityMap[indexOfLastElement];
        entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
        indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

        entityToIndexMap.erase(entity);
        indexToEntityMap.erase(indexOfLastElement);

        --size;
    }

    /**
     * \brief Retrieves the component for the specified entity.
     *
     * \param entity The entity to retrieve the component for.
     * \return A reference to the component.
     */
    inline T& GetData(Entity entity) {
        assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Retrieving non-existent component.");
        //if (entityToIndexMap.find(entity) != entityToIndexMap.end()) {
        //    return componentArray[entityToIndexMap[entity]];
        //}

        return componentArray[entityToIndexMap[entity]];
    }

    inline std::optional<std::reference_wrapper<T>> TryGetData(Entity entity) {
        if (entityToIndexMap.find(entity) != entityToIndexMap.end()) {
            return componentArray[entityToIndexMap[entity]];
        }
        return std::nullopt;
    }

    /**
     * \brief Handles the destruction of an entity by removing its component.
     *
     * \param entity The entity that was destroyed.
     */
    inline void EntityDestroyed(Entity entity) override {
        if (entityToIndexMap.find(entity) != entityToIndexMap.end())
            RemoveData(entity);
    }

    inline void AllEntitiesDestroyed() override {
        entityToIndexMap.clear();
        indexToEntityMap.clear();
        std::fill(componentArray.begin(), componentArray.end(), T{});
        size = 0;
    }

private:
    std::array<T, MAX_ENTITIES> componentArray; /**< Array storing the components for all entities. */
    std::unordered_map<Entity, size_t> entityToIndexMap; /**< Maps entities to indices in the component array. */
    std::unordered_map<size_t, Entity> indexToEntityMap; /**< Maps indices in the component array back to entities. */

    size_t size; /**< The current number of components stored in the array. */
};

#endif // COMPONENT_ARRAY_HPP
