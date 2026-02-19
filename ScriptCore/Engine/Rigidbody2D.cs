/*********************************************************************
 * \file	Rigidbody2D.cs
 * \brief	Defines the Rigidbody2D component in C# to allow users to
 *          script.
 *
 * \author	Wong Woon Li, woonli.wong, 2301308
 * \email	woonli.wong@digipen.edu
 * \date	20 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

using ScriptCore;

namespace ScriptCore
{
    /**
    * \class Rigidbody2D
    * \brief The representation of a Rigidbody2D component for C# scripting.
    *
    * This class contains the portions of the Rigidbody2DComponent component that is
    * intended to be scriptable by users of the engine. Users can modify the
    * position, velocity, mass, drag, gravity scale and static/kinematic status of
    * the Rigidbody. They can also call functions to add forces to the rigidbody.
    * Any modifications made to the rigidbody in C# will be reflected in the actual
    * rigidbody in C++.
    */
    public class Rigidbody2D : Component
    {
        public Vec2 Position
        {
            get
            {
                InternalCalls.Rigidbody2DComponent_GetRBPosition(Entity.ID, out Vec2 position);
                return position;
            }
            set { InternalCalls.Rigidbody2DComponent_SetRBPosition(Entity.ID, value); }
        }
        public Vec2 Velocity
        {
            get
            {
                InternalCalls.Rigidbody2DComponent_GetRBVelocity(Entity.ID, out Vec2 velocity);
                return velocity;
            }
            set { InternalCalls.Rigidbody2DComponent_SetRBVelocity(Entity.ID, value); }
        }
        public float Mass
        {
            get { return InternalCalls.Rigidbody2DComponent_GetRBMass(Entity.ID); }
            set { InternalCalls.Rigidbody2DComponent_SetRBMass(Entity.ID, value); }
        }
        public float Drag
        {
            get { return InternalCalls.Rigidbody2DComponent_GetRBDrag(Entity.ID); }
            set { InternalCalls.Rigidbody2DComponent_SetRBDrag(Entity.ID, value); }
        }
        public float GravityScale
        {
            get { return InternalCalls.Rigidbody2DComponent_GetRBGravityScale(Entity.ID); }
            set { InternalCalls.Rigidbody2DComponent_SetGravityScale(Entity.ID, value); }
        }
        public bool IsStatic
        {
            get { return InternalCalls.Rigidbody2DComponent_IsRBStatic(Entity.ID); }
            set { InternalCalls.Rigidbody2DComponent_SetRBStatic(Entity.ID, value); }
        }
        public bool IsKinematic
        {
            get { return InternalCalls.Rigidbody2DComponent_IsRBKinematic(Entity.ID); }
            set { InternalCalls.Rigidbody2DComponent_SetRBKinematic(Entity.ID, value); }
        }

        /**
        * \brief Adds an impulse force to the Rigidbody.
        *
        * \param direction Unit direction of the force to be applied.
        * \param magnitude Magnitude of the force to be applied.
        * \return ID of the force added (storing the return ID is not necessary).
        */
        public ulong AddImpulseForce(Vec2 direction, float magnitude)
        {
            return InternalCalls.Rigidbody2DComponent_AddImpulseForce(Entity.ID, direction, magnitude);
        }

        /**
        * \brief Adds a force over time to the Rigidbody.
        *
        * \param direction Unit direction of the force to be applied.
        * \param magnitude Magnitude of the force to be applied.
        * \param duration Duration of the force to apply to the Rigidbody.
        * \return ID of the force added (storing the return ID is not necessary).
        */
        public ulong AddForceOverTime(Vec2 direction, float magnitude, float duration)
        {
            return InternalCalls.Rigidbody2DComponent_AddForceOverTime(Entity.ID, direction, magnitude, duration);
        }

        /**
        * \brief Adds a force to the Rigidbody that will be always active until deactivated.
        *
        * \param forceID Provide an ID that will be used to access the force at a later time.
        * \param direction Unit direction of the force to be applied.
        * \param magnitude Magnitude of the force to be applied.
        * \return ID of the force added.
        */
        public ulong AddAlwaysActiveForce(ulong forceID, Vec2 direction, float magnitude)
        {
            return InternalCalls.Rigidbody2DComponent_AddAlwaysActiveForce(Entity.ID, forceID, direction, magnitude);
        }

        /**
        * \brief Adds an impulse force that will exist for the entire lifetime of the Rigidbody.
        *
        * \param direction Unit direction of the force to be applied.
        * \param magnitude Magnitude of the force to be applied.
        * \param startActive Whether the force will immediately be active.
        * \param forceID Optional ID of the force you can provide to access the force at a later time.
        * \return ID of the force added.
        */
        public ulong AddForeverImpulseForce(Vec2 direction, float magnitude, bool startActive, ulong forceID = ulong.MaxValue)
        {
            return InternalCalls.Rigidbody2DComponent_AddForeverImpulseForce(Entity.ID, direction, magnitude, startActive, forceID);
        }

        /**
        * \brief Activates/deactivates the force with the specified ID.
        *
        * \param forceID ID of the force to access.
        * \param activate Whether to activate or deactivate the force.
        */
        public void ActivateForce(ulong forceID, bool activate)
        {
            InternalCalls.Rigidbody2DComponent_ActivateForce(Entity.ID, forceID, activate);
        }
    }
}