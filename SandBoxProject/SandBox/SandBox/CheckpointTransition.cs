using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class CheckpointTransition : Entity
    {
        private Checkpoint checkpoint;
        public float transitionDuration;
        private float timer;
        private Animation animation;
        private bool transitioning = false;
        private Transform transform;

        protected override void OnInit()
        {
            animation = GetComponent<Animation>();
            transform = GetComponent<Transform>();
            IsActive = false;
        }

        protected override void OnUpdate(float dt)
        {
            if (!transitioning) return;

            if (timer >= transitionDuration)
            {
                checkpoint.ActivateCheckpoint();
                IsActive = false;
                transitioning = false;
            }
            else timer += dt;
        }
        public void StartTransition()
        {   
            Audio.PlaySpatial(this.ID,"../Assets/Audio/Environment SFX/CHECKPOINT_ACTIVATION.wav", transform.Translation, 1f);
            IsActive = true;
            animation.PlayAnimation(true);
            timer = 0f;
            transitioning = true;
        }

        public void GiveReference(Checkpoint checkpoint)
        {
            this.checkpoint = checkpoint;
        }

    }
}
