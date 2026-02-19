/*********************************************************************
 * \file	    StateMachine.cs
 * \brief	    Defines a StateMachine class that links the StateMachine into C#.
 *
 *              This class allows logging of messages with different severity
 *              levels (DEBUG, INFO, WARN, ERROR) to the internal logging system.
 *
 * \author	    Irwin Lim Jun
 * \email	    irwinjun.l@digipen.edu
 * \date	    24th Febuary 2025
 *
 * \copyright   Copyright (C) 2024 DigiPen Institute of Technology.
 *              Reproduction or disclosure of this file or its contents without the
 *              prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ScriptCore
{
    /**
     * \class StateMachine
     * \brief Allows dynamic state management for entities in C#.
     *
     * This class provides scripting capabilities for the StateMachineComponent.
     * Users can add states, create transitions, and trigger state changes dynamically.
     * The underlying state machine logic is handled by the C++ engine, while this
     * class serves as a bridge for C# scripting.
     */
    public class StateMachine : Component
    {
        /// <summary>
        /// Gets the current active state name of the entity's state machine.
        /// </summary>
        public string CurrentState
        {
            get { return InternalCalls.StateMachineComponent_GetCurrentState(Entity.ID); }
        }

        /// <summary>
        /// Adds a new state to the entity's state machine.
        /// </summary>
        /// <param name="stateName">The name of the new state.</param>
        public void AddState(string stateName)
        {
            InternalCalls.StateMachineComponent_AddState(Entity.ID, stateName);
        }

        /// <summary>
        /// Adds a transition between two states in the entity's state machine.
        /// </summary>
        /// <param name="from">The name of the starting state.</param>
        /// <param name="to">The name of the target state.</param>
        public void AddTransition(string from, string to)
        {
            InternalCalls.StateMachineComponent_AddTransition(Entity.ID, from, to);
        }

        /// <summary>
        /// Triggers a manual transition from the current state to the specified target state.
        /// </summary>
        /// <param name="targetState">The name of the state to transition to.</param>
        public void TriggerTransition(string targetState)
        {
            InternalCalls.StateMachineComponent_TriggerTransition(Entity.ID, targetState);
        }
    }
}
