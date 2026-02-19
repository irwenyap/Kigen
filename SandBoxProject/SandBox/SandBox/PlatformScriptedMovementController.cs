using System;
using System.Collections.Generic;
using System.Data;
using System.Diagnostics;
using System.Diagnostics.SymbolStore;
using System.IO;
using ScriptCore;

namespace SandBox
{
    public class PlatformScriptedMovementController : Entity
    {
        private PlayerNew player;
        private CameraScript camera;

        private Vec2 targetDestination = new Vec2(17162, -4126);
        private int currentTargetIndex = 0;
        private float tolerance = 1f;
        private bool cutsceneActive = false;

        //Platform Cutscene
        private PlatformBlockBoss platformBoss;
        private float platformDelayTimer = 0f;

        protected override void OnInit()
        {
            player = FindEntityByName("Player")?.As<PlayerNew>();
            camera = FindEntityByName("Main Camera")?.As<CameraScript>();
            platformBoss = FindEntityByName("UCB13_1")?.As<PlatformBlockBoss>();
        }

        protected override void OnUpdate(float dt)
        {
            if (!cutsceneActive || player == null) return;

            // Get the current x position of the player
            float currentX = player.transform.Translation.x;
            float targetX = targetDestination.x;

            // Calculate the horizontal difference
            float xDiff = targetX - currentX;

            // Check if the player is close enough in the x direction
            if (Math.Abs(xDiff) < tolerance)
            {
                // Snap player to the target x while leaving y unchanged
                player.transform.Translation = new Vec3(targetX, player.transform.Translation.y, player.transform.Translation.z);

                // Reset scripted movement and animation state
                player.ChangeState(0);
                player.SetScriptedMovement(new Vec2(0, 0));

                // Handle any platform delay logic, then end scripted mode if appropriate
                if (platformBoss.freezeUCB)
                {
                    if (platformDelayTimer >= 4f)
                    {
                        player.isScriptedMode = false;
                        player.ResetScriptedMoveAudio();
                        cutsceneActive = false;
                        return;
                    }
                    else
                    {
                        platformDelayTimer += dt;
                        return;
                    }
                }

                return;
            }

            // Create a direction vector that only considers the x-axis difference.
            // Since we ignore the y component, we set it to 0.
            Vec2 direction = new Vec2(xDiff, 0);

            // Multiply by a speed multiplier if you want to boost speed (optional)
            direction = direction.Normalized();  // Adjust multiplier as needed

            // Feed this movement into the player
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

            //Make the player ignore real input
            if (player != null)
            {
                player.isScriptedMode = true;
                camera?.ResetOffset();
            }
        }
    }
}

