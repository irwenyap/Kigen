using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class Datalog
    {
        public Entity entity;
        public Animation animation;
        public Entity reverse;
        public Animation animationReverse;
        public Entity freeze;

        public Datalog(Entity entity, Animation animation, Entity reverse, Animation animationReverse, Entity freeze)
        {
            this.entity = entity;
            this.animation = animation;
            this.reverse = reverse;
            this.animationReverse = animationReverse;
            this.freeze = freeze;
        }
    }
}
