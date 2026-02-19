//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using ScriptCore;

//namespace SandBox
//{
//    public class PauseMenu_Resume : Entity
//    {
//        private bool pauseMenuActive;
//        private bool H2PBackButtonActive;

//        Entity tmpQuitBtn;
//        Entity tmpH2PBtn;
//        Entity tmpOptionBtn;
//        Entity pauseMenu;
//        Entity tmpH2PBackBtn;

//        private Renderer myRenderer;
//        private Renderer pauseMenuRenderer;

//        protected override void OnInit()
//        {
//            tmpQuitBtn = FindEntityByName("PauseMenuQuit");
//            tmpH2PBtn = FindEntityByName("PauseMenuH2P");
//            tmpOptionBtn = FindEntityByName("PauseMenuOption");
//            pauseMenu = FindEntityByName("PauseMenu");
//            tmpH2PBackBtn = FindEntityByName("H2PBack");

//            myRenderer = GetComponent<Renderer>();
            
//            if(pauseMenu != null)
//            {
//                pauseMenuRenderer = pauseMenu.GetComponent<Renderer>();
//            }

//            SetStartState();
//        }
//        protected override void OnUpdate(float dt)
//        {
//            if(Input.IsKeyPressed(KeyCode.Escape))
//            {
//                pauseMenuActive = !pauseMenuActive;

//                //Toggle Pause Menu
//                if(pauseMenu != null)
//                {
//                    if (tmpH2PBackBtn.IsActive)
//                    {
//                        //Set texture back to Pause Menu and turn off everything else
//                        pauseMenuRenderer?.SetTextureToEntity("19381a269d6-9133605c9898783d-a050581c49fbcde6");
//                        pauseMenuActive = false;
                        
//                        myRenderer?.SetVisibility(pauseMenuActive);
//                        pauseMenu.IsActive = pauseMenuActive;
//                        tmpQuitBtn.IsActive = pauseMenuActive;
//                        tmpH2PBtn.IsActive = pauseMenuActive;
//                        tmpOptionBtn.IsActive = pauseMenuActive;
//                        tmpH2PBackBtn.IsActive = pauseMenuActive;
//                    }

//                    myRenderer?.SetVisibility(pauseMenuActive);
//                    pauseMenu.IsActive = pauseMenuActive;
//                    tmpQuitBtn.IsActive = pauseMenuActive;
//                    tmpH2PBtn.IsActive = pauseMenuActive;
//                    tmpOptionBtn.IsActive = pauseMenuActive;
//                    tmpH2PBackBtn.IsActive = false;
//                }
//            }

//            if(Input.GetMouseDown(0))
//            {
//                UInt32 id = Renderer.GetClickedEntity();

//                if (id == ID)
//                {
//                    //Untoggle Pause Menu
//                    if (pauseMenu != null)
//                    {
//                        pauseMenuActive = false;

//                        myRenderer?.SetVisibility(pauseMenuActive);
//                        pauseMenu.IsActive = pauseMenuActive;
//                        tmpQuitBtn.IsActive = pauseMenuActive;
//                        tmpH2PBtn.IsActive = pauseMenuActive;
//                        tmpOptionBtn.IsActive = pauseMenuActive;
//                        tmpH2PBackBtn.IsActive = pauseMenuActive;
//                    }
//                }
//                else if (id == tmpQuitBtn.ID)
//                {
//                    //Quit Game
//                    Input.QuitGame();
//                }
//                else if (id == tmpH2PBtn.ID)
//                {
//                    pauseMenuRenderer?.SetTextureToEntity("19381fde1c6-66ddaf89d36da90f-635b5e50588d340d");
//                    tmpH2PBackBtn.IsActive = true;

//                    pauseMenuActive = false;
//                    myRenderer?.SetVisibility(pauseMenuActive);
//                    tmpQuitBtn.IsActive = pauseMenuActive;
//                    tmpH2PBtn.IsActive = pauseMenuActive;
//                    tmpOptionBtn.IsActive = pauseMenuActive;
//                }
//                else if (id == tmpOptionBtn.ID)
//                {
//                    //Nothing for now
//                }
//                else if (id == tmpH2PBackBtn.ID)
//                {
//                    pauseMenuRenderer?.SetTextureToEntity("19381a269d6-9133605c9898783d-a050581c49fbcde6");
//                    tmpH2PBackBtn.IsActive = false;

//                    pauseMenuActive = true;
//                    myRenderer?.SetVisibility(pauseMenuActive);
//                    pauseMenu.IsActive = pauseMenuActive;
//                    tmpQuitBtn.IsActive = pauseMenuActive;
//                    tmpH2PBtn.IsActive = pauseMenuActive;
//                    tmpOptionBtn.IsActive = pauseMenuActive;
//                }
//            }
//        }

//        private void SetStartState()
//        {
//            H2PBackButtonActive = false;
//            pauseMenuActive = false;

//            pauseMenuRenderer?.SetTextureToEntity("19381a269d6-9133605c9898783d-a050581c49fbcde6");

//            myRenderer?.SetVisibility(pauseMenuActive);
//            pauseMenu.IsActive = pauseMenuActive;
//            tmpQuitBtn.IsActive = pauseMenuActive;
//            tmpH2PBtn.IsActive = pauseMenuActive;
//            tmpOptionBtn.IsActive = pauseMenuActive;
//            tmpH2PBackBtn.IsActive = pauseMenuActive;

//        }
//    }
//}
