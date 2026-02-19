using ScriptCore;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class Credits : Entity
    {
        private Transform transform;
        private Transform transform2;

        public Entity credits2;

        public float startPos;
        public float endPos;

        public float moveSpeed;

        private bool creditEnd = false;
        private float delayTimer = 0f;

        public bool activate = false;

        protected override void OnInit()
        {
            base.OnInit();
            transform = GetComponent<Transform>();

            credits2 = FindEntityByName("Credits 2");
            transform2 = credits2.GetComponent<Transform>();

            this.IsActive = false;
            credits2.IsActive = false;
        }

        protected override void OnUpdate(float dt)
        {

            if (!activate) return;

            if(transform2.Translation.y >= endPos && !creditEnd)
            {
                if (delayTimer >= 3f)
                {
                    creditEnd = true;
                    //Change Scene to Main Menu
                    Input.ChangeScenePath("../Assets/Scenes/Main Menu.scene");
                }
                else
                {
                    delayTimer += dt;
                    return;
                }
            }

            float nextPos = moveSpeed * dt;
            transform.Translation = new Vec3(transform.Translation.x , transform.Translation.y + nextPos, transform.Translation.z);
            transform2.Translation = new Vec3(transform2.Translation.x, transform2.Translation.y + nextPos, transform2.Translation.z);
        }


    }
}
