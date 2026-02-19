using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class WilmerSunlight : Entity
    {
        public float chargeRate;

        private PlayerNew player;

        Entity tmpbatterBarVFX;
        Animation batteryBarAnim;

        //public bool chargingPlayer = false;
        Entity tmpUCB;

        Entity tmpDatalog;
        Animation tmpDatalogAnimation;
        private DatalogManager datalogManager;

        protected override void OnInit()
        {
            player = FindEntityByName("Player")?.As<PlayerNew>();

            //Getting reference to the Battery VFX
            tmpbatterBarVFX = FindEntityByName("BatteryBarVFX");

            batteryBarAnim = tmpbatterBarVFX?.GetComponent<Animation>();
            batteryBarAnim?.PauseAnimation(true);
            datalogManager = FindEntityByName("Datalog Manager")?.As<DatalogManager>();
        }
        protected override void OnUpdate(float dt)
        {
            if(player != null)
            {
                if (player.chargingPlayer)
                {
                    player.ChangeBatteryLevel(chargeRate * dt);

                    if (tmpbatterBarVFX != null)
                    {
                        batteryBarAnim?.PlayAnimation(false, true, false, true);
                    }
                }
            }
        }

        protected override void OnTriggerEnter(AABBCollider2D collider)
        {
            if (collider != null)
            {
                if (collider.Entity.ID == player?.ID)
                {
                    //datalogManager?.CompleteDatalog();
                    if (player != null)
                    {
                        player.chargingPlayer = true;
                        player.PlayChargingVFX();
                        Audio.PlaySound(this.ID, "../Assets/Audio/Character SFX/NANO_CHARGING.wav", 0.5f);

                        Audio.UpdateSound(player.ID, "../Assets/Audio/BGM/NANO_BGM.wav", 0f);

                        //If player is on the ground and in the light then change to STATE_CHARGING
                        //This ensures that it doesn't change state if the player jumps into the sunlight
                        if (player.onGround)
                        {
                            player?.ChangeState(3);
                        }
                    }
                }
            }
        }

        protected override void OnTriggerStay(AABBCollider2D collider)
        {
            if (player.currentBattery >= 95)
            {
                Audio.StopClip(this.ID, "../Assets/Audio/Character SFX/NANO_CHARGING.wav");
            }
        }

        protected override void OnTriggerExit(AABBCollider2D collider)
        {
            if (collider != null)
            {
                if (collider.Entity.ID == player?.ID)
                {
                    if(player != null)
                    {
                        player.chargingPlayer = false;
                        player.StopChargingVFX();
                        //Audio.StopClip(this.ID,"../Assets/Audio/Character SFX/NANO_CHARGING.wav");

                        batteryBarAnim?.PauseAnimation(true);

                        Audio.UpdateSound(player.ID, "../Assets/Audio/BGM/NANO_BGM.wav", 0.6f);

                        if (tmpDatalogAnimation != null)
                        {
                            tmpDatalogAnimation?.PauseAnimation(true);
                        }
                    }
                }
            }
        }
    }
}
