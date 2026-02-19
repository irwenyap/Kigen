/*********************************************************************
 * \file		EntityManager.cpp
 * \brief		Manages all the entities of the ECS architecture
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
#include "EntityManager.hpp"
#include <cassert>

EntityManager::EntityManager() {
	for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
		m_availableEntities.push(entity);
	}
	for (uint32_t entity = 0; entity < MAX_ENTITIES; entity++) {
		entityLayers[entity] = NO_LAYER;
	}
}

EntityManager::~EntityManager() {
}

Entity EntityManager::CreateEntity() {
	assert(m_livingEntityCount < MAX_ENTITIES);

	Entity id = m_availableEntities.front();
	m_availableEntities.pop();
	++m_livingEntityCount;

	m_activeEntities[id] = true;

	return id;
}

void EntityManager::DestroyEntity(Entity entity) {
	assert(entity < MAX_ENTITIES && "Entity out of range.");

	m_signatures[entity].reset();

	m_availableEntities.push(entity);
	--m_livingEntityCount;

	m_activeEntities[entity] = false;
}

Signature EntityManager::GetSignature(Entity entity) {
	assert(entity < MAX_ENTITIES && "Entity out of range.");

	// Get this entity's signature from the array
	return m_signatures[entity];

}

void EntityManager::SetSignature(Entity entity, Signature signature) {
	assert(entity < MAX_ENTITIES && "Entity out of range.");

	// Put this entity's signature into the array
	m_signatures[entity] = signature;
}

uint32_t EntityManager::GetEntities() const {
	return m_livingEntityCount;
}

void EntityManager::DestroyAllEntities() {
	for (auto& signature : m_signatures) {
		signature.reset();
	}

	m_activeEntities.reset();

	while (!m_availableEntities.empty()) {
		m_availableEntities.pop();
	}
	for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
		m_availableEntities.push(entity);
	}

	m_livingEntityCount = 0;
}

void EntityManager::SetLayer(Entity entity, const Layer layer)
{
	entityLayers[entity] = layer;
}

Layer EntityManager::GetLayer(Entity entity) const
{
	return entityLayers[entity];
}

void EntityManager::SetActive(Entity entity, bool active) {
	assert(entity < MAX_ENTITIES && "Entity out of range.");
	m_activeEntities[entity] = active;
}

bool EntityManager::GetActive(Entity entity) {
	assert(entity < MAX_ENTITIES && "Entity out of range.");
	return m_activeEntities[entity];
}
