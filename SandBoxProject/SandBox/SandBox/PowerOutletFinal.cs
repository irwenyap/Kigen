using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class PowerOutletFinal : Entity
    {
        private Animation anim;
        private AniData tmpAnim;

        public bool startTimer = false;
        private float timer = 0f;
        public bool animRan = false;

        protected override void OnInit()
        {
            base.OnInit();

            //Animation Component
            anim = GetComponent<Animation>();
        }

        protected override void OnUpdate(float dt)
        {
            tmpAnim = anim.data;

            if(startTimer)
            {
                if (timer >= 2f && !animRan)
                {
                    PlayAnimation();
                    animRan = true;
                }
                else timer += dt;
            }
        }

        public void PlayAnimation()
        {
            if(tmpAnim.currentFrame != 0) tmpAnim.currentFrame = 0;
            tmpAnim.startFrame = 0;
            tmpAnim.endFrame = 125;
            tmpAnim.playOnce = true;
            tmpAnim.isLooping = false;
            anim.data = tmpAnim;

            Audio.PlaySound(this.ID,"../Assets/Audio/Environment SFX/OUTLET_AUDIO.wav", 0.3f);
        }
    }
}
