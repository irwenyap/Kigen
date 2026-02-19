/*********************************************************************
 * \file	Player.cs
 * \brief	Script Behavior for Player in Physics Scene.
 *
 * \author	wengkhong.thang, 2301372
            woonli.wong, 2301308
 * \email	wengkhong.thang@digipen.edu
            woonli.wong@digipen.edu
 * \date	08 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

using System;
using System.IO;
using ScriptCore;

namespace SandBox
{
    public class PlayerOld : Entity
    {
        public float playerMoveForce = 50;
        public float playerJumpForce = 500;
        float playerMaxVelocity = 500;
        bool isGrounded = false;
        //ulong moveLeftFID, moveRightFID, jumpFID;
        private int charge = 0;
        private Rigidbody2D rb;
        private Renderer render;

        private Transform mTrans;

        Vec3 rotation = new Vec3(0, 0, 0);
        //Vec3 translation = new Vec3(500, 500, 0);

        public float Speed;
        public float Time = 0.0f;

        public UInt64 hi;

        string path = Directory.GetCurrentDirectory();

        protected override void OnInit()
        {
            Console.WriteLine($"Player Init! - {ID}");

            mTrans = GetComponent<Transform>();

            bool hasTransform = HasComponent<Transform>();

            Console.WriteLine($"{hasTransform}");
            rb = GetComponent<Rigidbody2D>();
            render = GetComponent<Renderer>();

            //Console.WriteLine($"RB entity id: - {rb.Entity.ID}");

            // Add the left, right and jump forces.
            //moveLeftFID = rb.AddForeverImpulseForce(new Vec2(-1, 0), playerMoveForce, false, (ulong)Physics.LinearForces.MOVELEFT);
            //Console.WriteLine($"moveLeftFID: - {moveLeftFID}");
            //moveRightFID = rb.AddForeverImpulseForce(new Vec2(1, 0), playerMoveForce, false, (ulong)Physics.LinearForces.MOVERIGHT);
            //Console.WriteLine($"moveRightFID: - {moveRightFID}");
            //jumpFID = rb.AddForeverImpulseForce(new Vec2(0, 1), playerJumpForce, false, (ulong)Physics.LinearForces.JUMP);
            //Console.WriteLine($"jumpFID: - {jumpFID}");
        }

        protected override void OnUpdate(float dt)
        {
            Time += dt;
            //Console.WriteLine("Player Updating!");

            //float speed = 500.0f;
            

            //Move box with A and D keys
            if (Input.IsKeyDown(KeyCode.A))
            {
                //Physics.ActivateForce(ID, Physics.LinearForces.MOVELEFT);
                //rb.ActivateForce(moveLeftFID, true);
                rb.AddImpulseForce(new Vec2(-1, 0), playerMoveForce);
                //rb.Velocity = new Vec2(-playerMoveForce, rb.Velocity.y);
                //Console.WriteLine("Hitting A!");
            }
            if (Input.IsKeyDown(KeyCode.D))
            {
                //Physics.ActivateForce(ID, Physics.LinearForces.MOVERIGHT);
                //rb.ActivateForce(moveRightFID, true);
                rb.AddImpulseForce(new Vec2(1, 0), playerMoveForce);
                //rb.Velocity = new Vec2(-playerMoveForce, rb.Velocity.y);
                //Audio.PlaySound("../Assets/Audio/CraneStopSFX.wav", 1.0f);//still jank
            }

            if (Input.IsKeyPressed(KeyCode.Space) && isGrounded)
            {
                //Physics.ActivateForce(ID, Physics.LinearForces.JUMP);
                //Physics.SetRBGrounded(ID, false);
                //rb.ActivateForce(jumpFID, true);
                rb.AddImpulseForce(new Vec2(0, 1), playerJumpForce);
                //rb.AddForceOverTime(new Vec2(0, 1), playerJumpForce, 0.2f);
                isGrounded = false;
                //Console.WriteLine("Is jumping set to true");
            }
            
            


            if (Input.IsKeyPressed(KeyCode.O))
            {
                rotation.z += 90;
                render.SetVisibility(true);
                //translation = new Vec3(500.0f, 500.0f, 0.0f);
            }
            //mTrans.Translation = translation;
            mTrans.Rotation = rotation;
            if (rb.Velocity.x >= playerMaxVelocity)
            {
                rb.Velocity = new Vec2(playerMaxVelocity, rb.Velocity.y);
            }
            else if (rb.Velocity.x <= -playerMaxVelocity)
            {
                rb.Velocity = new Vec2(-playerMaxVelocity, rb.Velocity.y);
            }

            Entity cameraEntity = FindEntityByName("Camera");
            if (cameraEntity != null)
            {
                Console.WriteLine("have cam!");
                //Camera camera = cameraEntity.As<Camera>();

                //if (Input.IsKeyDown(KeyCode.Q))
                //    camera.zoom += Speed;
                //else if (Input.IsKeyDown(KeyCode.E))
                //    camera.zoom -= Speed;
            }

            
        }


        protected override void OnCollisionEnter(Collision collision)
        {
            //Console.WriteLine($"On collision enter contact point y: {collision.contactPoint.point.y}");
            if (collision.contactPoint.point.y < rb.Position.y)
            {
                //Console.WriteLine("Is jumping set to false");
                isGrounded = true;
            }
        }

        protected override void OnCollisionStay(Collision collision)
        {
            //Console.WriteLine($"Charge: {++charge}");
        }

        protected override void OnExit()
        {

        }

        protected override void OnCollisionExit(Collision collision)
        {
            //Console.WriteLine("On collision exit");
        }

        protected override void OnTriggerEnter(AABBCollider2D collider)
        {
            Console.WriteLine("Player On trigger enter:");
        }

        protected override void OnTriggerStay(AABBCollider2D collider)
        {
            //Console.WriteLine($"Charge: {++charge}");
        }

        protected override void OnTriggerExit(AABBCollider2D collider)
        {
           //Console.WriteLine($"On trigger stay: Other collider sizeY: {collider.SizeY}");
        }
    }
}
