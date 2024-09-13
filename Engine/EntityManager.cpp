#include "EntityManager.hpp"
#include <cassert>

EntityManager::EntityManager() {
	for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
		m_availableEntities.push(entity);
	}
}

EntityManager::~EntityManager() {
}

Entity EntityManager::CreateEntity() {
	assert(m_livingEntityCount < MAX_ENTITIES);

	Entity id = m_availableEntities.front();
	m_availableEntities.pop();
	++m_livingEntityCount;

	return id;
}

void EntityManager::DestroyEntity(Entity entity) {
	assert(entity < MAX_ENTITIES && "Entity out of range.");

	m_signatures[entity].reset();

	m_availableEntities.push(entity);
	--m_livingEntityCount;

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