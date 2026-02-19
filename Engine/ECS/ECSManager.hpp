/*********************************************************************
 * \file		ECSManager.hpp
 * \brief		Core of the ECS architecture handling all the
 *				entities, components and systems
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

#ifndef ECS_MANAGER_HPP
#define ECS_MANAGER_HPP

#include <memory>

#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "SystemManager.hpp"

#include "../Graphics/RenderSystem.hpp"
#include "../Physics/PhysicsSystem.hpp"
#include "../Graphics/UISystem.hpp"
#include "../Systems/TransformSystem.hpp"
#include "../Systems/AnimationSystem.hpp"
#include "../Systems/AudioSystem.hpp"
#include "../Systems/CameraSystem.hpp"
#include "../Systems/VideoPlayerSystem.hpp"
#include "../Systems/StateMachineSystem.hpp"

 /**
  * \class ECSManager
  * \brief Manages the core operations of an Entity Component System (ECS) architecture.
  *
  * ECSManager is responsible for creating and destroying entities, registering and managing
  * components, and registering and configuring systems. It provides an interface for interacting
  * with the EntityManager, ComponentManager, and SystemManager.
  */
class ECSManager {
public:

	// Delete copy constructor and assignment operator to prevent copying
	ECSManager(const ECSManager&) = delete;
	ECSManager& operator=(const ECSManager&) = delete;

	// Static method to get the single instance of ECSManager
	static ECSManager& GetInstance();

	/**
	 * \brief Initializes the ECSManager by creating instances of EntityManager, ComponentManager, and SystemManager.
	 */
	void Initialize();

	/**
	 * \brief Creates a new entity.
	 * \return The created Entity object.
	 */
	Entity CreateEntity();

	/**
	 * \brief Destroys the given entity and updates relevant managers.
	 *
	 * \param entity The entity to be destroyed.
	 */
	void DestroyEntity(Entity entity);

	/**
	 * \brief Destroys all entities and updates relevant managers.
	 */
	void ClearEntities();

	/**
	 * \brief Registers a new component type with the ECS.
	 *
	 * \tparam T The component type to register.
	 */
	template<typename T>
	void RegisterComponent() {
		m_componentManager->RegisterComponent<T>();
	}

	/**
	 * \brief Adds a component to an entity.
	 *
	 * \tparam T The type of the component.
	 * \param entity The entity to which the component is added.
	 * \param component The component to add.
	 */
	template<typename T>
	void AddComponent(Entity entity, T component) {
		m_componentManager->AddComponent<T>(entity, component);

		auto signature = m_entityManager->GetSignature(entity);
		signature.set(m_componentManager->GetComponentType<T>(), true);
		m_entityManager->SetSignature(entity, signature);

		m_systemManager->EntitySignatureChanged(entity, signature);
	}

	/**
	 * \brief Removes a component from an entity.
	 *
	 * \tparam T The type of the component to remove.
	 * \param entity The entity from which the component is removed.
	 */
	template<typename T>
	void RemoveComponent(Entity entity) {
		m_componentManager->RemoveComponent<T>(entity);

		auto signature = m_entityManager->GetSignature(entity);
		signature.set(m_componentManager->GetComponentType<T>(), false);
		m_entityManager->SetSignature(entity, signature);

		m_systemManager->EntitySignatureChanged(entity, signature);
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
		return m_componentManager->GetComponent<T>(entity);
	}

	template <typename T>
	std::optional<std::reference_wrapper<T>> TryGetComponent(Entity entity) {
		return m_componentManager->TryGetComponent<T>(entity);
	}

	template <typename T>
	bool HasComponent(Entity entity) {
		if (m_componentManager->TryGetComponent<T>(entity)) {
			return true;
		}
		else {
			return false;
		}
	}

	template<typename T>
	ComponentType GetComponentType() {
		return m_componentManager->GetComponentType<T>();
	}

	/**
	 * \brief Registers a system with the ECS.
	 *
	 * \tparam T The type of the system to register.
	 * \return A shared pointer to the registered system.
	 */
	template<typename T>
	std::shared_ptr<T> RegisterSystem() {
		return m_systemManager->RegisterSystem<T>();
	}

	/**
	 * \brief Sets the component signature for a system.
	 *
	 * \tparam T The type of the system.
	 * \param signature The component signature to associate with the system.
	 */
	template<typename T>
	void SetSystemSignature(Signature signature) {
		m_systemManager->SetSignature<T>(signature);
	}

	EntityManager& GetEntityManager() {
		return *m_entityManager;
	}

	size_t GetNumOfSystems() {
		return m_systemManager->GetNumOfSystems();
	}

	std::shared_ptr<RenderSystem> renderSystem;
	std::shared_ptr<PhysicsSystem> physicsSystem;
	std::shared_ptr<TransformSystem> transformSystem;
	std::shared_ptr<UISystem> uiSystem;
	std::shared_ptr<AnimationSystem> animationSystem;
	std::shared_ptr<AudioSystem> audioSystem;
	std::shared_ptr<CameraSystem> cameraSystem;
	std::shared_ptr<VideoPlayerSystem> videoPlayerSystem;
	std::shared_ptr<StateMachineSystem> stateMachineSystem;

private:
	ECSManager() {
		Initialize();
	};
	~ECSManager();

	std::unique_ptr<EntityManager> m_entityManager;			/**< The manager responsible for creating and destroying entities. */
	std::unique_ptr<ComponentManager> m_componentManager;	/**< The manager responsible for registering and managing components. */
	std::unique_ptr<SystemManager> m_systemManager;			/**< The manager responsible for registering and managing systems. */
};

#endif // !ECS_MANAGER_HPP