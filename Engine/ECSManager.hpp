#ifndef ECS_MANAGER_HPP
#define ECS_MANAGER_HPP

#include <memory>

#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "SystemManager.hpp"

class ECSManager {
public:
	void Initialize() {
		// Create pointers to each manager
		m_componentManager = std::make_unique<ComponentManager>();
		m_entityManager = std::make_unique<EntityManager>();
		m_systemManager = std::make_unique<SystemManager>();
	}


	// Entity methods
	Entity CreateEntity() {
		return m_entityManager->CreateEntity();
	}

	void DestroyEntity(Entity entity) {
		m_entityManager->DestroyEntity(entity);

		m_componentManager->EntityDestroyed(entity);

		m_systemManager->EntityDestroyed(entity);
	}


	// Component methods
	template<typename T>
	void RegisterComponent() {
		m_componentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(Entity entity, T component) {
		m_componentManager->AddComponent<T>(entity, component);

		auto signature = m_entityManager->GetSignature(entity);
		signature.set(m_componentManager->GetComponentType<T>(), true);
		m_entityManager->SetSignature(entity, signature);

		m_systemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(Entity entity) {
		m_componentManager->RemoveComponent<T>(entity);

		auto signature = m_entityManager->GetSignature(entity);
		signature.set(m_componentManager->GetComponentType<T>(), false);
		m_entityManager->SetSignature(entity, signature);

		m_systemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& GetComponent(Entity entity) {
		return m_componentManager->GetComponent<T>(entity);
	}

	//template<typename T>
	//T& TryGetComponent(Entity entity) {
	//	try {
	//		return m_componentManager->GetComponent<T>(entity);
	//	}
	//	catch () {
	//		return;
	//	}
	//}

	template<typename T>
	ComponentType GetComponentType() {
		return m_componentManager->GetComponentType<T>();
	}


	// System methods
	template<typename T>
	std::shared_ptr<T> RegisterSystem() {
		return m_systemManager->RegisterSystem<T>();
	}

	template<typename T>
	void SetSystemSignature(Signature signature) {
		m_systemManager->SetSignature<T>(signature);
	}

private:
	std::unique_ptr<ComponentManager> m_componentManager;
	std::unique_ptr<EntityManager> m_entityManager;
	std::unique_ptr<SystemManager> m_systemManager;
};

#endif // !COORDINATOR_H