using System;
using System.Collections.Generic;
using System.Data;
using System.Diagnostics;
using System.Diagnostics.SymbolStore;
using System.IO;
using ScriptCore;

namespace SandBox
{
    public class PlayerNew : Entity
    {
        //State Variables
        public bool onGround = true;
        public bool chargingPlayer = false;
        public bool drainingPlayer = false;
        private bool facingRight = true;
        private bool canMove = true;

        //Movement Control Parameters
        public float maxSpeed = 400f;
        public float groundAcceleration = 550f;
        public float groundDeceleration = 170f;
        public float airControlSpeed = 450f;
        public float airDeceleration = 25f;
        public float jumpForce = 62000f;

        //Movement Related Variables
        private Vec2 velocity;
        private Vec2 desiredVelocity;
        private float acceleration;
        private float maxSpeedChange;
        private Vec3 initialScale;
        private float initialGravity;

        //Component References
        public Rigidbody2D body;
        public Transform transform;
        public Renderer renderer;

        //PlayerBehaviour Variables
        public float maxBattery = 100f;
        public bool platformCrushing = false;

        //Switched to a getter and setter for ease of use
        public float currentBattery;

        public float drainRate = 5f;
        public float chargeRate = 5f;

        //Checkpoint
        Checkpoint checkpoint1;
        Transform cp1Transform;

        //Battery Bar
        BatteryBar batteryBar;
        public UInt32 idBatt;
        Entity tmpBattery;

        //Player Animation
        Animation anim;
        AniData tmpAnim;

        //CB Reference
        ChargedBlock chargedBlock;
        Entity tmpCB;

        ChargedBlock chargedBlock2;
        Entity tmpCB2;

        //UCB Reference
        UnchargedBlock UCB1; //Reference to Uncharged Block 1
        Entity tmpUCB1;
        public bool UCBCheck = false;
        public bool CBCheck = false;
        public float penetrationDeath = 20f;

        //State Machine
        private int currentState = 0;
        private int previousState = 0;

        //C# state machine
        private const int STATE_IDLE = 0;
        private const int STATE_MOVING = 1;
        private const int STATE_JUMPING = 2;
        private const int STATE_CHARGING = 3;
        private const int STATE_LANDING = 4;

        //Audio
        public bool BGMPlayed;
        private bool JumpPlayed = true;
        private bool LandPlayed = true;
        private bool MoveToIdlePlayed = true;
        private bool IdleToMovePlayed = true;
        private bool CheckpointPlayed = true;
        private bool wasAirborne = false;
        public bool gamePaused = false;
        private bool bossBGMPlayed = false;
        private float bossBGMTimer = 0f;
        private bool isMoveSoundPlaying = false;

        //VFX
        private JumpVFX jumpVFX;
        private Entity chargingVFX;
        private Rigidbody2D chargingVFXRB;

        private CheckpointManager cpManager;

        //Boss Arena
        private PlatformBlockBoss platformBoss;
        private BossArenaManager bossArenaManager;
        private Entity fadeTransition;

        //Scripted Movement
        public bool isScriptedMode = false;
        private Vec2 scriptedMovement = new Vec2(0, 0);
        private bool scriptedMoveAudioPlayed = false;

        //Camera
        private CameraScript camera;

        string BGMpath = "../Assets/Audio/BGM/NANO_BGM.wav";

        protected override void OnInit()
        {
            #region Components
            transform = GetComponent<Transform>();
            body = GetComponent<Rigidbody2D>();
            anim = GetComponent<Animation>();
            renderer = GetComponent<Renderer>();
            #endregion

            #region References
            renderer?.SetTextureToEntity("194d9833da9-20760c263943842d-8a523d96658923d");

            tmpBattery = FindEntityByID(idBatt);

            batteryBar = tmpBattery?.As<BatteryBar>();
            if (batteryBar != null) Console.WriteLine("Found battery");

            Entity tmpCP = FindEntityByName("Checkpoint1");

            checkpoint1 = tmpCP?.As<Checkpoint>();
            cp1Transform = tmpCP?.GetComponent<Transform>();

            jumpVFX = FindEntityByName("JumpVFX")?.As<JumpVFX>();

            chargingVFX = FindEntityByName("ChargingVFX");
            chargingVFXRB = chargingVFX?.GetComponent<Rigidbody2D>();
            
            platformBoss = FindEntityByName("UCB13_1")?.As<PlatformBlockBoss>();
            bossArenaManager = FindEntityByName("Boss Arena Manager")?.As<BossArenaManager>();

            camera = FindEntityByName("Main Camera")?.As<CameraScript>();
            #endregion

            #region Variables 
            initialScale = transform.Scale;
            currentBattery = maxBattery;
            cpManager = FindEntityByName("Checkpoint Manager")?.As<CheckpointManager>();
            initialGravity = body.GravityScale;
            #endregion

            camera?.ResetOffset();
            camera?.ResetZoom(1f);
        }

        private void updateListener()
        {
            if (transform == null || body == null)
                return;

            Vec3 playerPosition = transform.Translation;
            Vec3 playerVelocity = new Vec3(body.Velocity.x, body.Velocity.y, 0f);
            Vec3 playerForwards = new Vec3(0, 0, 1);
            Vec3 playerUP = new Vec3(0, 1, 0);

            Audio.SetListener(playerPosition, playerVelocity, playerForwards, playerUP);

        }

        protected override void OnUpdate(float dt)
        {
            //anim
            tmpAnim = anim.data;

            if (platformCrushing && onGround) PlayerDeath();

            //Handling Movement
            if (canMove)
            {
                Vec2 moveDirection = new Vec2(0, 0);

                if (isScriptedMode)
                {
                    //Directly apply full speed in the scripted direction
                    velocity = body.Velocity;
                    velocity.x = scriptedMovement.x * maxSpeed;

                    //Update the player's position directly
                    transform.Translation = Vec3.AddToX(transform.Translation, velocity.x * dt);

                    //Force the sprite's facing direction based on scripted input
                    if (scriptedMovement.x > 0)
                        facingRight = true;
                    else if (scriptedMovement.x < 0)
                        facingRight = false;

                    //Only trigger the moving state (and its audio) once when scripted movement starts
                    if (velocity.x != 0 && !scriptedMoveAudioPlayed)
                    {
                        ChangeState(STATE_MOVING, true);
                        scriptedMoveAudioPlayed = true;
                    }
                }
                else
                {
                    //Handling Left & Right
                    if (Input.IsKeyDown(KeyCode.A) || Input.GetGamepadLeftStick().x < -0.1f)
                    {
                        moveDirection = new Vec2(-1, 0);
                        facingRight = false;
                    }
                    else if (Input.IsKeyDown(KeyCode.D) || Input.GetGamepadLeftStick().x > 0.1f)
                    {
                        moveDirection = new Vec2(1, 0);
                        facingRight = true;
                    }

                    //Handling Jump
                    if ((Input.IsKeyPressed(KeyCode.Space) || Input.IsGamepadButtonPressed(ButtonCode.GamepadButtonA)) && onGround)
                    {
                        body.AddImpulseForce(new Vec2(0, 1), jumpForce);

                        //JumpVFX
                        jumpVFX?.SpawnVFX(transform);

                        onGround = false; //Set to airborne until collision resets it
                        LandPlayed = true;
                        wasAirborne = true;
                        ChangeState(STATE_JUMPING);
                    }

                    velocity = body.Velocity;

                    if (onGround) //Ground movement
                    {
                        if (moveDirection.x != STATE_IDLE)
                        {
                            if (currentState != STATE_MOVING)
                            {
                                ChangeState(STATE_MOVING);
                            }
                            //Calculate desired velocity for smooth movement
                            desiredVelocity = new Vec2(moveDirection.x * maxSpeed, velocity.y);

                            //Smoothly adjust velocity
                            velocity.x = MoveTowards(velocity.x, desiredVelocity.x, groundAcceleration);
                        }
                        else
                        {
                            if (currentState != STATE_IDLE)
                            {
                                ChangeState(STATE_IDLE);
                            }
                            //Apply deceleration if no input is pressed
                            velocity.x = MoveTowards(velocity.x, 0, groundDeceleration);

                            //If velocity is 0, player is under sunlight and is on the ground, change to STATE_CHARGING
                            if (velocity.x <= 0 && chargingPlayer)
                            {
                                ChangeState(STATE_CHARGING);
                            }
                            else if (velocity.x <= 0)
                            {
                                ChangeState(STATE_IDLE);
                            }
                        }
                    }
                    else //Air movement
                    {
                        Audio.StopClip(this.ID,"../Assets/Audio/Character SFX/NANO_MOVE.wav");

                        if (moveDirection.x != 0)
                        {
                            //Override horizontal velocity for instant response in air
                            velocity.x = moveDirection.x * airControlSpeed;
                        }
                        else
                        {
                            //Apply deceleration if no input is pressed
                            velocity.x = MoveTowards(velocity.x, 0, airDeceleration);
                        }
                    }

                    //Apply updated velocity
                    transform.Translation = Vec3.AddToX(transform.Translation, velocity.x * dt);
                }
            }

            //Reduced code for flip
            transform.Scale = new Vec3(facingRight ? 256 : -256, transform.Scale.y, transform.Scale.z);

            //Handling Battery Input
            //if (Input.IsKeyPressed(KeyCode.H))
            //{
            //    ChangeBatteryLevel(10);
            //}

            //if (Input.IsKeyPressed(KeyCode.J))
            //{
            //    ChangeBatteryLevel(-10);
            //}


            //if (Input.IsKeyPressed(KeyCode.Slash))
            //{
            //    Input.ChangeScenePath("../Assets/Scenes/Main Menu.scene");
            //}

            //ChargingVFX 
            if (chargingVFX.IsActive) chargingVFXRB.Position = new Vec2(transform.Translation.x, transform.Translation.y);

            //Updates Listener
            updateListener();

            //Audio
            HandleAudio(dt);
        }

        private void HandleAudio(float dt)
        {
            if(!bossBGMPlayed && platformBoss.freezeUCB)
            {
                Audio.StopClip(this.ID,BGMpath);

                if(bossBGMTimer >= 4f)
                {
                    Audio.PlaySound(this.ID,"../Assets/Audio/BGM/NANO_BOSS BGM.wav", 0.15f, true);
                    bossBGMPlayed = true;
                }
                else
                {
                    bossBGMTimer += dt;
                }
            }

            //BGM
            if (!Audio.ClipIsPlaying(BGMpath) && !gamePaused && !bossBGMPlayed)
            {
                Audio.PlaySound(this.ID, BGMpath, 0.6f, true);
            }

            if (!JumpPlayed)
            {
                Audio.PlaySound(this.ID,"../Assets/Audio/Character SFX/NANO_JUMP.wav", 0.6f);
                JumpPlayed = true;
            }

            if (!LandPlayed)
            {
                if (CBCheck)
                {
                    Audio.PlaySound(this.ID,"../Assets/Audio/Character SFX/NANO_LAND_CHARGEDBLOCK.wav", 0.4f);
                }
                else
                {
                    Console.WriteLine("[DEBUG] Attempting to play landing sound.");
                    Audio.StopClip(this.ID,"../Assets/Audio/Character SFX/NANO_LAND_GROUND.wav");
                }

                LandPlayed = true;
            }

            if (!MoveToIdlePlayed)
            {
                Audio.PlaySound(this.ID,"../Assets/Audio/Character SFX/NANO_MOVE_TO_IDLE.wav", 0.65f);
                MoveToIdlePlayed = true;
            }

            if (!IdleToMovePlayed)
            {
                Audio.PlaySound(this.ID,"../Assets/Audio/Character SFX/NANO_IDLE_TO_MOVE.wav", 0.65f);
                IdleToMovePlayed = true;
            }

            if (!CheckpointPlayed)
            {
                Vec3 pos = cp1Transform.Translation;
                Audio.PlaySpatial(this.ID, "../Assets/Audio/Environment SFX/CHECKPOINT_ACTIVATION.wav", pos, 1f);
                CheckpointPlayed = true;
            }

            bool pausePressed = Input.IsKeyPressed(KeyCode.Escape) || Input.IsGamepadButtonDown(ButtonCode.GamepadButtonStart);

            if (!gamePaused && !bossBGMPlayed)
            {
                //Pausing the game
                Audio.PlaySound(this.ID, BGMpath, 0.6f, true);
            }
            else
            {
                Audio.StopSound(BGMpath);
            }

        }
        public void ResetScriptedMoveAudio()
        {
            scriptedMoveAudioPlayed = false;
        }
        public void ChangeState(int newState, bool force = false)
        {
            if (currentState == newState && !force) return;

            previousState = currentState;
            currentState = newState;
            HandleStateChange();
        }
        public void SetGravity(bool state) => body.GravityScale = state ? initialGravity : 0f;
        private void HandleStateChange()
        {
            switch (currentState) //Switched to switch case for more efficiency 
            {
                case STATE_IDLE: //STATE_IDLE
                    if (tmpAnim.startFrame != 0)
                    {
                        tmpAnim.currentFrame = 0;
                        tmpAnim.isLooping = true;
                    }
                    tmpAnim.startFrame = 0; tmpAnim.endFrame = 4;
                    anim.data = tmpAnim;

                    if (previousState == 1)
                    {
                        MoveToIdlePlayed = false;
                        Audio.StopClip(this.ID,"../Assets/Audio/Character SFX/NANO_MOVE.wav");
                        isMoveSoundPlaying = false; // Update flag
                    }
                    break;

                case STATE_MOVING: //STATE_MOVING
                    if (tmpAnim.startFrame != 14)
                    {
                        tmpAnim.currentFrame = 14;
                        tmpAnim.isLooping = true;
                    }
                    tmpAnim.startFrame = 14; tmpAnim.endFrame = 20;
                    anim.data = tmpAnim;
                    IdleToMovePlayed = false;
                    Audio.PlaySound(this.ID, "../Assets/Audio/Character SFX/NANO_MOVE.wav", 0.1f, true); // true for loop
                    isMoveSoundPlaying = true; // Update flag

                    break;

                case STATE_JUMPING: //STATE_JUMPING
                    if (tmpAnim.startFrame != 21)
                    {
                        tmpAnim.currentFrame = 21;
                        Audio.StopClip(this.ID, "../Assets/Audio/Character SFX/NANO_MOVE.wav");
                        isMoveSoundPlaying = false;
                    }
                    tmpAnim.startFrame = 21; tmpAnim.endFrame = 29;
                    tmpAnim.playOnce = true;
                    tmpAnim.isLooping = false;
                    anim.data = tmpAnim;
                    JumpPlayed = false;

                    Audio.StopClip(this.ID,"../Assets/Audio/Character SFX/NANO_MOVE.wav");
                    break;

                case STATE_CHARGING: //STATE_CHARGING
                    if (tmpAnim.startFrame != 45)
                    {
                        tmpAnim.currentFrame = 45;
                    }
                    tmpAnim.startFrame = 45; tmpAnim.endFrame = 48;
                    tmpAnim.isLooping = true;
                    anim.data = tmpAnim;
                    break;

                case STATE_LANDING: //STATE_LANDING
                    // Play landing sound
                    if (CBCheck)
                    {
                        //Console.WriteLine("[DEBUG] Landed on ChargedBlock. Playing CB landing sound.");
                        Audio.PlaySound(this.ID,"../Assets/Audio/Character SFX/NANO_LAND_CHARGEDBLOCK.wav", 1f);
                    }
                    else
                    {
                        //Console.WriteLine("[DEBUG] Landed on normal ground. Playing normal landing sound.");
                        Audio.PlaySound(this.ID,"../Assets/Audio/Character SFX/NANO_LAND_GROUND.wav", 1f);
                    }

                    // Delay transition back to IDLE
                    ChangeState(STATE_IDLE);
                    break;
            }
        }
        public void ChangeBatteryLevel(float amount)
        {
            currentBattery += amount; //Change battery value 
            if (currentBattery > maxBattery) currentBattery = maxBattery; //Clamps the battery if it goes above max 
            else if (currentBattery <= 0) PlayerDeath(); //Dies if battery reaches 0

            batteryBar.UpdateBattery(); //Updates battery
        }
        public void PlayChargingVFX()
        {
            chargingVFX.IsActive = true;
        }
        public void StopChargingVFX()
        {
            chargingVFX.IsActive = false;
        }
        public void TeleportPlayer(Transform transform)
        {
            if (transform != null)
            {
                this.transform.Translation = new Vec3(transform.Translation.x, transform.Translation.y, this.transform.Translation.z);
                CheckpointPlayed = true;
                Logger.Log($"Teleporting player to {transform.Translation.x}, {transform.Translation.y}", LogLevel.DEBUG);
            }
        }
        public void SetMovement(bool state) => canMove = state;
        public void PlayerDeath()
        {
            if(bossArenaManager.arenaDeath) bossArenaManager.ArenaDeath();

            //Teleport back to spawn/first checkpoint
            cpManager?.LastCheckpoint();
            ChangeBatteryLevel(maxBattery);
            Audio.PlaySound(this.ID,"../Assets/Audio/Character SFX/Death.wav", 0.1f);
            platformCrushing = false;

            //Reset Zoom
            camera.ResetOffset();
        }
        private float MoveTowards(float current, float target, float maxDelta)
        {
            if (Math.Abs(target - current) <= maxDelta)
            {
                return target;
            }
            return current + Math.Sign(target - current) * maxDelta;
        }
        protected override void OnCollisionEnter(Collision collision)
        {
            if (collision.collider == null || collision.collider.Entity == null)
            { return; }

            if (collision.contactPoint.point.y < body.Position.y)
            {
                onGround = true;

                if (wasAirborne)
                {
                    ChargedBlock cb = collision.collider.Entity.As<ChargedBlock>();
                    CBCheck = cb != null;

                    ChangeState(STATE_LANDING);
                    wasAirborne = false;
                }
            }

            //if(collision.contactPoint.penetration >= penetrationDeath)
            //{
            //    PlayerDeath();
            //}
        }

        //Scripted Movement
        public void SetScriptedMovement(Vec2 movement)
        {
            scriptedMovement = movement;
        }
        public void ScriptedJump()
        {
            // Only allow a jump if the player is on the ground.
            if (onGround)
            {
                // Apply an impulse force upward.
                body.AddImpulseForce(new Vec2(0, 1), 50000);

                // Spawn any jump visual effects.
                jumpVFX?.SpawnVFX(transform);

                // Set the player as airborne.
                onGround = false;
                wasAirborne = true;
                LandPlayed = true;

                // Force a state change to jumping.
                ChangeState(STATE_JUMPING, true);
            }
        }
    }
}