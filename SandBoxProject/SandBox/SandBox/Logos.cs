using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ScriptCore;

namespace SandBox
{
    public class Logos : Entity
    {
        private float timer = 0f;
        private float loadSceneDelay = 6f;
        private float switchSceneDelay = 8f;

        private Entity playerEntt;
        private VideoPlayer player;
        private bool startPlaying = false;


        protected override void OnInit()
        {
            playerEntt = FindEntityByName("Logos");
            player = playerEntt?.GetComponent<VideoPlayer>();
        }
        protected override void OnUpdate(float dt)
        {
            timer += dt;
            if (timer >= loadSceneDelay)
            {
                if (!startPlaying)
                {
                    player.IsPlaying = true;
                    startPlaying = true;
                }
                if (!player.IsPlaying || Input.GetAnyKeyDown())
                {
                    Input.ChangeScenePath("../Assets/Scenes/Main Menu.scene");
                }
            
            }
        }
    }
}
