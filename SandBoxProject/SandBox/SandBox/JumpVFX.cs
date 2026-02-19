using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class JumpVFX : Entity
    {
        public float xOffset, yOffset;

        private Rigidbody2D rigidbody;
        private Animation animation;

        private float timer;

        protected override void OnInit()
        {
            rigidbody = GetComponent<Rigidbody2D>();
            animation = GetComponent<Animation>();
        }

        protected override void OnUpdate(float dt)
        {
            if (timer > 0.27f)
            {
                IsActive = false;
            }
            else timer += dt;
        }
        public void SpawnVFX(Transform location)
        {
            timer = 0f;
            rigidbody.Position = new Vec2(location.Translation.x + xOffset, location.Translation.y + yOffset);
            IsActive = true;
            animation.PlayAnimation(true);
        }
    }
}
