using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class FadeDeath : Entity
    {
        private Animation anim;
        private AniData tmpAnim;

        protected override void OnInit()
        {
            anim = GetComponent<Animation>();
        }

        protected override void OnUpdate(float dt)
        {
            tmpAnim = anim.data;

            if (FinishAnim()) IsActive = false;
        }

        public void PlayAnimation()
        {
            IsActive = true;
            if (tmpAnim.currentFrame != 0) tmpAnim.currentFrame = 0;
            tmpAnim.startFrame = 0;
            tmpAnim.endFrame = 34;
            tmpAnim.playOnce = true;
            tmpAnim.isLooping = false;
            anim.data = tmpAnim;
            anim.PlayAnimation();
        }
        
        public bool FinishAnim()
        {
            if (tmpAnim.currentFrame == 34) return true;
            else return false;
        }
    }
}
