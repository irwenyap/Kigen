/*********************************************************************
 * \file		ECSManager.cpp
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

#include "ECSManager.hpp"

//temp
#include <iostream>

#include "../Components/Name.hpp"
#include "../Components/Transform.hpp"
#include "../Components/Renderer.hpp"
#include "../Components/Collider2D.hpp"
#include "../Components/Rigidbody2D.hpp"
#include "../Components/ScriptComponent.hpp"
#include "../Components/Textbox.hpp"
#include "../Components/Animation.hpp"
#include "../Components/UI.hpp"
#include "../Components/AudioSource.hpp"
#include "../Components/Camera.hpp"
#include "../Components/StateMachineComponent.hpp"

#include "../Utility/ComponentIDGenerator.hpp"
#include "../Components/VideoPlayer.hpp"

ECSManager& ECSManager::GetInstance() {
	static ECSManager instance;
	return instance;
}

void ECSManager::Initialize() {
	m_entityManager = std::make_unique<EntityManager>();
	m_componentManager = std::make_unique<ComponentManager>();
	m_systemManager = std::make_unique<SystemManager>();

	RegisterComponent<Name>();
	RegisterComponent<Transform>();
	RegisterComponent<Renderer>();
	RegisterComponent<StateMachineComponent>();
	RegisterComponent<AABBCollider2D>();
	RegisterComponent<Rigidbody2D>();
	RegisterComponent<Textbox>();
	RegisterComponent<Animation>();
	RegisterComponent<UI>();
	RegisterComponent<AudioSource>();
	RegisterComponent<ScriptComponent>();
	RegisterComponent<Camera>();
	RegisterComponent<VideoPlayer>();

	transformSystem = RegisterSystem<TransformSystem>();
	{
		Signature signature;
		signature.set(GetComponentType<Transform>());
		SetSystemSignature<TransformSystem>(signature);
	}

	renderSystem = RegisterSystem<RenderSystem>();
	{
		Signature signature;
		signature.set(GetComponentType<Renderer>());
		SetSystemSignature<RenderSystem>(signature);
	}

	physicsSystem = RegisterSystem<PhysicsSystem>();
	{
		Signature signature;
		signature.set(GetComponentType<AABBCollider2D>());
		signature.set(GetComponentType<Rigidbody2D>());
		SetSystemSignature<PhysicsSystem>(signature);
	}

	animationSystem = RegisterSystem<AnimationSystem>();
	{
		Signature signature;
		signature.set(GetComponentType<Renderer>());
		signature.set(GetComponentType<Animation>());
		SetSystemSignature<AnimationSystem>(signature);
	}

	uiSystem = RegisterSystem<UISystem>();
	{
		Signature signature;
		signature.set(GetComponentType<UI>());
		SetSystemSignature<UISystem>(signature);
	}

	audioSystem = RegisterSystem<AudioSystem>();
	{
		Signature signature;
		signature.set(GetComponentType<AudioSource>());
		SetSystemSignature<AudioSystem>(signature);
	}

	cameraSystem = RegisterSystem<CameraSystem>();
	{
		Signature signature;
		signature.set(GetComponentType<Camera>());
		signature.set(GetComponentType<Transform>());
		SetSystemSignature<CameraSystem>(signature);
	}
	videoPlayerSystem = RegisterSystem<VideoPlayerSystem>();
	{
		Signature signature;
		signature.set(GetComponentType<VideoPlayer>());
		SetSystemSignature<VideoPlayerSystem>(signature);
	}
	
	stateMachineSystem = RegisterSystem<StateMachineSystem>();
	{
		Signature signature;
		signature.set(GetComponentType<StateMachineComponent>());
		SetSystemSignature<StateMachineSystem>(signature);
	}
}

Entity ECSManager::CreateEntity() {
	Entity entt = m_entityManager->CreateEntity();

	//std::cout << "Entity ID Created: " << entt << std::endl;

	AddComponent(entt, Name());
	Transform transform;
	transform.uuid = ComponentIDGenerator::GenerateID('t');
	AddComponent(entt, transform);
	return entt;
}

void ECSManager::DestroyEntity(Entity entity) {
	//std::cout << "Entity ID Destroyed: " << entity << std::endl;

	m_entityManager->DestroyEntity(entity);
	m_componentManager->EntityDestroyed(entity);
	m_systemManager->EntityDestroyed(entity);
}

void ECSManager::ClearEntities()
{
	m_entityManager->DestroyAllEntities();
	m_componentManager->AllEntitiesDestroyed();
	m_systemManager->AllEntitiesDestroyed();
}

// Temp to shift
ECSManager::~ECSManager()
{

}

