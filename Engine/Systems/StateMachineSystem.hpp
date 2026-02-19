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
#ifndef STATEMACHINE_SYSTEM_HPP
#define STATEMACHINE_SYSTEM_HPP

#include "../ECS/System.hpp"
#include "../Components/StateMachineComponent.hpp"

/**
 * \class StateMachineSystem
 * \brief System that updates all entities with StateMachineComponent.
 */
class StateMachineSystem : public System {
public:
    /**
     * \brief Initializes the state machine system.
     */
    void Init();

    /**
     * \brief Updates all state machines managed by the system.
     * \param dt The delta time (time elapsed since the last update).
     */
    void Update(double dt);

    /**
     * \brief Cleans up resources when the system exits.
     */
    void Exit();

    /**
     * \brief Adds a new state to the state machine of a specified entity.
     * \param entity The entity whose state machine will receive the new state.
     * \param name The name of the new state.
     * \param state A shared pointer to the state to be added.
     */
    void AddState(Entity entity, std::shared_ptr<State<Entity>> state);

    /**
     * \brief Adds an automated transition between states in the state machine of a specified entity.
     * \param entity The entity whose state machine will receive the transition.
     * \param from The name of the starting state for the transition.
     * \param to The name of the target state for the transition.
     * \param condition The condition function to trigger the transition.
     */
    void AddAutomatedTransition(Entity entity, const std::string& from, const std::string& to, std::function<bool(Entity*)> condition);

    /**
     * \brief Triggers a manual transition to a target state for the specified entity.
     * \param entity The entity whose state machine will be transitioned.
     * \param to The name of the target state for the transition.
     */
    void TriggerManualTransition(Entity entity, const std::string& to);
};

#endif // STATEMACHINE_SYSTEM_HPP