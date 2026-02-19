using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class FinalDoorHandPrint : Entity
    {
        public float frequency;
        public int blinks; 

        public string whiteId = "1958e60d8c3-cbc5a3e6b38b2413-f07e8e5b2a34a57a";
        public string redId = "1958e69ff10-fb0c8b091cdea63b-d2715f41bb62fec7";

        private Renderer renderer; 
        private int blinkCounter; 
        private float timer = 0f;
        private bool startBlinking = false;
        private DialogueManager dialogueManager;
        protected override void OnInit()
        {
            renderer = GetComponent<Renderer>();
            dialogueManager = FindEntityByName("Dialogue Manager")?.As<DialogueManager>();
            IsActive = true;
        }
        protected override void OnUpdate(float dt)
        {
            if(startBlinking)
            {
                Logger.Log(dt.ToString(), LogLevel.INFO);
                timer += dt * frequency; 

                if(timer >= 1f)
                {
                    blinkCounter++; 
                    string id = (blinkCounter % 2 == 0) ? redId : whiteId;
                    renderer?.SetTextureToEntity(id);
                    timer -= 1f;

                    if (blinkCounter >= blinks) startBlinking = false;
                }
            }
        }
        public void StartBlink()
        {
            timer = 0f;
            startBlinking = true;
            blinkCounter = 0;
            Audio.PlaySound(this.ID, "../Assets/Audio/UI SFX/DOOR LOCKED.wav", 1f);
        }
    }
}
