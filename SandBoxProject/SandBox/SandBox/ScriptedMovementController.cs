using System;
using System.Collections.Generic;
using System.Data;
using System.Diagnostics;
using System.Diagnostics.SymbolStore;
using System.IO;
using ScriptCore;

namespace SandBox
{
    public class ScriptedMovementController : Entity
    {
        public float zoomDuration = 1f;
        private PlayerNew player;
        private CameraScript camera;

        private Entity outletBase;
        private Entity outletBack;

        private Vec2 targetDestination = new Vec2(17162, -4126);
        private int currentTargetIndex = 0;
        private float tolerance = 10f;
        private bool cutsceneActive = false;

        private bool runTimer = false;
        private float cutsceneTimer = 0f;

        private bool cutsceneDelayActive = false;
        private float cutsceneDelayTimer = 0f;

        private DialogueManager dialogueManager;

        protected override void OnInit()
        {
            player = FindEntityByName("Player")?.As<PlayerNew>();
            camera = FindEntityByName("Main Camera")?.As<CameraScript>(); 
            dialogueManager = FindEntityByName("Dialogue Manager")?.As<DialogueManager>();

            outletBase = FindEntityByName("Outlet Base")?.As<Asda>();
            outletBack = FindEntityByName("Outlet Backing")?.As<Asda>();
        }

        protected override void OnUpdate(float dt)
        {
            if (!cutsceneActive || player == null) return;


            if(cutsceneDelayActive)
            {
                if (cutsceneDelayTimer >= 4f)
                {
                    cutsceneDelayActive = false;
                }
                else
                {
                    cutsceneDelayTimer += dt;
                    return;
                }
            }


            if (runTimer)
            {
                Logger.Log("Timer Running [Cutscene]", LogLevel.DEBUG);
                cutsceneTimer += dt;
            }

            Vec2 currentPos = new Vec2(player.transform.Translation.x, player.transform.Translation.y);

            //Calculate direction 
            Vec2 direction = new Vec2(targetDestination.x - currentPos.x, targetDestination.y - currentPos.y);

            //If the player is close enough than move on to the next waypoint
            if(Magnitude(direction) < tolerance)
            {
                runTimer = true;

                //Snap player to target destination
                player.transform.Translation = new Vec3(targetDestination.x, targetDestination.y, player.transform.Translation.z);

                //Reset scripted movement input so that theres no direction
                player.SetScriptedMovement(new Vec2(0,0));

                //Force player animation back to Idle
                player.ChangeState(0);

                //Scripted Jump then back to Idle
                if (cutsceneTimer >= 2f)
                {
                    Logger.Log("Enter Cutscene Timer", LogLevel.DEBUG);
                    player.ScriptedJump();
                    outletBase.IsActive = true;
                    outletBack.IsActive = true;
                    player.ChangeState(0);
                    ResetTimer();
                    //player.isScriptedMode = false;
                    //player.ResetScriptedMoveAudio();
                    cutsceneActive = false;
                    return;
                }
                else return;
            }

            //Normalise the Vec2 and feed into player scripted movement
            direction = direction.Normalized();
            player.SetScriptedMovement(direction);

        }

        private float Magnitude(Vec2 direction)
        {
            return (float)Math.Sqrt(direction.x * direction.x + direction.y * direction.y);
        }

        public void StartCutscene()
        {
            //Reset state
            cutsceneActive = true;
            cutsceneDelayActive = true;
            player.renderer?.SetTextureToEntity("19602ca5d68-b20b7589ab3a08e1-c2ef1e625d69d142");
            player.ChangeState(0);

            //Make the player ignore real input
            if (player != null)
            {
                player.isScriptedMode = true;
                camera?.ResetZoom(zoomDuration);
                camera?.ResetOffset();
                //Audio.PlaySound(this.ID, "../Assets/Audio/Voiceovers/Dialogue84-87.wav", 0.6f);
                dialogueManager.PlayDialogue(29, 0.6f, false);
            }
        }

        private void ResetTimer()
        {
            cutsceneTimer = 0;
            runTimer = false;
        }
    }
}

