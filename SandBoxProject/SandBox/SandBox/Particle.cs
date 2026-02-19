using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class Particle : Entity
    {
        private float lifetime;
        private bool playOnce;
        private bool paused;
        private float timer;


        protected override void OnUpdate(float dt)
        {
            if (paused) return; 

            if (timer >= lifetime)
            {
                IsActive = false;

                if (playOnce) Complete();
            }
            else timer += dt;
        }
        public void InitializeParticle(float lifetime, bool playOnce)
        {
            this.lifetime = lifetime;
            this.playOnce = playOnce;
        }
        public void Spawn()
        {
            timer = 0f;
            IsActive = true;
        }
        public void Complete()
        {
            paused = true;
        }
    }
}
