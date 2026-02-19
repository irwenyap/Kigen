using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class BossArenaManager : Entity
    {
        public bool arenaDeath = false;
        private FadeDeath fadeToBlackDeath;
        public bool scene1 = false;

        private PowerOutlet outlet1;
        private PowerOutlet outlet2;
        private PowerOutlet outlet3;

        private bool outlet1Active = false;
        private bool outlet2Active = false;
        private bool outlet3Active = false;

        private Cable cable1;
        private Cable cable2;
        private Cable cable3;

        private bool cable1Active = false;
        private bool cable2Active = false;
        private bool cable3Active = false;

        public float outlet1Timer = 0;
        public float outlet2Timer = 0;
        public float outlet3Timer = 0;

        private FinalCutsceneTrigger cutsceneTrigger;
        private ScriptedMovementController cutsceneController;
        private bool finalCutTriggered = false;

        //Camera
        private CameraScript camera;

        //Player
        private PlayerNew player;

        //Intro Cutscene
        private PlatformBlockBoss platformBlockBoss;
        private BossIntroCutscene bossIntro;
        private BossTracking bossHand;
        private bool introRan;
        private float introDelayTimer = 0f;

        //Boss Outlet
        private Entity outletBase;
        private Entity outletBack;
        private PowerOutletFinal outletFinal;

        //Final Cutscene
        private bool finalCutRan = false;

        //Core VFX
        private CoreVFX coreVFX;

        //UI
        private Entity batteryBarCharger;
        private Entity batteryBar;

        //Explosion VFX
        private ExplosionVFX explosionVFX;
        private bool explosion2Ran = false;
        private float explosion2DelayTimer = 0f;
        private float explosionDelayTimer = 0f;
        private bool explosionRan = false;

        //Credit
        private Credits credits;
        private float creditDelayTimer = 0f;
        private bool creditRan = false;


        private DialogueManager dialogueManager;
        protected override void OnInit()
        {
            base.OnInit();
            if (scene1) return;
            dialogueManager = FindEntityByName("Dialogue Manager")?.As<DialogueManager>();
            //Player
            player = FindEntityByName("Player")?.As<PlayerNew>();
            fadeToBlackDeath = FindEntityByName("Fade Death")?.As<FadeDeath>();

            //Camera
            camera = FindEntityByName("Main Camera")?.As<CameraScript>();

            //UI
            batteryBar = FindEntityByName("Battery Bar");
            batteryBarCharger = FindEntityByName("Battery Bar Charger");
            batteryBar.IsActive = true;
            batteryBarCharger.IsActive = true;

            //Core VFX
            coreVFX = FindEntityByName("CoreVFX")?.As<CoreVFX>();
            coreVFX.IsActive = false;

            //Explosion VFX
            explosionVFX = FindEntityByName("ExplosionVFX")?.As<ExplosionVFX>();
            explosionVFX.IsActive = false;

            //Credit
            credits = FindEntityByName("Credits")?.As<Credits>();

            outlet1 = FindEntityByName("Power Outlet 1")?.As<PowerOutlet>();
            outlet2 = FindEntityByName("Power Outlet 2")?.As<PowerOutlet>();
            outlet3 = FindEntityByName("Power Outlet 3")?.As<PowerOutlet>();

            outlet1.IsActive = true;
            outlet2.IsActive = true;
            outlet3.IsActive = true;

            cable1 = FindEntityByName("Cable Wire_1")?.As<Cable>();
            cable2 = FindEntityByName("Cable Wire_2")?.As<Cable>();
            cable3 = FindEntityByName("Cable Wire_3")?.As<Cable>();

            cutsceneTrigger = FindEntityByName("Cut Scene Trigger Box")?.As<FinalCutsceneTrigger>();
            cutsceneController = FindEntityByName("ScriptedMovement_Final Cutscene")?.As<ScriptedMovementController>();

            platformBlockBoss = FindEntityByName("UCB13_1")?.As<PlatformBlockBoss>();
            bossIntro = FindEntityByName("Boss Hand Intro")?.As<BossIntroCutscene>();
            bossHand = FindEntityByName("Boss Hand Idle")?.As<BossTracking>();

            //On Start, set Intro to active and Idle to inactive
            bossIntro.IsActive = true;
            bossHand.IsActive = false;

            //Outlet 3
            outletBase = FindEntityByName("Outlet Base")?.As<Asda>();
            outletBack = FindEntityByName("Outlet Backing")?.As<Asda>();
            outletFinal = FindEntityByName("Power Outlet Final")?.As<PowerOutletFinal>();

            outletBase.IsActive = false;
            outletBack.IsActive = false;
            outletFinal.IsActive = false;

        }

        protected override void OnUpdate(float dt)
        {
            if (scene1) return;

            //==========================================  Boss Intro Cutscene  ==========================================

            //If platform reaches destination then start intro delay timer
            if (platformBlockBoss.freezeUCB)
            {
                //If delay timer reaches 2s, then play the intro animation
                if (introDelayTimer >= 3f)
                {
                    if (!introRan)
                    {
                        bossIntro.PlayBossIntroAnimation();
                        Audio.PlaySound(this.ID, "../Assets/Audio/Environment SFX/BOSS_INTRO.wav", 0.3f);
                        introRan = true;
                        arenaDeath = true;
                    }
                }
                else
                {
                    introDelayTimer += dt;
                }
            }

            if (bossIntro.introComplete && !bossHand.IsActive)
            {
                bossHand.IsActive = true;
                bossIntro.IsActive = false;
            }


            //==========================================  Final Cutscene  ==========================================
            if (cable1Active && cable2Active && !finalCutTriggered)
            {
                //cutsceneTrigger.ActivateTrigger();
                cutsceneController?.StartCutscene();

                //Disable Outlet 3
                outlet3.outletDeactivated = true;
                finalCutTriggered = true;
            }

            if (outlet1.OutletStatus() && !cable1Active)
            {
                if (outlet1Timer >= 3f)
                {
                    //Play Cable 1 Animation & SFX
                    cable1.PlayCableAnimation(1);
                    camera?.CameraShake(0.2f);
                    cable1Active = true;
                }
                else
                {
                    outlet1Timer += dt;
                }
            }

            if (outlet2.OutletStatus() && !cable2Active)
            {
                if (outlet2Timer >= 3f)
                {
                    //Play Cable 2 Animation & SFX
                    cable2.PlayCableAnimation(2);
                    //camera?.CameraShake(0.2f);
                    cable2Active = true;
                }
                else
                {
                    outlet2Timer += dt;
                }
            }

            //If outlet base has been activated then trigger final explosion sequence
            if (outletBase.IsActive && !finalCutRan)
            {
                finalCutRan = true;
                outletFinal.IsActive = true; //Activate final sequence outlet3
                outlet3.IsActive = false; //Deactive original outlet3
                outletFinal.startTimer = true; //Start Delay for Outlet 3 Animation Timer
            }

            //Activate cable 3 > Warning Loop > Zoom Out
            if (outletFinal.animRan && !cable3Active)
            {
                if (outlet3Timer >= 3f)
                {
                    cable3.PlayCableAnimation(3); //Play Cable 3 Animation

                    //Hide UI
                    batteryBar.IsActive = false;
                    batteryBarCharger.IsActive = false;

                    //Camera Changes
                    camera?.ChangeOffset(new Vec2(0, 500));
                    camera?.ChangeZoom(0.650f, 3f);
                    camera?.ChangeCameraShake(0.5f);
                    camera?.CameraShake(10f);

                    //Activate Core VFX
                    coreVFX.IsActive = true;
                    coreVFX.PlayAnimation();

                    //Play Warning Loop
                    Audio.PlaySound(this.ID, "../Assets/Audio/Environment SFX/WARNING_LOOP.wav", 1f);
                    cable3Active = true;
                }
                else
                {
                    outlet3Timer += dt;
                }
            }

            //Explosion Delay + Explosion
            if (cable3Active && !explosion2Ran)
            {
                //if (explosionRan && explosion2Ran) return;

                if(explosionDelayTimer >= 6.5f && !explosionRan)
                {
                    Audio.PlaySound(this.ID, "../Assets/Audio/Voiceovers/Dialogue105.wav", 0.7f);
                    explosionRan = true;
                }
                else
                {
                    explosionDelayTimer += dt;
                }

                if(explosion2DelayTimer >= 8f)
                {
                    //coreVFX.IsActive = false;
                    Audio.StopClip(this.ID, "../Assets/Audio/Environment SFX/WARNING_LOOP.wav");
                    

                    explosionVFX.IsActive = true;
                    explosionVFX.PlayAnimation();
                    Audio.PlaySound(this.ID, "../Assets/Audio/Environment SFX/BOSS_EXPLOSION.wav", 0.6f);
                    Audio.StopClip(player.ID, "../Assets/Audio/BGM/NANO_BOSS BGM.wav");
                    explosion2Ran = true;
                }
                else
                {
                    explosion2DelayTimer += dt;
                }
            }

            //Credit Scene
            if(explosion2Ran)
            {
                if (creditRan) return;

                if (creditDelayTimer >= 8f)
                {
                    credits.activate = true;
                    credits.IsActive = true;
                    credits.credits2.IsActive = true;

                    camera.camera.SetBloom(0);
                        
                    Audio.PlaySound(this.ID, "../Assets/Audio/BGM/CREDITS_BGM.wav", 0.2f, false);
                    //Audio.PlaySound(this.ID, "../Assets/Audio/Voiceovers/Dialogue88-89.wav", 0.8f);
                    dialogueManager.PlayDialogue(28, 0.8f, false);
                    creditRan = true;

                    //Play Credit BGM
                }
                else creditDelayTimer += dt; Logger.Log($"creditDelayTimer: {creditDelayTimer}",LogLevel.DEBUG);
            }

            //Death Transition Screen
            if (fadeToBlackDeath.FinishAnim()) Input.ChangeScenePath("../Assets/Scenes/NANO_Level2.scene");
        }

        public void ArenaDeath()
        {
            player.ChangeState(4);
            player.isScriptedMode = false;

            //Fade to Black Transition
            fadeToBlackDeath.PlayAnimation();

            //Play Audio
            //Audio.PlaySound("../Assets/Audio/Environment SFX/BOSS_EXPLOSION.wav", 1f);
        }
    }
}
