using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class FakeParticleSystem : Entity
    {
        public int fXAmount;
        public AABBCollider2D collider;
        public float frequency;
        public float particleLifetime;
        public bool playOnce;

        private float timer;
        private Random random;
        private List<Particle> particles = new List<Particle>();
        protected override void OnInit()
        {
            collider = GetComponent<AABBCollider2D>();
            random = new Random();
            for(int i = 1; i <= fXAmount; i++)
            {
                Entity entity = FindEntityByName($"SparkVFX{1}");
                Particle particle = entity?.As<Particle>();
                if (particle != null)
                {
                    particle.InitializeParticle(particleLifetime, playOnce);
                    particles.Add(particle);
                }
            }
        }
        protected override void OnUpdate(float dt)
        {
            if (timer >= frequency)
            {
                SpawnVFX();
                timer = 0f;
            }
            else
            {
                timer += dt;
            }
        }
        private void SpawnVFX()
        {
            
        }
        private Vec2 GetRandomLocation()
        {
            double x = random.NextDouble() * (collider.Max.x - collider.Min.x) + collider.Min.x;
            double y = random.NextDouble() * (collider.Max.y - collider.Min.y) + collider.Min.y;
            return new Vec2((float)x, (float)y);
        }
    }
}
