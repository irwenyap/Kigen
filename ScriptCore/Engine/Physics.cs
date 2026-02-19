/*********************************************************************
 * \file	Physics.cs
 * \brief	Defines a Physics class that links the Physics Engine into C#.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	08 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ScriptCore
{
    public class Physics
    {

        public enum LinearForces : ulong
        {
            DRAG = ulong.MaxValue,
            GRAVITY = 1,
            MOVERIGHT,
            MOVELEFT,
            JUMP,
            STARTING
        };

        //public static void ActivateForce(UInt32 id, LinearForces LinearForceID)
        //{
        //    InternalCalls.PhysicsSystem_ActivateForce(id,LinearForceID);
        //}

        //public static void SetRBGrounded(UInt32 id, bool b)
        //{
        //    InternalCalls.PhysicsSystem_SetRBGrounded(id, b);
        //}

        //public static void AddRigidbodyComponent(UInt32 id, Vec2 pos, Vec2 vel, float mass, float drag, float gravityScale, bool isStatic = false, bool isKinematic = false) 
        //{ 
        //    InternalCalls.PhysicsSystem_AddRigidbodyComponent(id,pos,vel,mass,drag,gravityScale,isStatic,isKinematic);
        //}

        //public static void AddImpulseForce(UInt32 id, Vec2 dir,float mag)
        //{
        //    InternalCalls.PhysicsSystem_AddImpulseForce(id,dir,mag);
        //}

        //public static void AddAABBColliderComponent(UInt32 id,float bounciness, Vec2 min, Vec2 max)
        //{
        //    InternalCalls.PhysicsSystem_AddAABBColliderComponent(id,bounciness,min,max);
        //}

        //public static void AddDragAndGravity(UInt32 id)
        //{
        //    InternalCalls.PhysicsSystem_AddDragAndGravity(id);
        //}


    }
}
