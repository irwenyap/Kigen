/*********************************************************************
 * \file	StateMachine.hpp
 * \brief	Contains the customStates that the game will have and their logic.
 *
 * \author	irwinjun.l, 2301305
 * \email	irwinjun.l@digipen.edu
 * \date	31 October 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*********************************************************************/
#ifndef CUSTOMSTATE_HPP
#define CUSTOMSTATE_HPP
#include "StateMachine.hpp"
#include "../ECS/Entity.hpp"
#include "../Tools/ImGui/imgui.h"
#include <functional>
#include <iostream>
// States

// Idle State
class IdleState : public State<Entity> {
public:
    void OnEnter(Entity* owner) override {
        (void)owner;
        std::cout << "Entering Idle State\n";
    }

    void OnUpdate(Entity* owner, double dt) override {
        (void)owner; (void)dt;
        std::cout << "Updating Idle State\n";

        // Use ECSManager to get the StateMachineComponent associated with this entity
        auto& stateMachineComponent = ECSManager::GetInstance().GetComponent<StateMachineComponent>(*owner);

        // Trigger a manual transition if the "=" key is pressed
        if (ImGui::IsKeyPressed(ImGuiKey_Minus)) {
            stateMachineComponent.stateMachine->TriggerManualTransition("Move");
        }
    }

    void OnExit(Entity* owner) override {
        (void)owner;
        std::cout << "Exiting Idle State\n";
    }

    std::string GetName() const override {
        return "Idle";
    }
};

// Move State
class MoveState : public State<Entity> {
public:
    void OnEnter(Entity* owner) override {
        (void)owner;
        std::cout << "Entering Move State\n";
    }

    void OnUpdate(Entity* owner, double dt) override {
        (void)dt;
        std::cout << "Updating Move State\n";

        // Use ECSManager to get the StateMachineComponent associated with this entity
        auto& stateMachineComponent = ECSManager::GetInstance().GetComponent<StateMachineComponent>(*owner);

        // Trigger a manual transition if the "=" key is pressed
        if (ImGui::IsKeyPressed(ImGuiKey_Minus)) {
            stateMachineComponent.stateMachine->TriggerManualTransition("Idle");
        }
    }

    void OnExit(Entity* owner) override {
        (void)owner;
        std::cout << "Exiting Move State\n";
    }

    std::string GetName() const override {
        return "Move";
    }
};

class StateFactory {
public:
    static std::unordered_map<std::string, std::function<std::shared_ptr<State<Entity>>()>>& GetRegistry() {
        static std::unordered_map<std::string, std::function<std::shared_ptr<State<Entity>>()>> registry;
        return registry;
    }

    template <typename T>
    static void RegisterState(const std::string& name) {
        GetRegistry()[name] = []() {
            return std::make_shared<T>();
            };
    }
};

// Register states in a global scope
inline void RegisterAllStates() {
    StateFactory::RegisterState<IdleState>("Idle");
    StateFactory::RegisterState<MoveState>("Move");
}

//Transitions

#endif // CUSTOMSTATE_HPP
