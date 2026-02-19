using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ScriptCore;

namespace SandBox
{
    public class UnchargedBlock : Block
    {
        //Reference to texture
        public Renderer renderer;

        //Reference to collider
        public AABBCollider2D colliderUCB;

        public bool flipCB = false;

        //Reference to Player
        protected Entity tmpPlayer;
        protected PlayerNew player;

        protected ChargedBlock controlBlock;

        protected override void OnInit()
        {
            renderer = GetComponent<Renderer>();
            colliderUCB = GetComponent<AABBCollider2D>();

            //Player
            tmpPlayer = FindEntityByName("Player");
            player = tmpPlayer?.As<PlayerNew>();

            if (flipCB) renderer?.SetTextureToEntity("1955615a49f-6d0cbcba4263e001-bbf6bb2a39aeec81");

            Activate(false);
        }

        protected override void OnUpdate(float dt)
        {

        }
        protected override void OnCollisionEnter(Collision collision)
        {
            if (collision.collider != null)
            {
                if (collision.collider.Entity.ID == player.ID)
                {
                    controlBlock?.PlayerOnBlock(this, true);
                }
            }
        }
        protected override void OnCollisionExit(Collision collision)
        {
            if (collision.collider != null)
            {
                if (collision.collider.Entity.ID == player.ID)
                {
                    controlBlock?.PlayerOnBlock(this, false);
                }
            }
        }
        public void SetConnections(ChargedBlock controlBlock)
        {
            if (controlBlock != null)
            {
                this.controlBlock = controlBlock;
            }
        }
        public override void Activate(bool state)
        {
            if (state)
            {
                if (flipCB) renderer?.SetTextureToEntity("1955615a463-d38b67c4b0d5b83b-362e96d522819937");
                else renderer?.SetTextureToEntity("19557868ee9-c1b15bf5820aee02-6b8c159a6690a74a");
                if (colliderUCB != null) colliderUCB.IsTrigger = false;
            }
            else
            {
                if (flipCB) renderer?.SetTextureToEntity("1955615a49f-6d0cbcba4263e001-bbf6bb2a39aeec81");
                else renderer?.SetTextureToEntity("1955615a4a4-822496f6e4728a79-84939a18f8528648");
                if (colliderUCB != null) colliderUCB.IsTrigger = true;
            }
        }
    }
}