using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class PowerOutlet : Entity
    {
        private bool outletActivated;
        public bool outletDeactivated = false;
        private bool canInteract = true;

        public int outletNumber;

        private PlayerNew player;

        //Outlet 3 Only
        private bool startTimer;
        private float outletTimer;

        //Interactable UI
        private Entity interactUI;
        private bool interactable;
        private Transform interactUITransform;

        //Outlet Animation Component
        //private Animation outletAnimation;
        private Animation anim;
        private AniData tmpAnim;

        private bool failPlayed = false;
        private DialogueManager dialogueManager;

        protected override void OnInit()
        {
            dialogueManager = FindEntityByName("Dialogue Manager")?.As<DialogueManager>();

            //References
            player = FindEntityByName("Player")?.As<PlayerNew>();
            interactUI = FindEntityByName($"Power Outlet Interact UI_{outletNumber}");
            interactUITransform = interactUI.GetComponent<Transform>();

            //Animation Component
            anim = GetComponent<Animation>();

            interactUI.IsActive = false;

        }

        protected override void OnUpdate(float dt)
        {
            //Animation Component
            tmpAnim = anim.data;

            //Outlet 3 Only
            if (startTimer)
            {
                if (outletTimer >= 2f)
                {
                    canInteract = true;
                    outletDeactivated = false;

                    outletTimer = 0f;
                    startTimer = false;
                }
                else
                {
                    outletTimer += dt;
                    Logger.Log($"{outletTimer}", LogLevel.DEBUG);
                    return;
                }
            }

            if (!canInteract) return;

            if (interactable)
            {
                if (Input.IsKeyPressed(KeyCode.E) || Input.IsGamepadButtonPressed(ButtonCode.GamepadButtonX))
                {
                    interactUI.IsActive = false;
                    ActivateOutlet();
                }
            }

            if(outletDeactivated)
            {
                DeactivateOutlet();
            }
        }

        private void ActivateOutlet()
        {

            if(outletNumber == 1)
            {
                if(tmpAnim.currentFrame != 0) tmpAnim.currentFrame = 0;
                tmpAnim.startFrame = 0;
                tmpAnim.endFrame = 125;
                tmpAnim.playOnce = true;
                tmpAnim.isLooping = false;
                anim.data = tmpAnim;

                Audio.PlaySound(this.ID,"../Assets/Audio/Environment SFX/OUTLET_AUDIO.wav", 0.2f);
                //Audio.PlaySound(this.ID,"../Assets/Audio/Voiceovers/Dialogue78.wav", 0.9f);
                dialogueManager.PlayDialogue(25, 0.9f, false);
                outletDeactivated = true;
            }
            else if(outletNumber == 2)
            {
                if (tmpAnim.currentFrame != 0) tmpAnim.currentFrame = 0;
                tmpAnim.startFrame = 0;
                tmpAnim.endFrame = 125;
                tmpAnim.playOnce = true;
                tmpAnim.isLooping = false;
                anim.data = tmpAnim;

                Audio.PlaySound(this.ID,"../Assets/Audio/Environment SFX/OUTLET_AUDIO.wav", 0.2f);
                //Audio.PlaySound(this.ID,"../Assets/Audio/Voiceovers/Dialogue79.wav", 0.9f);
                dialogueManager.PlayDialogue(26, 0.9f, false);
                outletDeactivated = true;
            }
            else
            {
                if(tmpAnim.currentFrame != 0) tmpAnim.currentFrame = 0;

                tmpAnim.startFrame = 0;
                tmpAnim.endFrame = 83;
                tmpAnim.playOnce = true;
                tmpAnim.isLooping = false;
                anim.data = tmpAnim;

                Audio.PlaySound(this.ID,"../Assets/Audio/Environment SFX/OUTLET FAIL_AUDIO.wav", 0.2f);

                //Play Sound
                if (!failPlayed)
                {
                    //Audio.PlaySound(this.ID, "../Assets/Audio/Voiceovers/Dialogue80.wav", 0.9f);
                    dialogueManager.PlayDialogue(27, 0.9f, false);
                    failPlayed = true;
                }


                outletDeactivated = true;
                startTimer = true;
            }

        }

        public void DeactivateOutlet()
        {
            canInteract = false;
        }

        public bool OutletStatus()
        {
            if(outletDeactivated)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        protected override void OnTriggerEnter(AABBCollider2D collider)
        {
            if (outletDeactivated) return;

            if (collider != null && collider.Entity.ID == player?.ID)
            {
                if (player != null)
                {
                    interactable = true;
                    interactUI.IsActive = true;
                }

            }
        }

        protected override void OnTriggerStay(AABBCollider2D collider)
        {
            if (collider != null && collider.Entity.ID == player?.ID && outletTimer > 1.9)
            {
                    interactable = true;
                    interactUI.IsActive = true;
            }
        }

        protected override void OnTriggerExit(AABBCollider2D collider)
        {
            if (collider != null && collider.Entity.ID == player?.ID)
            {
                interactable = false;
                interactUI.IsActive = false;
            }
        }
    }
}
