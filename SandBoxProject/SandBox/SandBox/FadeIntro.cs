using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class FadeIntro : Entity
    {
        private Animation anim;
        private AniData tmpAnim;

        private float delayTimer;
        private bool startTimer;

        protected override void OnInit()
        {
            anim = GetComponent<Animation>();
            this.IsActive = true;
            startTimer = true;
        }

        protected override void OnUpdate(float dt)
        {
            tmpAnim = anim.data;

            if (!this.IsActive) return;

            if(startTimer)
            {
                if (delayTimer >= 0.5f)
                {
                    
                    if (tmpAnim.currentFrame != 0) tmpAnim.currentFrame = 0;
                    tmpAnim.startFrame = 0;
                    tmpAnim.endFrame = 34;
                    tmpAnim.playOnce = true;
                    tmpAnim.isLooping = false;
                    anim.data = tmpAnim;

                    Audio.PlaySound(this.ID,"../Assets/Audio/UI SFX/SCENE_TRANSITION_IN.wav", 0.7f);

                    startTimer = false;
                }
                else
                {
                    delayTimer += dt;
                    return;
                }
            }

            //this.IsActive = false;
        }
    }
}
