/*********************************************************************
 * \file	StateMachine.hpp
 * \brief	Contains the generic stateMachine class, state class and transition class.
 *
 * \author	irwinjun.l, 2301305
 * \email	irwinjun.l@digipen.edu
 * \date	31 October 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*********************************************************************/
#ifndef STATEMACHINE_HPP
#define STATEMACHINE_HPP

#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <iostream>  // For debugging or logging

template <typename T>
class State {
public:
    virtual ~State() = default;
    virtual void OnEnter(T* owner) = 0;
    virtual void OnUpdate(T* owner, double dt) = 0;
    virtual void OnExit(T* owner) = 0;
    virtual std::string GetName() const = 0;
};

template<typename T>
class Transition {
public:
    Transition(const std::string& targetState, std::function<bool(T*)> cond)
        : targetStateName(targetState), condition(std::move(cond)) {}

    bool CanTransition(T* owner) const {
        return condition ? condition(owner) : false;
    }
    // Accessors
    const std::string& GetTargetState() const { return targetStateName; }
    const std::function<bool(T*)>& GetCondition() const { return condition; }

private:
    std::string targetStateName;
    std::function<bool(T*)> condition;
};

template<typename T>
class StateMachine {
public:
    explicit StateMachine(T* owner) : owner(owner), currentState(nullptr) {}

    void SetOwner(T* newOwner) {
        owner = newOwner;
    }

    T* GetOwner() const {
        return owner;
    }

    //Adding, Getting and Removing States
    void AddState(std::shared_ptr<State<T>> state) {
        states[state->GetName()] = state;
    }

    const std::unordered_map<std::string, std::shared_ptr<State<T>>>& GetStates() const {
        return states;
    }

    void RemoveState(const std::string& stateName) {
        // Erase the state from the states map
        states.erase(stateName);

        // Remove all transitions involving the state
        transitions.erase(stateName); // Remove outgoing transitions
        for (auto& [_, transitionList] : transitions) {
            // Remove transitions pointing to the deleted state
            transitionList.erase(
                std::remove_if(
                    transitionList.begin(),
                    transitionList.end(),
                    [&stateName](const Transition<T>& transition) {
                        return transition.GetTargetState() == stateName;
                    }),
                transitionList.end());
        }

        // Reset the current state if it matches the removed state
        if (currentState && currentState->GetName() == stateName) {
            currentState = nullptr;
        }
    }

    // Define automated transitions with conditions (checked every update loop)
    void AddAutomatedTransition(const std::string& from, const std::string& to, std::function<bool(T*)> condition) {
        transitions[from].emplace_back(to, condition);
    }

    // Trigger a manual transition (1 off transition etc..)
    bool TriggerManualTransition(const std::string& targetStateName) {
        if (states.find(targetStateName) != states.end()) {
            TransitionTo(targetStateName);
            return true;
        }
        return false;
    }

    const std::unordered_map<std::string, std::vector<Transition<T>>>& GetTransitions() const {
        return transitions;
    }

    void SetInitialState(const std::string& stateName) {
        if (states.find(stateName) != states.end()) {
            currentState = states[stateName].get();
            currentState->OnEnter(owner);
        }
    }

    std::string GetCurrentStateName() const {
        return currentState ? currentState->GetName() : "None";
    }

    void Update(double dt) {
        if (!currentState) return;

        // Handle transitions
        auto it = transitions.find(currentState->GetName());
        if (it != transitions.end()) {
            for (const auto& transition : it->second) {
                if (transition.CanTransition(owner)) {
                    TransitionTo(transition.GetTargetState());
                    return;
                }
            }
        }

        // Update the current state
        currentState->OnUpdate(owner, dt);
    }



private:
    void TransitionTo(const std::string& newStateName) {
        if (currentState) {
            currentState->OnExit(owner);
        }
        currentState = states[newStateName].get();
        if (currentState) {
            currentState->OnEnter(owner);
        }
    }

    T* owner;
    State<T>* currentState = nullptr;
    std::unordered_map<std::string, std::shared_ptr<State<T>>> states;
    std::unordered_map<std::string, std::vector<Transition<T>>> transitions;
};

#endif // STATEMACHINE_HPP

