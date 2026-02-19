using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class ParallaxLayer : Entity
    {
        private Transform layerPosition;
        public float parallaxFactor;

        public int parallaxLayerNumber;

        private Transform cameraPos;
        private float oldPos; 

        protected override void OnInit()
        {
            layerPosition = GetComponent<Transform>();
            cameraPos = FindEntityByName("Main Camera")?.GetComponent<Transform>();
            oldPos = cameraPos.Translation.x;
        }

        protected override void OnUpdate(float dt)
        {
            float nextPos = cameraPos.Translation.x - oldPos;
            layerPosition.Translation = Vec3.AddToX(layerPosition.Translation, nextPos * parallaxFactor);
            oldPos = cameraPos.Translation.x;
        }
    }
}