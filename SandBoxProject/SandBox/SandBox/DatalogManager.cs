using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class DatalogManager : Entity
    {
        public int datalogAmount;
        public float interactionDelay;

        public float cameraShakeDuration, cameraShakeDelay, changeToDoorDelay, playDoorAnimationDelay, changeToPlayerDelay, restartMovementDelay;
        public float finalDoorCutsceneOffset;

        private int datalogCounter = 0;
        private List<Datalog> datalogArray = new List<Datalog>();
        private AniData datalogAniData;
        private Entity finalDoorEntity;
        private FinalDoor finalDoor;
        private bool datalogOpen = false, datalogClose = false;
        private float timer = 0, closeTimer = 0;
        private PlayerNew player;
        private CameraScript camera;
        private bool startCutscene = false;
        private float cutsceneTimer = 0f;
        private bool cameraShaking = false;

        private Entity finalDoorAnimationEntity;
        private Animation finalDoorAnimation;
        private AniData finalDoorAniData;
        private Entity finalDoorHandPrint;
        private bool cutscene1, cutscene2, cutscene3, cutscene4, cutscene5, cutscene6;

        protected override void OnInit()
        {
            InitializeDatalogArray();
            player = FindEntityByName("Player")?.As<PlayerNew>();
            camera = FindEntityByName("Main Camera")?.As<CameraScript>();
            finalDoorAnimationEntity = FindEntityByName("Final Door Animation");
            finalDoorAnimation = finalDoorAnimationEntity?.GetComponent<Animation>();
            finalDoorHandPrint = FindEntityByName("Hand print");

            finalDoorAnimationEntity.IsActive = false;
        }
        protected override void OnUpdate(float dt)
        {
            finalDoorAniData = finalDoorAnimation.data;
            

            // Gamepad B button to close datalog
            if (Input.IsKeyPressed(KeyCode.Enter) || Input.IsGamepadButtonPressed(ButtonCode.GamepadButtonB)) CloseDatalog();
            //if(Input.IsKeyPressed(KeyCode.H)) startCutscene = true;
            //if (Input.IsKeyPressed(KeyCode.P)) CompleteDatalog();

            if (datalogOpen)
            {
                timer += dt;
                datalogAniData = datalogArray[datalogCounter].animation.data;
                if (datalogAniData.currentFrame == datalogAniData.endFrame) {
                    datalogArray[datalogCounter].freeze.IsActive = true;
                    datalogArray[datalogCounter].entity.IsActive = false;
                }
            }

            
            

            if (datalogClose)
            {
                closeTimer += dt;

                if(closeTimer >= 5f)
                {
                    datalogClose = false;
                    datalogArray[datalogCounter-1].reverse.IsActive = false;
                }
            }

            if (startCutscene)
            {
                cutsceneTimer += dt;
                if (cutsceneTimer >= cameraShakeDelay + changeToDoorDelay + playDoorAnimationDelay + changeToPlayerDelay + restartMovementDelay)
                {
                    if (cutscene6) return;
                    cutscene6 = true;
                    player.SetMovement(true);
                }
                else if (cutsceneTimer >= cameraShakeDelay + changeToDoorDelay + playDoorAnimationDelay + changeToPlayerDelay)
                {
                    if (cutscene5) return;
                    cutscene5 = true;
                    camera.ChangeTarget(player.GetComponent<Transform>());
                    camera.ChangeOffset(new Vec2(0f, 0f));
                }
                else if (cutsceneTimer >= cameraShakeDelay + changeToDoorDelay + playDoorAnimationDelay)
                {
                    if (cutscene4) {
                        if (finalDoorAniData.currentFrame == finalDoorAniData.endFrame)
                        {
                            finalDoorAnimationEntity.IsActive = false;
                        }
                        return;
                    }
                    cutscene4 = true;
                    finalDoorHandPrint.IsActive = false;
                    finalDoorAnimationEntity.IsActive = true;
                    finalDoorAnimation?.PlayAnimation(true, true, true, false);
                    finalDoor.CompleteDatalog(); // Sets the final door to open
                    Audio.PlaySound(this.ID, "../Assets/Audio/Environment SFX/FINAL DOOR_OPENING.wav", 1f);
                }
                else if (cutsceneTimer >= cameraShakeDelay + changeToDoorDelay)
                {
                    if (cutscene3) return;
                    cutscene3 = true;
                    camera.ChangeTarget(finalDoorEntity.GetComponent<Transform>());
                    camera.ChangeOffset(new Vec2(0f, 200f + finalDoorCutsceneOffset));
                    Audio.PlaySound(this.ID, "../Assets/Audio/Environment SFX/FINAL DOOR_PRE-CAMERA-PAN.wav", 1f);
                }
                else if (cutsceneTimer >= cameraShakeDelay)
                {
                    if (cutscene2) return;
                    cutscene2 = true;
                    if (!cameraShaking)
                    {
                        cameraShaking = true;
                        camera.CameraShake(cameraShakeDuration);
                        Audio.PlaySound(this.ID, "../Assets/Audio/Environment SFX/CAMERA_SHAKE.wav", 1f);
                    }
                }
                else
                {
                    if (cutscene1) return;
                    cutscene1 = true;
                    player.SetMovement(false);
                }

                
            }
        }
        private void InitializeDatalogArray()
        {
            for (int i = 1; i <= datalogAmount; i++)
            {
                Entity entity = FindEntityByName($"Datalog{i}");
                Entity reverse = FindEntityByName($"Datalog{i}_Reverse");
                Entity freeze = FindEntityByName($"Datalog{i}_Freeze");
                Animation animation = entity?.GetComponent<Animation>();
                Animation animationReverse = reverse?.GetComponent<Animation>();
                Renderer renderer = entity?.GetComponent<Renderer>();

                

                if (entity != null && animation != null && reverse != null && animationReverse != null)
                {
                    entity.IsActive = false;
                    reverse.IsActive = false;
                    datalogArray?.Add(new Datalog(entity, animation, reverse, animationReverse, freeze));
                }
            }

            finalDoorEntity = FindEntityByName("Final Door");
            finalDoor = finalDoorEntity?.As<FinalDoor>();
        }
        public void CompleteDatalog()
        {
            datalogArray[datalogCounter].entity.IsActive = true;
            datalogArray[datalogCounter].animation.PlayAnimation(true, true, true, false);

            datalogOpen = true;
            timer = 0f;

            Audio.PlaySound(this.ID, "../Assets/Audio/Datalog SFX/DataLog Transition In.wav", 1f);
            player.SetMovement(false);

            finalDoor.CompleteDatalog();
        }
        public void CloseDatalog()
        {
            if(datalogOpen && timer >= interactionDelay)
            {
                datalogArray[datalogCounter].reverse.IsActive = true;
                datalogArray[datalogCounter].animationReverse.PlayAnimation(true,true, true, false);
                datalogArray[datalogCounter].entity.IsActive = false;
                datalogArray[datalogCounter].freeze.IsActive = false;

                datalogOpen = false;
                datalogClose = true;
                closeTimer = 0f;

                Audio.PlaySound(this.ID, "../Assets/Audio/Datalog SFX/DataLog Transition Out.wav", 1f);
                datalogArray[datalogCounter].entity.IsActive = false;
                player.SetMovement(true);

                if (datalogCounter < datalogAmount)
                {
                    datalogCounter++;

                    if(datalogCounter >= datalogAmount)
                    {
                        startCutscene = true;
                    }
                }
            }
        }
    }
}