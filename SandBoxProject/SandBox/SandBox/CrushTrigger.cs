using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class CrushTrigger : Entity
    {
        private Entity playerEntity;
        private PlayerNew player;
        protected override void OnInit()
        {
            playerEntity = FindEntityByName("Player");
            player = playerEntity?.As<PlayerNew>();
        }
        protected override void OnTriggerEnter(AABBCollider2D collider)
        {
            if (collider != null)
            {
                if (collider.Entity.ID == playerEntity.ID)
                {
                    player.platformCrushing = true;
                }
            }
        }
        protected override void OnTriggerExit(AABBCollider2D collider)
        {
            if (collider != null)
            {
                if (collider.Entity.ID == playerEntity.ID)
                {
                    player.platformCrushing = false;
                }
            }
        }
    }
}
