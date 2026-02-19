using System;
using ScriptCore;

namespace SandBox
{
    public class Checkpoint : Entity
    {
        //private Rigidbody2D body;
        private CheckpointManager manager;
        private Renderer renderer;
        private PlayerNew player;
        private bool active = false;

        protected override void OnInit()
        {
            renderer = GetComponent<Renderer>();
            ResetCheckpoint();
            //body = GetComponent<Rigidbody2D>();
        }
        public void GiveReference(CheckpointManager manager, PlayerNew player)
        {
            this.manager = manager;
            this.player = player;
        }
        public void ResetCheckpoint()
        {
            renderer?.SetTextureToEntity("194d9c68589-ea0951a1a3210f0a-a5d1cab48b802ec7");
            active = false;
        }
        public void ActivateCheckpoint()
        {
            renderer?.SetTextureToEntity("194d9dcd7c2-f1434ecd7eb42c6b-9a66f112e141a15a");
            active = true;
        }
        protected override void OnTriggerEnter(AABBCollider2D collider)
        {
            if (collider != null)
            {
                if (collider.Entity.ID == player?.ID)
                {
                    if (!active) manager?.ReachedCheckpoint(this);
                }
            }
        }
    }
}