/*********************************************************************
 * \file	Balls.cs
 * \brief	Script Behavior for Balls in Physics Scene
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	08 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

using System;
using System.Runtime.InteropServices;
using ScriptCore;

namespace SandBox
{
    public class Ball : Entity
    {
        Rigidbody2D rb;

        Animation animation;

        //float ballMinForce = -25000;
        //float ballMaxForce = 25000;
        //Vec2 ballMinPos = new Vec2(50, 500);
        //Vec2 ballMaxPos = new Vec2(1880, 1030);

        Animation tmp = new Animation();

        protected override void OnInit()
        {
            //Console.WriteLine($"Ball Init! - {ID}");
            //rb = GetComponent<Rigidbody2DComponent>();
            Console.WriteLine($"Ball Init! - {ID}");
            rb = GetComponent<Rigidbody2D>();

            //// Spawn at random position.
            //float randPosX = ballMinPos.x + (float)new Random().NextDouble() * (ballMaxPos.x - ballMinPos.x);
            //float randPosY = ballMinPos.y + (float)new Random().NextDouble() * (ballMaxPos.y - ballMinPos.y);
            //rb.Position = new Vec2(randPosX, randPosY);

            //// Add random force.
            //float randForceX = ballMinForce + (float)new Random().NextDouble() * (ballMaxForce - ballMinForce);
            //float randForceY = ballMinForce + (float)new Random().NextDouble() * (ballMaxForce - ballMinForce);
            //rb.AddImpulseForce(new Vec2(randForceX, randForceY), 1);

            animation = GetComponent<Animation>();
            
        }

        protected override void OnUpdate(float dt)
        {
            if (Input.IsKeyPressed(KeyCode.K))
            {
                animation.SetAnimation(tmp);
                animation.PlayAnimation();

            }

            
        }

        protected override void OnExit()
        {

        }


        void OnCollisionEnter()
        {
            //Console.WriteLine("On collision enter");
        }
    }
}
