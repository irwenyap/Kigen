//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using ScriptCore;

//namespace SandBox
//{
//    public class MainMenu_Start : Entity
//    {

//        Entity tmpQuitBtn;
//        Entity tmpStartBtn;

//        private UInt32 id;

//        private float timer;
//        private float loadSceneDelay = 4f;
//        private bool startTimer;

//        private Entity fadeOutScreen;
//        private Animation fadeOutScreenAnimation;

//        protected override void OnInit()
//        {
//            //IsActive = true;
//            Audio.PlaySound(this.ID, "../Assets/Audio/BGM/MainMenu_Audio_Final.wav", 0.5f);
//            fadeOutScreen = FindEntityByName("Fade Out Screen");
//            fadeOutScreen.IsActive = false;
//            fadeOutScreenAnimation = fadeOutScreen?.GetComponent<Animation>();
//        }
//        protected override void OnUpdate(float dt)
//        {
//            //if (Input.GetMouseDown(0))
//            //{
//            //    id = Renderer.GetClickedEntity();

//            //    if (id == ID)
//            //    {
//            //        Input.ChangeScenePath("../Assets/Scenes/NANO_Level1.scene");
//            //    }
//            //    else if (tmpQuitBtn?.ID == id)
//            //    {
//            //        Input.QuitGame();
//            //    }
//            //}
//            if(startTimer)
//            {
//                timer += dt;
//                if (timer >= loadSceneDelay)
//                {
//                    startTimer = false;
//                    Input.ChangeScenePath("../Assets/Scenes/NANO_Level1.scene");
//                }
//            }

//            // Gamepad A or START button to start
//            if (Input.IsKeyPressed(KeyCode.Enter) || Input.IsGamepadButtonPressed(ButtonCode.GamepadButtonA) || Input.IsGamepadButtonPressed(ButtonCode.GamepadButtonStart))
//            {
//                IsActive = false;
//                Audio.StopClip(this.ID, "../Assets/Audio/BGM/MainMenu_Audio_Final.wav");

//                Audio.PlaySound(this.ID, "../Assets/Audio/UI SFX/MAIN MENU_START.wav", 1f);
//                startTimer = true;
//                timer = 0f;
//                fadeOutScreen.IsActive = true;
//                fadeOutScreenAnimation.PlayAnimation(true,true,true,false);
//                //id = Renderer.GetClickedEntity();

//                //if (id == ID)
//                //{
//                //}
//            }


//        }
//    }
//}
