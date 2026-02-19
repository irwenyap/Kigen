using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class PlatformBlock : UnchargedBlock
    {
        private Transform transform;

        public float startPos, topLimit, bottomLimit;
        public float moveSpeed;
        public bool moveX;
        public bool startMovingUp; //To determine initial direction
        public uint crushTriggerId;
        private Transform crushTrigger;
        private bool isMoving = false;

        public bool moveSet;

        private float targetPos;
        private bool moveToStart = false;
        private bool movePlayer = false;

        private Animation platformAnimation;

        private bool playing;
        public bool offAudio;
        private float timer;

        private CameraScript camera;
        public bool triggerOffset = false;

        public bool stopPlatform = false;
        private bool platformInactive = false;

        protected override void OnInit()
        {
            base.OnInit();

            transform = GetComponent<Transform>();
            if (topLimit > bottomLimit)
            {
                //Endpos has to always be higher than startpos
                //This code switches it in the event that it is not
                float a = topLimit;
                topLimit = bottomLimit;
                bottomLimit = a;
            }

            //Set initial move direction based on startMovingUp flag
            moveToStart = !startMovingUp;

            platformAnimation = GetComponent<Animation>();
            if (crushTriggerId != 0) crushTrigger = FindEntityByID(crushTriggerId)?.GetComponent<Transform>();

            camera = FindEntityByName("Main Camera")?.As<CameraScript>();
        }
        protected override void OnUpdate(float dt)
        {
            if (platformInactive) return;

            if (playing && offAudio)
            {
                if (timer >= 27f)
                {
                    LoopAudio();
                }
                else timer += dt;
            }

            if(stopPlatform)
            {
                if (transform.Translation.y <= topLimit)
                {
                    camera.CameraShake(0.25f);
                    Audio.PlaySound(this.ID, "../Assets/Audio/Block SFX/PLATFORM STOP.wav", 0.5f);
                    stopPlatform = false;
                    platformInactive = true;
                    return;
                }
            }

            //if (Input.IsKeyPressed(KeyCode.O)) Activate(!isMoving);
            if (!isMoving) return;

            float nextPos = moveSpeed * dt;

            if (transform != null)
            {
                if (moveX)
                {
                    float direction = moveToStart ? -1 : 1;
                    transform.Translation = new Vec3(transform.Translation.x + (direction * nextPos), transform.Translation.y, transform.Translation.z);
                    if (movePlayer) player.transform.Translation = new Vec3(player.transform.Translation.x + (direction * nextPos), player.transform.Translation.y, player.transform.Translation.z);
                    if (crushTrigger != null) crushTrigger.Translation = new Vec3(crushTrigger.Translation.x + (direction * nextPos), crushTrigger.Translation.y, crushTrigger.Translation.z);

                    if (!moveToStart && transform.Translation.x >= bottomLimit)
                    {
                        //targetPos = topLimit;
                        moveToStart = true;
                    }
                    else if (moveToStart && transform.Translation.x <= topLimit)
                    {
                        //targetPos = bottomLimit;
                        moveToStart = false;
                    }
                }
                else
                {
                    float direction = moveToStart ? -1 : 1;
                    transform.Translation = new Vec3(transform.Translation.x, transform.Translation.y + (direction * nextPos), transform.Translation.z);
                    if (movePlayer) player.transform.Translation = new Vec3(player.transform.Translation.x, player.transform.Translation.y + (direction * nextPos), player.transform.Translation.z);
                    if (crushTrigger != null) crushTrigger.Translation = new Vec3(crushTrigger.Translation.x, crushTrigger.Translation.y + (direction * nextPos), crushTrigger.Translation.z);

                    if (!moveToStart && transform.Translation.y >= bottomLimit)
                    {
                        //targetPos = topLimit;
                        moveToStart = true;
                    }
                    else if (moveToStart && transform.Translation.y <= topLimit)
                    {
                        //targetPos = bottomLimit;
                        moveToStart = false;
                    }
                }
            }
        }
        private void LoopAudio()
        {
            playing = true;
            timer = 0f;
            Audio.PlaySound(this.ID, "../Assets/Audio/Block SFX/Platform Loop.wav", 1f);
        }
        protected override void OnCollisionEnter(Collision collision)
        {
            if (collision.collider != null)
            {
                if (collision.collider.Entity.ID == player.ID)
                {
                    if (moveSet) controlBlock.PlayerOnBlock(this, true);
                    else Activate(true);
                    movePlayer = true;

                    if (!moveToStart) return;
                    else if (!moveX & triggerOffset) camera.ChangeOffset(new Vec2(0, -400));
                }
            }
        }

        protected override void OnCollisionStay(Collision collision)
        {
            if(triggerOffset)
            {
                if (!moveToStart) camera.ResetOffset();
                else camera.ChangeOffset(new Vec2(0, -400));
            }
        }
        protected override void OnCollisionExit(Collision collision)
        {
            if (collision.collider != null)
            {
                if (collision.collider.Entity.ID == player.ID)
                {
                    if (moveSet) controlBlock.PlayerOnBlock(this, false);
                    else Activate(false);
                    movePlayer = false;

                    if (!moveX && triggerOffset) camera.ResetOffset();
                }
            }
        }
        public override void Activate(bool state)
        {
            isMoving = state;
            if (controlBlock != null)
            {
                controlBlock.drainingPlayer = state;
                //Console.WriteLine("Starting drain");
            }
            if (state)
            {
                platformAnimation?.PlayAnimation(false, true, false, true);
                if(!offAudio) LoopAudio();
            }
            else
            {
                platformAnimation?.PauseAnimation();
                playing = false;
                Audio.StopClip(this.ID, "../Assets/Audio/Block SFX/Platform Loop.wav");
            }

            //Console.WriteLine("Platform activation is " + isMoving);
        }
    }
}
