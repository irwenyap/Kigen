using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class PauseMenu : Entity
    {
        private Dictionary<string, Renderer> renderers = new Dictionary<string, Renderer>();

        private Entity batteryBar, batteryBarCharger, batteryBarVFX;

        private Entity resumeButton;
        private string resumeDefaultId = "195910b0eac-402a0506d9887c80-125ba7a1b5546dce";
        private string resumeHoverId = "195910b0eb2-68a1e4241e2fd1c7-c74cc0874e6cd68";

        private Entity quitButton;
        private string quitDefaultId = "195910b0ea8-5f77c3ef57ca2d44-cae4c7b73c1f4db2";
        private string quitHoverId = "195910b0eaf-34ee8ec670690d5c-6a46f2a96bfad3dc";
        
        private Entity mainMenuButton;
        private string mainMenuDefaultId = "196077a1239-c9102c8d4e136f26-b8c0a3131f58ff9b";
        private string mainMenuHoverId = "196077a123d-b5242f1f573c1944-291e9453bc63f925";

        private Entity mainMenuPopup;

        private Entity popup;

        private Entity popupConfirm;
        private string confirmDefaultId = "19608f34e6a-751a8b1bbda5e457-a47bb8e467f290ff";
        private string confirmHoverId = "19608f34e74-da1440a86845ae55-8fd49c7007cbeaa1";

        private Entity popupReturn;
        private string returnDefaultId = "19608f34e71-d018a1286e26df1-54da9d1ed1a90849";
        private string returnHoverId = "19608f34e6d-6e57f6915310fafd-7c817f5f24bbcfbf";

        private Entity h2pButton;
        private string h2pDefaultId = "196074ba185-cf0fe91a01deb1f6-a037b65f1db661b9"; 
        private string h2pHoverId = "196074ba19d-9baebe61e0224de7-874f7117ba60736f";

        private Entity audioButton;
        private string audioDefaultId = "196074ba19a-e80ebb1cc6d391b7-6da660b9cc59d161";
        private string audioHoverId = "196074ba196-c52413afe8cbaf11-4b6f88b9c5dd77f7";

        private PlayerNew player;
        private string hoverAudio = "../Assets/Audio/UI SFX/UI_HOVER.wav";
        private string clickAudio = "../Assets/Audio/UI SFX/UI_BUTTON_PRESS.wav";
        private string menuTransitionAudio = "../Assets/Audio/UI SFX/PAUSE MENU_TRANSITION.wav";
        private UInt32 previousButton;

        private Entity h2pMenu;
        private Entity audioMenu;

        private UInt32 currentButton;
        private float leftStickHoldDuration = 0.0f;
        private bool leftStickWasHeld = false;

        private Entity volumeSlider;
        private UI volumeSliderUI;
        private float volumeMin = 0.265f;
        private float volumeMax = 0.565f;
        public float volumeAmount = 100f;

        private bool wasMousePressed = false;
        private bool isMouseReleased = false;

        private bool quitGame;

        private Entity nanoAnim;

        protected override void OnInit()
        {
            batteryBar = FindEntityByName("Battery Bar");
            batteryBarCharger = FindEntityByName("Battery Bar Charger");
            batteryBarVFX = FindEntityByName("BatteryBarVFX");

            nanoAnim = FindEntityByName("NANO Menu Animation");

            resumeButton = FindEntityByName("Resume Button");
            quitButton = FindEntityByName("Quit Button");
            mainMenuButton = FindEntityByName("Main Menu Button");
            player = FindEntityByName("Player")?.As<PlayerNew>();
            volumeSlider = FindEntityByName("Volume Slider");
            volumeSliderUI = volumeSlider?.GetComponent<UI>();
            volumeSliderUI?.SetPosition(new Vec3(volumeMax, 0.518f ,1f));

            h2pMenu = FindEntityByName("H2P Menu");
            h2pButton = FindEntityByName("H2P Button");
            renderers.Add("H2P", h2pButton?.GetComponent<Renderer>()); 

            audioMenu = FindEntityByName("Audio Menu");
            audioButton = FindEntityByName("Audio Button");
            renderers.Add("Audio", audioButton?.GetComponent<Renderer>());

            popup = FindEntityByName("Popup");
            mainMenuPopup = FindEntityByName("Main Menu Popup");
            popupConfirm = FindEntityByName("Popup Confirm");
            popupReturn = FindEntityByName("Popup Cancel");

            renderers.Add("Resume", resumeButton?.GetComponent<Renderer>());
            renderers.Add("Quit", quitButton?.GetComponent<Renderer>());
            renderers.Add("Main Menu", mainMenuButton?.GetComponent<Renderer>());
            renderers.Add("Confirm", popupConfirm?.GetComponent<Renderer>());
            renderers.Add("Return", popupReturn?.GetComponent<Renderer>());

            IsActive = false;
            resumeButton.IsActive = false;
            quitButton.IsActive = false;
            mainMenuButton.IsActive = false;
            popup.IsActive = false;
            mainMenuPopup.IsActive = false;
            popupConfirm.IsActive = false;
            popupReturn.IsActive = false;
            volumeSlider.IsActive = false;
            nanoAnim.IsActive = false;

            SetH2PMenuActive(false);
            SetAudioMenuActive(false);
            h2pButton.IsActive = false;
            audioButton.IsActive = false;

            ResetButtons();
            Input.HideCursor();
        }
        protected override void OnUpdate(float dt)
        {
            if (IsActive || popup.IsActive || mainMenuPopup.IsActive)
            {
                if (Input.GetGamepadLeftStick().x > 0.5f || Input.GetGamepadLeftStick().x < -0.5f ||
                    Input.GetGamepadLeftStick().y > 0.5f || Input.GetGamepadLeftStick().y < -0.5f)
                {
                    leftStickWasHeld = true;
                    // Prevent spamming while holding the left stick
                    leftStickHoldDuration += dt;
                }
                else
                {
                    leftStickHoldDuration = 0.5f;
                    leftStickWasHeld = false;
                }
            }
            else currentButton = UInt32.MaxValue;

            if (Input.IsKeyPressed(KeyCode.Escape) || Input.IsGamepadButtonPressed(ButtonCode.GamepadButtonStart)) // Gamepad start button to toggle pause
            {
                if (popup.IsActive || mainMenuPopup.IsActive)
                {
                    SetConfirmMenuActive(false);
                    
                }
                else if (IsActive)
                {
                    Input.HideCursor();
                    TogglePauseMenu();
                }
                else
                {
                    Input.UnhideCursor();
                    TogglePauseMenu();
                }
                Audio.PlaySound(this.ID, menuTransitionAudio, 1f);
            }

            // Gamepad A button to select currently hovered button
            if ((IsActive || popup.IsActive || mainMenuPopup.IsActive) && (Input.GetMouseDown(0) || Input.IsGamepadButtonPressed(ButtonCode.GamepadButtonA)))
            {
                ButtonFunctions(currentButton);
                Audio.PlaySound(this.ID, clickAudio, 1f);
            }
            
            // Gamepad B button to go out of volume slider
            if (currentButton == volumeSlider.ID && Input.IsGamepadButtonPressed(ButtonCode.GamepadButtonB))
            {
                currentButton = audioButton.ID;
                Audio.PlaySound(this.ID, clickAudio, 1f);
            }

            //Logger.Log($"before hoverfunctions leftStickHoldDuration: {leftStickHoldDuration}", LogLevel.DEBUG);
            HoverFunctions();

            bool isCurrentlyPressed = Input.GetMouseHold(0);

            // Detect onRelease
            if (wasMousePressed && !isCurrentlyPressed)
            {
                isMouseReleased = true;
            }
            else if (isCurrentlyPressed) //Mouse hold
            {
                if (currentButton == volumeSlider.ID)
                {
                    volumeAmount = (Clamp(Input.GetMouseX() / Input.GetWindowWidth(), volumeMin, volumeMax) - volumeMin) / (volumeMax - volumeMin);
                    volumeSliderUI?.SetPosition(new Vec3(Clamp(Input.GetMouseX() / Input.GetWindowWidth(), volumeMin, volumeMax), 0.518f, 1f));
                    Audio.MasterVol = volumeAmount;
                }
                isMouseReleased = false; // Reset release state when key is pressed again
            }
            else if (currentButton == volumeSlider.ID && (Input.GetGamepadLeftStick().x < -0.5f || Input.GetGamepadLeftStick().x > 0.5f))
            {
                if (leftStickHoldDuration >= 0.5f)
                {
                    volumeAmount += (Input.GetGamepadLeftStick().x * 0.05f);
                    volumeAmount = Clamp(volumeAmount, volumeMin, volumeMax);
                    volumeSliderUI?.SetPosition(new Vec3(volumeAmount, 0.518f, 1f));
                    Audio.MasterVol = (volumeAmount - volumeMin) / (volumeMax - volumeMin);
                }
            }

            // Detect onReleaseHold
            if (isMouseReleased && !isCurrentlyPressed) //Mouse released
            {
                isMouseReleased = false;
            }
            //Logger.Log(volumeAmount.ToString(), LogLevel.INFO);
            // Store current key state for next frame
            wasMousePressed = isCurrentlyPressed;

            if (leftStickHoldDuration >= 0.5f && leftStickWasHeld)
            {
                leftStickHoldDuration = 0;
            }
        }
        private void TogglePauseMenu()
        {
            IsActive = !IsActive;
            player.SetMovement(!IsActive && !popup.IsActive && !mainMenuPopup.IsActive);
            player.BGMPlayed = IsActive;
            player.gamePaused = IsActive;

            resumeButton.IsActive = IsActive;
            quitButton.IsActive = IsActive;
            mainMenuButton.IsActive = IsActive;
            SetH2PMenuActive(IsActive);

            if (!IsActive) SetAudioMenuActive(false);

            h2pButton.IsActive = IsActive;
            audioButton.IsActive = IsActive;

            batteryBar.IsActive = !IsActive;
            batteryBarCharger.IsActive = !IsActive;
            batteryBarVFX.IsActive = !IsActive;

            nanoAnim.IsActive = IsActive;

            // Set the resume button to hovered by default
            if (Input.IsGamepadConnected() && IsActive)
            {
                currentButton = resumeButton.ID;
            }
        }
        private void TogglePauseMenuPopUp()
        {
            IsActive = !IsActive;
            player.SetMovement(!IsActive && !popup.IsActive && !mainMenuPopup.IsActive);

            resumeButton.IsActive = IsActive;
            quitButton.IsActive = IsActive;
            mainMenuButton.IsActive = IsActive;
            SetH2PMenuActive(IsActive);

            if (!IsActive) SetAudioMenuActive(false);

            h2pButton.IsActive = IsActive;
            audioButton.IsActive = IsActive;

            batteryBar.IsActive = !IsActive;
            batteryBarCharger.IsActive = !IsActive;
            batteryBarVFX.IsActive = !IsActive;

            nanoAnim.IsActive = IsActive;

            // Set the resume button to hovered by default
            if (Input.IsGamepadConnected() && IsActive)
            {
                currentButton = resumeButton.ID;
            }
        }
        private void SetConfirmMenuActive(bool state)
        {
            if (quitGame) popup.IsActive = state;
            else mainMenuPopup.IsActive = state;
            player.SetMovement(!IsActive && !popup.IsActive && !mainMenuPopup.IsActive);

            popupConfirm.IsActive = state;
            popupReturn.IsActive = state;

            // Set the return button to hovered by default
            if (Input.IsGamepadConnected() && (popup.IsActive || mainMenuPopup.IsActive))
            {
                currentButton = popupReturn.ID;
            }
        }
        private void SetAudioMenuActive(bool state)
        {
            audioMenu.IsActive = state;
            volumeSlider.IsActive = state;
            renderers["Audio"].SetTextureToEntity(audioHoverId);

            if (state)
            {
                currentButton = volumeSlider.ID;
            }
        }
        private void SetH2PMenuActive(bool state)
        {
            h2pMenu.IsActive = state;
            renderers["H2P"].SetTextureToEntity(h2pHoverId);
        }
        private void ButtonFunctions(UInt32 buttonID)
        {
            if (resumeButton.ID == buttonID)
            {
                TogglePauseMenu();
                
            }
            else if (quitButton.ID == buttonID)
            {
                quitGame = true;
                TogglePauseMenuPopUp();
                SetConfirmMenuActive(true);
            }
            else if (mainMenuButton.ID == buttonID)
            {
                quitGame = false;
                TogglePauseMenuPopUp(); 
                SetConfirmMenuActive(true);
            }
            else if (popupConfirm.ID == buttonID)
            {
                if (quitGame) Input.QuitGame();
                else Input.ChangeScenePath("../Assets/Scenes/Main Menu.scene");
            }
            else if (popupReturn.ID == buttonID)
            {
                TogglePauseMenu();
                SetConfirmMenuActive(false);
            }
            else if (h2pButton.ID == buttonID)
            {
                SetH2PMenuActive(true);
                SetAudioMenuActive(false);
            }
            else if (audioButton.ID == buttonID)
            {
                SetAudioMenuActive(true);
                SetH2PMenuActive(false);
            }
        }
        private void HoverFunctions()
        {
            // Gamepad left stick to change the hover target
            if (Input.GetGamepadLeftStick().x < -0.5f)
            {
                //Logger.Log($"leftStickHoldDuration: {leftStickHoldDuration}", LogLevel.DEBUG);

                if (leftStickHoldDuration >= 0.5f)
                {
                    // Switch between resume, main menu and quit game buttons
                    if (currentButton == resumeButton.ID) currentButton = quitButton.ID;
                    else if (currentButton == quitButton.ID) currentButton = mainMenuButton.ID;
                    else if (currentButton == mainMenuButton.ID) currentButton = resumeButton.ID;

                    // Switch between popup confirm and popup cancel buttons
                    else if (currentButton == popupConfirm.ID) currentButton = popupReturn.ID;
                    else if (currentButton == popupReturn.ID) currentButton = popupConfirm.ID;
                }
            }
            else if (Input.GetGamepadLeftStick().x > 0.5f)
            {
                //Logger.Log($"leftStickHoldDuration: {leftStickHoldDuration}", LogLevel.DEBUG);

                if (leftStickHoldDuration >= 0.5f)
                {
                    // Switch between resume, main menu and quit game buttons
                    if (currentButton == resumeButton.ID) currentButton = mainMenuButton.ID;
                    else if (currentButton == quitButton.ID) currentButton = resumeButton.ID;
                    else if (currentButton == mainMenuButton.ID) currentButton = quitButton.ID;

                    // Switch between popup confirm and popup cancel buttons
                    else if (currentButton == popupConfirm.ID) currentButton = popupReturn.ID;
                    else if (currentButton == popupReturn.ID) currentButton = popupConfirm.ID;
                }
            }
            else if (Input.GetGamepadLeftStick().y < -0.5f)
            {
                //Logger.Log($"leftStickHoldDuration: {leftStickHoldDuration}", LogLevel.DEBUG);

                if (leftStickHoldDuration >= 0.5f)
                {
                    // Switch between resume/main menu/quit, audio and how to play buttons
                    if (currentButton == resumeButton.ID || currentButton == mainMenuButton.ID | currentButton == quitButton.ID)
                        currentButton = audioButton.ID;
                    else if (currentButton == audioButton.ID) currentButton = h2pButton.ID;
                    else if (currentButton == h2pButton.ID) currentButton = resumeButton.ID;
                }
            }
            else if (Input.GetGamepadLeftStick().y > 0.5f)
            {
                //Logger.Log($"leftStickHoldDuration: {leftStickHoldDuration}", LogLevel.DEBUG);

                if (leftStickHoldDuration >= 0.5f)
                {
                    // Switch between resume/main menu/quit, audio and how to play buttons
                    if (currentButton == resumeButton.ID || currentButton == mainMenuButton.ID | currentButton == quitButton.ID)
                        currentButton = h2pButton.ID;
                    else if (currentButton == audioButton.ID) currentButton = resumeButton.ID;
                    else if (currentButton == h2pButton.ID) currentButton = audioButton.ID;
                }
            }

            // Mouse hover takes precedence over gamepad controls.
            UInt32 hoveredEntity = Renderer.GetClickedEntity(FrameBufferCode.OBJ_PICKING_UI);
            if (hoveredEntity == resumeButton.ID || hoveredEntity == quitButton.ID ||
                hoveredEntity == popupConfirm.ID || hoveredEntity == popupReturn.ID || 
                hoveredEntity == volumeSlider.ID || hoveredEntity == h2pButton.ID || 
                hoveredEntity == audioButton.ID || hoveredEntity == mainMenuButton.ID)
            {
                currentButton = hoveredEntity;
            }

            //if (currentButton != previousButton) Audio.PlaySound(hoverAudio, 1f);

            //previousButton = currentButton;

            ResetButtons();

            if (currentButton == resumeButton.ID) renderers["Resume"].SetTextureToEntity(resumeHoverId);
            else if (currentButton == quitButton.ID) renderers["Quit"].SetTextureToEntity(quitHoverId);
            else if (currentButton == mainMenuButton.ID) renderers["Main Menu"].SetTextureToEntity(mainMenuHoverId);
            else if (currentButton == popupConfirm.ID) renderers["Confirm"].SetTextureToEntity(confirmHoverId);
            else if (currentButton == popupReturn.ID) renderers["Return"].SetTextureToEntity(returnHoverId);
            else if (currentButton == h2pButton.ID) renderers["H2P"].SetTextureToEntity(h2pHoverId);
            else if (currentButton == audioButton.ID) renderers["Audio"].SetTextureToEntity(audioHoverId);
        }
        private void ResetButtons()
        {
            renderers["Resume"].SetTextureToEntity(resumeDefaultId);
            renderers["Quit"].SetTextureToEntity(quitDefaultId);
            renderers["Main Menu"].SetTextureToEntity(mainMenuDefaultId);
            renderers["Confirm"].SetTextureToEntity(confirmDefaultId);
            renderers["Return"].SetTextureToEntity(returnDefaultId);
            if (!h2pMenu.IsActive) renderers["H2P"].SetTextureToEntity(h2pDefaultId);
            if (!audioMenu.IsActive || currentButton == volumeSlider.ID) renderers["Audio"].SetTextureToEntity(audioDefaultId);
        }
    }
}
