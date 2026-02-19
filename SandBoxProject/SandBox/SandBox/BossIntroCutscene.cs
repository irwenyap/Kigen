using System;
using System.Collections.Generic;
using System.Data;
using System.Diagnostics;
using System.Diagnostics.SymbolStore;
using System.IO;
using ScriptCore;

namespace SandBox
{
    public class BossIntroCutscene : Entity
    {
        private Animation introAnim;
        private AniData tmpAnim;

        public bool introComplete = false;

        protected override void OnInit()
        {
            introAnim = GetComponent<Animation>();
            tmpAnim.currentFrame = 0;
        }

        protected override void OnUpdate(float dt)
        {
            tmpAnim = introAnim.data;
            if (tmpAnim.currentFrame == 47 && !introComplete) introComplete = true;
        }

        public void PlayBossIntroAnimation()
        {
            if (tmpAnim.currentFrame != 0) tmpAnim.currentFrame = 0;
            tmpAnim.startFrame = 0;
            tmpAnim.endFrame = 47;
            tmpAnim.playOnce = true;
            tmpAnim.isLooping = false;
            introAnim.data = tmpAnim;
        }
    }
}
