using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class ExplosionVFX : Entity
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
        }

        public void PlayAnimation()
        {
            if (tmpAnim.currentFrame != 0) tmpAnim.currentFrame = 0;
            tmpAnim.startFrame = 0;
            tmpAnim.endFrame = 34;
            tmpAnim.playOnce = true;
            tmpAnim.isLooping = false;
            anim.data = tmpAnim;
        }
    }
}
