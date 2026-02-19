/*********************************************************************
 * \file	StateMachineSystem.hpp
 * \brief	System to handle all StateMachine Components
 *
 * \author	irwinjun.l, 2301305
 * \email	irwinjun.l@digipen.edu
 * \date	1 January 2025

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*********************************************************************/
#include "StateMachineSystem.hpp"
#include "../ECS/ECSManager.hpp"

void StateMachineSystem::Init() {
    for (auto const& entity : m_entities) {
        auto& stateMachineComponent = ECSManager::GetInstance().GetComponent<StateMachineComponent>(entity);
        if (stateMachineComponent.stateMachine) {
            stateMachineComponent.stateMachine->SetInitialState("Idle");
        }
    }
}


void StateMachineSystem::Update(double dt) {
    auto& ecsManager = ECSManager::GetInstance();

    for (auto const& entity : m_entities) {
        if (!ecsManager.GetEntityManager().GetActive(entity)) continue;

        auto& stateMachineComponent = ecsManager.GetComponent<StateMachineComponent>(entity);
        if (stateMachineComponent.stateMachine) {
            stateMachineComponent.stateMachine->Update(dt);
        }
    }
}

void StateMachineSystem::AddState(Entity entity, std::shared_ptr<State<Entity>> state) {
    auto& ecsManager = ECSManager::GetInstance();
    auto& stateMachineComponent = ecsManager.GetComponent<StateMachineComponent>(entity);

    // Add the state directly to the state machine
    stateMachineComponent.stateMachine->AddState(state);
}

void StateMachineSystem::AddAutomatedTransition(Entity entity, const std::string& from, const std::string& to, std::function<bool(Entity*)> condition) {
    auto& ecsManager = ECSManager::GetInstance();
    auto& stateMachineComponent = ecsManager.GetComponent<StateMachineComponent>(entity);

    // Add the automated transition to the state machine
    stateMachineComponent.stateMachine->AddAutomatedTransition(from, to, condition);
}

void StateMachineSystem::TriggerManualTransition(Entity entity, const std::string& to) {
    auto& ecsManager = ECSManager::GetInstance();
    auto& stateMachineComponent = ecsManager.GetComponent<StateMachineComponent>(entity);

    // Trigger a manual transition in the state machine
    stateMachineComponent.stateMachine->TriggerManualTransition(to);
}

void StateMachineSystem::Exit() {
    // Cleanup logic for the system, if needed
}