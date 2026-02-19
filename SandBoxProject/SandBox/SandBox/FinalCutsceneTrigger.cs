using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class FinalCutsceneTrigger : Entity
    {
        private bool activateTrigger = false;
        private bool activateCutscene = false;
        private bool cutsceneRunning = false;

        private PlayerNew player;
        private ScriptedMovementController cutsceneController;

        private PowerOutlet outlet3;

        protected override void OnInit()
        {   
            player = FindEntityByName("Player")?.As<PlayerNew>();
            cutsceneController = FindEntityByName("ScriptedMovement_Final Cutscene")?.As<ScriptedMovementController>();
            outlet3 = FindEntityByName("Power Outlet 3")?.As<PowerOutlet>();

        }

        //protected override void OnTriggerEnter(AABBCollider2D collider)
        //{
        //    //Trigger Cutscene
        //    if(activateTrigger && !activateCutscene)
        //    {
        //        Logger.Log("Start Cutscene OnTrigger", LogLevel.DEBUG);
        //        cutsceneController?.StartCutscene();

        //        //Disable Outlet 3
        //        outlet3.outletDeactivated = true;
        //        activateCutscene = true;
        //    }
        //}

        public void ActivateTrigger()
        {
            activateTrigger = true;
        }
    }
}
