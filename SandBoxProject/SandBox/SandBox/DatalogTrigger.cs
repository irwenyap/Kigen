using ScriptCore;
using System;

namespace SandBox
{
    public class DatalogTrigger : Entity
    {
        Entity tmpPlayer;
        PlayerNew player;

        private DatalogManager datalogManager;
        private bool canInteract, interacted = false;
        private bool isLooping = false;
        private Transform transform;

        private string loopAudioPath = "../Assets/Audio/Environment SFX/DataLog Area Loop.wav";

        protected override void OnInit()
        {
            datalogManager = FindEntityByName("Datalog Manager")?.As<DatalogManager>();

            tmpPlayer = FindEntityByName("Player");
            player = tmpPlayer?.As<PlayerNew>();
            transform = GetComponent<Transform>();
            IsActive = false;
            LoopAudio();
        }

        protected override void OnUpdate(float dt)
        {
            AdjustLoopAudioVolume(); // Still update every frame

            if (interacted) return;

            if (canInteract && (Input.IsKeyPressed(KeyCode.E) || Input.IsGamepadButtonPressed(ButtonCode.GamepadButtonX))) // Gamepad X button to interact
            {
                datalogManager?.CompleteDatalog();
                interacted = true;
                IsActive = false;
            }
        }

        private void AdjustLoopAudioVolume()
        {

            Vec3 playerPos = player.transform.Translation;
            Vec3 datalogPos = transform.Translation;
            float distance = Vec3.Distance(playerPos, datalogPos);

            float fadeStart = 300f;
            float fadeEnd = 2000f;
            float maxVol = 0.4f;
            float volume;

            if (distance <= fadeStart)
                volume = maxVol;
            else if (distance >= fadeEnd)
                volume = 0f;
            else
                volume = maxVol * (1f - (distance - fadeStart) / (fadeEnd - fadeStart));

            // Clamp volume
            volume = Math.Max(0f, Math.Min(maxVol, volume));

            float relativeX = playerPos.x - datalogPos.x;
            float panningRange = 600f; // narrower range = harder pan
            float rawPan = -relativeX / panningRange;
            float panning = Math.Max(-1f, Math.Min(1f, rawPan));


            if (Math.Abs(panning) < 0.05f)
                panning = 0.0f;

            panning = Clamp(panning, -1f, 1f); // Use your own Clamp method


            // Apply audio settings
            Audio.SetPanning(this.ID, loopAudioPath, panning);
            Audio.UpdateSound(this.ID, loopAudioPath, volume);

            // Estimate stereo balance
            float left = (1f - panning) * volume * 0.5f;
            float right = (1f + panning) * volume * 0.5f;
        }

        private void LoopAudio()
        {
            string uniqueClipID = loopAudioPath + "_" + this.ID;

            if (!Audio.ClipIsPlaying(uniqueClipID))
            {
                //Console.WriteLine($"[DEBUG] Will play sound: {uniqueClipID}");
                Audio.PlaySoundWithPanning(this.ID, loopAudioPath, 1.0f, true, true);
                isLooping = true;
            }
            else
            {
               // Console.WriteLine($"[DEBUG] Sound already playing for: {uniqueClipID}");
            }
        }



        protected override void OnTriggerEnter(AABBCollider2D collider)
        {
            if (collider != null && collider.Entity.ID == player?.ID)
            {
                if (!interacted) IsActive = true;
                canInteract = true;
            }
        }

        protected override void OnTriggerExit(AABBCollider2D collider)
        {
            if (collider != null && collider.Entity.ID == player?.ID)
            {
                IsActive = false;
                canInteract = false;
            }
        }
    }
}