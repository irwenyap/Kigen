using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ScriptCore;

namespace SandBox
{
    public class DialogueTrigger : Entity
    {
        Entity dialogueUI;
        private Renderer dialogueRenderer;

        //Reference to Player
        Entity tmpPlayer;
        PlayerNew player;

        private float timer = 0;
        private bool played = false;

        public int dialogueID;
        private DialogueManager dialogueManager;

        private float Clamp(float value, float min, float max)
        {
            return (value < min) ? min : (value > max) ? max : value;
        }

        protected override void OnInit()
        {
            //dialogueUI = FindEntityByName("DialogueUI");

            //Player
            tmpPlayer = FindEntityByName("Player");
            player = tmpPlayer?.As<PlayerNew>();

            //if (dialogueUI != null )
            //{
            //    dialogueRenderer = dialogueUI?.GetComponent<Renderer>();
            //}

            //dialogueUI.IsActive = false;
            dialogueManager = FindEntityByName("Dialogue Manager")?.As<DialogueManager>();
        }

        protected override void OnUpdate(float dt)
        {
            //if (timer < 27)
            //{
            //    timer += dt;
            //}

            //if (timer >= 27)
            //{
            //    dialogueUI.IsActive = false;
            //}
            //if (!played || player == null) return; // Ensure player exists

            //// Get dialogue position
            //Transform dialogueTransform = GetComponent<Transform>();
            //Vec3 dialoguePosition = dialogueTransform.Translation;

            //// Get player position
            //Vec3 playerPosition = player.transform.Translation;

            //// Calculate distance between player and dialogue source
            //float distance = Vec3.Distance(playerPosition, dialoguePosition);

            //float volume;

            //float maxVolume = 1.0f;
            //float minVolume = 0f;
            //float fadeStartDistance = 100.0f;   
            //float fadeEndDistance = 750.0f;   

            //// If within `fadeStartDistance`, use max volume
            //if (distance <= fadeStartDistance)
            //{
            //    volume = maxVolume;
            //}
            //else if (distance >= fadeEndDistance)
            //{
            //    volume = minVolume;
            //}
            //else
            //{
            //    // Gradually fade between fadeStartDistance and fadeEndDistance
            //    float fadeFactor = (distance - fadeStartDistance) / (fadeEndDistance - fadeStartDistance);
            //    volume = maxVolume * (1.0f - fadeFactor);
            //    volume = Math.Max(minVolume, volume); // Ensure volume never goes below `0.1f`
            //}

            //Console.WriteLine($"Calculated Volume: {volume} at Distance: {distance}");

            //// Update volume instead of restarting sound
            //Audio.UpdateSound("../Assets/Audio/Voiceovers/Dialogue1.wav", volume);
        }

        protected override void OnTriggerEnter(AABBCollider2D collider)
        {
            if (collider != null)
            {
                if (collider.Entity.ID == player?.ID)
                {
                    //Transform dialogueTrans = GetComponent<Transform>();
                    //Vec3 dialogPos = new Vec3();
                    //if (dialogueTrans != null) dialogPos = dialogueTrans.Translation;

                    //dialogueUI.IsActive = true;
                    //Audio.PlaySpatial("../Assets/Audio/Voiceovers/Dialogue1.wav", dialogPos, 2f);
                    //dialogueUI.IsActive = true;
                    ////Audio.PlaySound("../Assets/Audio/Voiceovers/Dialogue1.wav", 2f);
                    //played = true;
                    if (!played)
                    {
                        dialogueManager?.PlayDialogue(dialogueID);
                        played = true;
                    }
                }
            }
        }
    }
}
