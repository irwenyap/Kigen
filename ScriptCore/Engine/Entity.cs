/*********************************************************************
 * \file	Entity.cs
 * \brief	Defines a Entity class that setup the for entity system in C#.
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
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ScriptCore;

namespace ScriptCore
{
    public class Entity
    {

        protected Entity() { ID = 0; }
        internal Entity(UInt32 id)
        {
            ID = id;
        }

        public readonly UInt32 ID;

        public bool IsActive
        {

            get
            {
                return InternalCalls.Entity_GetActive(ID);
                
            }
            set
            {
                InternalCalls.Entity_SetActive(ID, value);
            }

        }

        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, componentType);
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            T component = new T() { Entity = this };
            return component;
        }

        public void AddComponent<T>() where T : Component, new()
        {
            Type type = typeof(T);
            InternalCalls.Entity_AddComponent(ID, type);
        }

        public static Entity CreateEntity()
        {
            return new Entity(InternalCalls.Entity_Create());
        }

        public Entity FindEntityByName(string name)
        {
            UInt32 entityID = InternalCalls.Entity_FindEntityByName(name);
            if (entityID == UInt32.MaxValue)
                return null;

            return new Entity(entityID);
        }

        public Entity FindEntityByID(UInt32 id)
        {
            UInt32 entityID = InternalCalls.Entity_FindEntityByID(id);
            if (entityID == UInt32.MaxValue)
                return null;

            return new Entity(entityID);
        }

        public void SetCamPosition(Vec3 Value)
        {
            InternalCalls.Camera_SetPosition(ref Value);
        }

        public T As<T>() where T : Entity, new()
        {
            object instance = InternalCalls.GetScriptInstance(ID);
            return instance as T;
        }


        protected virtual void OnInit()
        {

        }

        protected virtual void OnUpdate(float dt)
        {

        }

        protected virtual void OnExit()
        {

        }

        /**
        * \brief This function is invoked from C++ whenever this entity has just entered collision
        *  with another entity. It converts the CollisionCPP struct into a Collision struct and
        *  then calls the OnCollisionEnter function to allow users to script behaviours on collision
        *  enter.
        *
        * \param collisionCPP CollisionCPP struct with collision info passed from C++.
        */
        public void OnCollisionEnterCPP(CollisionCPP collisionCPP)
        {
            Collision collision = new Collision();
            Entity otherEntity = new Entity(collisionCPP.otherEntity);
            collision.collider = otherEntity.GetComponent<AABBCollider2D>();
            collision.rigidbody = otherEntity.GetComponent<Rigidbody2D>();
            collision.impulse = collisionCPP.impulse;
            collision.relativeVelocity = collisionCPP.relativeVelocity;
            collision.contactPoint.point = collisionCPP.contactPoint.point;
            collision.contactPoint.normal = collisionCPP.contactPoint.normal;
            collision.contactPoint.penetration = collisionCPP.contactPoint.penetration;
            collision.contactPoint.thisCollider = GetComponent<AABBCollider2D>();
            collision.contactPoint.otherCollider = collision.collider;

            OnCollisionEnter(collision);
        }

        /**
        * \brief This function is invoked from C++ whenever this entity is colliding with
        *  another entity. It converts the CollisionCPP struct into a Collision struct and
        *  then calls the OnCollisionStay function to allow users to script behaviours on
        *  collision stay.
        *
        * \param collisionCPP CollisionCPP struct with collision info passed from C++.
        */
        public void OnCollisionStayCPP(CollisionCPP collisionCPP)
        {
            Collision collision = new Collision();
            Entity otherEntity = new Entity(collisionCPP.otherEntity);
            collision.collider = otherEntity.GetComponent<AABBCollider2D>();
            collision.rigidbody = otherEntity.GetComponent<Rigidbody2D>();
            collision.impulse = collisionCPP.impulse;
            collision.relativeVelocity = collisionCPP.relativeVelocity;
            collision.contactPoint.point = collisionCPP.contactPoint.point;
            collision.contactPoint.normal = collisionCPP.contactPoint.normal;
            collision.contactPoint.penetration = collisionCPP.contactPoint.penetration;
            collision.contactPoint.thisCollider = GetComponent<AABBCollider2D>();
            collision.contactPoint.otherCollider = collision.collider;

            OnCollisionStay(collision);
        }

        //public void OnCollisionStayCPP(CollisionCPP collisionCPP)
        //{
        //    //if (collisionCPP == null)
        //    //    return;

        //    try
        //    {
        //        Entity otherEntity = null;
        //        try
        //        {
        //            otherEntity = new Entity(collisionCPP.otherEntity);
        //        }
        //        catch (Exception e)
        //        {
        //            Console.WriteLine($"Failed to create Entity: {e.Message}");
        //            return;
        //        }

        //        if (otherEntity == null)
        //            return;

        //        if (this == null)
        //            return;

        //        var otherCollider = otherEntity.GetComponent<AABBCollider2D>();
        //        var otherRigidbody = otherEntity.GetComponent<Rigidbody2D>();
        //        var thisCollider = GetComponent<AABBCollider2D>();

        //        if (otherCollider == null || thisCollider == null)
        //            return;

        //        Collision collision = new Collision
        //        {
        //            collider = otherCollider,
        //            rigidbody = otherRigidbody,
        //            impulse = collisionCPP.impulse,
        //            relativeVelocity = collisionCPP.relativeVelocity,
        //            contactPoint = new ContactPoint
        //            {
        //                point = collisionCPP.contactPoint.point,
        //                normal = collisionCPP.contactPoint.normal,
        //                penetration = collisionCPP.contactPoint.penetration,
        //                thisCollider = thisCollider,
        //                otherCollider = otherCollider
        //            }
        //        };

        //        OnCollisionStay(collision);
        //    }
        //    catch (Exception e)
        //    {
        //        Console.WriteLine($"Collision processing error: {e.Message}");
        //    }
        //}

        /**
        * \brief This function is invoked from C++ whenever this entity stops colliding with
        *  another entity. It converts the CollisionCPP struct into a Collision struct and
        *  then calls the OnCollisionExit function to allow users to script behaviours on
        *  collision exit.
        *
        * \param collisionCPP CollisionCPP struct with collision info passed from C++.
        */
        public void OnCollisionExitCPP(CollisionCPP collisionCPP)
        {
            Collision collision = new Collision();
            Entity otherEntity = new Entity(collisionCPP.otherEntity);
            collision.collider = otherEntity.GetComponent<AABBCollider2D>();
            collision.rigidbody = otherEntity.GetComponent<Rigidbody2D>();
            collision.impulse = collisionCPP.impulse;
            collision.relativeVelocity = collisionCPP.relativeVelocity;
            collision.contactPoint.point = collisionCPP.contactPoint.point;
            collision.contactPoint.normal = collisionCPP.contactPoint.normal;
            collision.contactPoint.penetration = collisionCPP.contactPoint.penetration;
            collision.contactPoint.thisCollider = GetComponent<AABBCollider2D>();
            collision.contactPoint.otherCollider = collision.collider;

            OnCollisionExit(collision);
        }

        /**
        * \brief This function is invoked from C++ whenever this entity has just entered collision
        *  with a trigger collider. It converts the ColliderCPP struct into a Collider struct and
        *  then calls the OnTriggerEnter function to allow users to script behaviours on trigger
        *  enter.
        *
        * \param colliderCPP ColliderCPP struct with collider info passed from C++.
        */
        public void OnTriggerEnterCPP(ColliderCPP colliderCPP)
        {
            Entity otherEntity = new Entity(colliderCPP.otherEntity);
            AABBCollider2D collider = otherEntity.GetComponent<AABBCollider2D>();

            OnTriggerEnter(collider);
        }

        /**
        * \brief This function is invoked from C++ whenever this entity is colliding with
        *  a trigger collider. It converts the ColliderCPP struct into a Collider struct and
        *  then calls the OnTriggerStay function to allow users to script behaviours on trigger
        *  stay.
        *
        * \param colliderCPP ColliderCPP struct with collider info passed from C++.
        */
        public void OnTriggerStayCPP(ColliderCPP colliderCPP)
        {
            Entity otherEntity = new Entity(colliderCPP.otherEntity);
            //Console.WriteLine($"{otherEntity.ID}");
            AABBCollider2D collider = otherEntity.GetComponent<AABBCollider2D>();

            OnTriggerStay(collider);
        }
        //public void OnTriggerStayCPP(ColliderCPP colliderCPP)
        //{

        //    try
        //    {
        //        Entity otherEntity = null;
        //        try
        //        {
        //            // Safely create Entity, catching any potential creation errors
        //            otherEntity = new Entity(colliderCPP.otherEntity);
        //        }
        //        catch (Exception e)
        //        {
        //            Console.WriteLine($"Failed to create Entity from ColliderCPP: {e.Message}");
        //            return;
        //        }

        //        // Check if Entity creation failed
        //        if (otherEntity == null)
        //            return;

        //        // Safely retrieve the collider
        //        AABBCollider2D collider = otherEntity.GetComponent<AABBCollider2D>();

        //        // Only proceed if collider is not null
        //        if (collider != null)
        //        {
        //            OnTriggerStay(collider);
        //        }
        //        else
        //        {
        //            Console.WriteLine("No AABBCollider2D found on the entity");
        //        }
        //    }
        //    catch (Exception e)
        //    {
        //        // Catch any unexpected errors during processing
        //        Debug.WriteLine($"Error in OnTriggerStayCPP: {e.Message}");
        //    }
        //}

        /**
        * \brief This function is invoked from C++ whenever this entity stops colliding with
        *  a trigger collider. It converts the ColliderCPP struct into a Collider struct and
        *  then calls the OnTriggerExit function to allow users to script behaviours on trigger
        *  exit.
        *
        * \param colliderCPP ColliderCPP struct with collider info passed from C++.
        */
        public void OnTriggerExitCPP(ColliderCPP colliderCPP)
        {
            Entity otherEntity = new Entity(colliderCPP.otherEntity);
            AABBCollider2D collider = otherEntity.GetComponent<AABBCollider2D>();

            OnTriggerExit(collider);
        }

        /**
        * \brief A pure virtual function to be overridden by classes that inherit from
        *  Entity.cs to script behaviours when the entity has just collided with another
        *  entity.
        *
        * \param collision Information about the collision.
        */
        protected virtual void OnCollisionEnter(Collision collision)
        {

        }

        /**
        * \brief A pure virtual function to be overridden by classes that inherit from
        *  Entity.cs to script behaviours when the entity is colliding with another entity.
        *
        * \param collision Information about the collision.
        */
        protected virtual void OnCollisionStay(Collision collision)
        {

        }

        /**
        * \brief A pure virtual function to be overridden by classes that inherit from
        *  Entity.cs to script behaviours when the entity has stopped collided with another
        *  entity.
        *
        * \param collision Information about the collision.
        */
        protected virtual void OnCollisionExit(Collision collision)
        {

        }

        /**
        * \brief A pure virtual function to be overridden by classes that inherit from
        *  Entity.cs to script behaviours when the entity has just collided with a
        *  trigger collider.
        *
        * \param collider The other collider involved in the collision.
        */
        protected virtual void OnTriggerEnter(AABBCollider2D collider)
        {

        }

        /**
        * \brief A pure virtual function to be overridden by classes that inherit from
        *  Entity.cs to script behaviours when the entity is colliding with a trigger
        *  collider.
        *
        * \param collider The other collider involved in the collision.
        */
        protected virtual void OnTriggerStay(AABBCollider2D collider)
        {

        }

        /**
        * \brief A pure virtual function to be overridden by classes that inherit from
        *  Entity.cs to script behaviours when the entity stops colliding with a trigger
        *  collider.
        *
        * \param collider The other collider involved in the collision.
        */
        protected virtual void OnTriggerExit(AABBCollider2D collider)
        {

        }

        #region Added Functions
        protected float Lerp(float lhs, float rhs, float amount)
        {
            Clamp(amount);
            return lhs + (rhs - lhs) * amount;
        }
        protected Vec3 Lerp(Vec3 lhs, Vec3 rhs, float amount)
        {
            Clamp(amount);
            return new Vec3(Lerp(lhs.x, rhs.x, amount),
                            Lerp(lhs.y, rhs.y, amount),
                            Lerp(lhs.z, rhs.z, amount));
        }
        protected float Clamp(float value, float min = 0, float max = 1)
        {
            if (value < min) value = min; 
            if (value > max) value = max;
            return value;
        }
        #endregion
    }
}
