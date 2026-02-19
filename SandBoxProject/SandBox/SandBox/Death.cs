using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ScriptCore;

namespace SandBox
{
    public class Death : Entity
    {
        //Reference to Player
        Entity tmpPlayer;
        PlayerNew player;

        protected override void OnInit()
        {
            //Player
            tmpPlayer = FindEntityByName("Player");
            player = tmpPlayer?.As<PlayerNew>();
        }

        protected override void OnTriggerEnter(AABBCollider2D collider)
        {
            if(collider.Entity.ID == player.ID)
            {
                player.PlayerDeath();
            }
        }
        protected override void OnTriggerStay(AABBCollider2D collider)
        {
            if (collider.Entity.ID == player.ID)
            {
                player.PlayerDeath();
            }
        }
        protected override void OnTriggerExit(AABBCollider2D collider)
        {
            if (collider.Entity.ID == player.ID)
            {
                player.PlayerDeath();
            }
        }
    }
}