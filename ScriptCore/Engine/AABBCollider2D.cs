/*********************************************************************
 * \file	AABBColliderComponent.cs
 * \brief	Defines the AABBCollider2D component in C# to allow users to
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
    * \class AABBColliderComponent
    * \brief The representation of an AABBCollider2D component for C# scripting.
    *
    * This class contains the portions of the AABBCollider2D component that is
    * intended to be scriptable by users of the engine. Users can modify the
    * bounciness and bounds of the collider via scripting. Any modifications
    * made to the collider in C# will be reflected to the actual collider in C++.
    */
    public class AABBCollider2D : Component
    {
        public float Bounciness // range from 0 to 1 with 0 being no bounciness, 1 being perfectly bouncy.
        {
            get
            {
                return InternalCalls.AABBColliderComponent_GetBounciness(Entity.ID);
            }
            set { InternalCalls.AABBColliderComponent_SetBounciness(Entity.ID, value); }
        }

        public Vec2 Min // minimum points of the AABBCollider2D.
        {
            get
            {
                InternalCalls.AABBColliderComponent_GetMin(Entity.ID, out Vec2 min);
                return min;
            }
            set { InternalCalls.AABBColliderComponent_SetMin(Entity.ID, value); }
        }

        public Vec2 Max // maximum points of the AABBCollider2D.
        {
            get
            {
                InternalCalls.AABBColliderComponent_GetMax(Entity.ID, out Vec2 max);
                return max;
            }
            set { InternalCalls.AABBColliderComponent_SetMax(Entity.ID, value); }
        }

        public float SizeX // x-size and y-size of the collider.
        {
            get
            {
                return InternalCalls.AABBColliderComponent_GetSizeX(Entity.ID);
            }
        }

        public float SizeY
        {
            get
            {
                return InternalCalls.AABBColliderComponent_GetSizeY(Entity.ID);
            }
        }

        public bool IsTrigger
        {
            get
            {
                return InternalCalls.AABBColliderComponent_GetIsTrigger(Entity.ID);
            }
            set
            {
                InternalCalls.AABBColliderComponent_SetIsTrigger(Entity.ID, value);
            }
        }
    }
}