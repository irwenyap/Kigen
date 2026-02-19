using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class Cable : Entity
    {
        private Animation cableAnim;
        private AniData tmpAnim;

        protected override void OnInit()
        {
            cableAnim = GetComponent<Animation>();
        }

        protected override void OnUpdate(float dt)
        {
            tmpAnim = cableAnim.data;
        }

        public void PlayCableAnimation(int cableNumber)
        {
            if (cableNumber == 1)
            {
                if (tmpAnim.currentFrame != 0) tmpAnim.currentFrame = 0;
                tmpAnim.startFrame = 0;
                tmpAnim.endFrame = 20;
                tmpAnim.playOnce = true;
                tmpAnim.isLooping = false;
                cableAnim.data = tmpAnim;
            }
            else if (cableNumber == 2)
            {
                if (tmpAnim.currentFrame != 0) tmpAnim.currentFrame = 0;
                tmpAnim.startFrame = 0; 
                tmpAnim.endFrame = 18;
                tmpAnim.playOnce = true;
                tmpAnim.isLooping = false;
                cableAnim.data = tmpAnim;
            }
            else if (cableNumber == 3)
            {
                if (tmpAnim.currentFrame != 0) tmpAnim.currentFrame = 0;
                tmpAnim.startFrame = 0;
                tmpAnim.endFrame = 20;
                tmpAnim.playOnce = true;
                tmpAnim.isLooping = false;
                cableAnim.data = tmpAnim;
            }
            else
            {
                return;
            }

        }
    }
}
