using System;
using ScriptCore;

namespace SandBox
{
    public class Ground : Entity
    {
        Entity player;

        private bool onGround;
        private bool wasGrounded = true;
        private bool isJumping = false;

        protected override void OnInit()
        {
            player = FindEntityByName("Player");
            if (player != null)
            {
                //Console.WriteLine(player.ID);
            }
        }

        protected override void OnTriggerEnter(AABBCollider2D collider)
        {
            if (collider != null)
            {
                //Console.WriteLine(collider.Entity.ID);
                if (collider.Entity.ID == player.ID)
                {
                    Console.WriteLine("On trigger enter with player");
                    //player.isGrounded = true;
                }
            }
        }
    }
}