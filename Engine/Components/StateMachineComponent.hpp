/*********************************************************************
 * \file	StateMachineComponent.hpp
 * \brief	Defines the State Machine Component
 *
 * \author	irwinjun.l, 2301305
 * \email	irwinjun.l@digipen.edu
 * \date	31 October 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*********************************************************************/

#ifndef StateMachineComponent_HPP
#define StateMachineComponent_HPP

#include "../StateMachine/StateMachine.hpp"

struct StateMachineComponent {
    uint32_t entityID; // Store the entity ID directly
    std::shared_ptr<StateMachine<Entity>> stateMachine;
    std::unordered_map<std::string, std::shared_ptr<State<Entity>>> states;
    std::vector<std::tuple<std::string, std::string, std::function<bool(Entity*)>>> transitions;


    // Default constructor
    StateMachineComponent() : stateMachine(nullptr) {}

    // Constructor with entity ID
    explicit StateMachineComponent(uint32_t id)
        : entityID(id),
        stateMachine(std::make_shared<StateMachine<Entity>>(nullptr)) {
    }

    void Initialize(Entity* owner) {
        stateMachine->SetOwner(owner); // Set owner for the state machine
    }

};

#endif // AI_HPP
