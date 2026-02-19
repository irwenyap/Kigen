using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class PlatformBlockReturn : UnchargedBlock
    {
        private Transform transform;

        public float startPos;
        public float topLimit;
        public float bottomLimit;
        public float moveSpeed;
        public bool startMovingUp; //To determine initial direction
        private bool isMoving = false;
        public uint crushTriggerId;
        private Transform crushTrigger;
        private float targetPos;
        private bool moveToStart = false;
        private bool movePlayer = false;

        private Animation platformAnimation;

        private bool playing;
        private float timer;

        protected override void OnInit()
        {
            base.OnInit();

            transform = GetComponent<Transform>();

            startPos = transform.Translation.y;

            //if (topLimit > bottomLimit)
            //{
            //    //Endpos has to always be higher than startpos
            //    //This code switches it in the event that it is not
            //    float a = topLimit;
            //    topLimit = bottomLimit;
            //    bottomLimit = a;
            //}

            //Set initial move direction based on startMovingUp flag
            moveToStart = !startMovingUp;
            if (crushTriggerId != 0) crushTrigger = FindEntityByID(crushTriggerId)?.GetComponent<Transform>();

            platformAnimation = GetComponent<Animation>();
        }
        protected override void OnUpdate(float dt)
        {
            if (playing)
            {
                if (timer >= 27f)
                {
                    LoopAudio();
                }
                else
                {
                    timer += dt;
                }
            }

            // When the platform is actively moving (player in contact), run the existing logic.
            if (isMoving)
            {
                float nextPos = moveSpeed * dt;

                if (transform != null)
                {
                    float direction = moveToStart ? -1 : 1;
                    transform.Translation = new Vec3(
                        transform.Translation.x,
                        transform.Translation.y + (direction * nextPos),
                        transform.Translation.z);

                    if (movePlayer)
                    {
                        player.transform.Translation = new Vec3(
                            player.transform.Translation.x,
                            player.transform.Translation.y + (direction * nextPos),
                            player.transform.Translation.z);
                    }
                    if (crushTrigger != null) crushTrigger.Translation = new Vec3(crushTrigger.Translation.x, crushTrigger.Translation.y + (direction * nextPos), crushTrigger.Translation.z);

                    // Switch directions when reaching limits.
                    if (!moveToStart && transform.Translation.y >= topLimit)
                    {
                        moveToStart = true;
                    }
                    else if (moveToStart && transform.Translation.y <= bottomLimit)
                    {
                        moveToStart = false;
                    }
                }
            }
            // Else, if not active, return the platform to its starting position.
            else
            {
                if (transform != null)
                {
                    float currentY = transform.Translation.y;
                    // Check if we're not at the starting position (using a small threshold to avoid floating point issues).
                    if (Math.Abs(currentY - startPos) > 0.001f)
                    {
                        float delta = moveSpeed * dt;
                        // Determine the direction needed to return to startPos.
                        float direction = startPos > currentY ? 1 : -1;
                        // Ensure we don't overshoot the target.
                        if (Math.Abs(currentY - startPos) < delta)
                        {
                            currentY = startPos;
                        }
                        else
                        {
                            currentY += direction * delta;
                        }
                        transform.Translation = new Vec3(transform.Translation.x, currentY, transform.Translation.z);

                        if (crushTrigger != null)
                            crushTrigger.Translation =
                                new Vec3(
                                    crushTrigger.Translation.x,
                                    crushTrigger.Translation.y + direction * delta,
                                    crushTrigger.Translation.z);
                    }
                }
            }
        }
        private void LoopAudio()
        {
            playing = true;
            timer = 0f;
            Audio.PlaySound(this.ID, "../Assets/Audio/Block SFX/Platform Loop.wav", 0.7f);
        }
        protected override void OnCollisionEnter(Collision collision)
        {
            if (collision.collider != null)
            {
                if (collision.collider.Entity.ID == player.ID)
                {
                    Activate(true);
                    movePlayer = true;
                }
            }
        }
        protected override void OnCollisionExit(Collision collision)
        {
            if (collision.collider != null)
            {
                if (collision.collider.Entity.ID == player.ID)
                {
                    Activate(false);
                    movePlayer = false;
                }
            }
        }
        public override void Activate(bool state)
        {
            //Draining Player if on controlBlock
            if (controlBlock != null) controlBlock.drainingPlayer = state;

            isMoving = state;

            if (state)
            {
                platformAnimation?.PlayAnimation();
                LoopAudio();
            }
            else
            {
                platformAnimation?.PauseAnimation();
                playing = false;
                Audio.StopClip(this.ID, "../Assets/Audio/Block SFX/Platform Loop.wav");
            }   
        }
    }
}
