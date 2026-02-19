using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class ButtonManager : Entity
    {
        private Dictionary<string, Renderer> renderers = new Dictionary<string, Renderer>();

        private Entity startGameButton;
        private string startGameDefaultId = "1964a1e9160-b4a87c2b95bbae5f-cb1a95d9b30444bc";
        private string startGameHoverId = "1964a1e9157-55c0ada27747158f-a378a3e6cf01732a";

        private Entity optionsButton;
        private string optionsDefaultId = "1964a24409e-530e6fe31effb106-e6c156cb1261c4af";
        private string optionsHoverId = "1964a244097-6c9709ac93b3c07f-551dfd4f526fe5d1";


        private Entity creditsButton;
        private string creditsDefaultId = "1964a16ef09-330a4be0297ca40c-dcc8125665ee8d22";
        private string creditsHoverId = "1964a16ef00-6bc0c441de735f38-da2b666fbf589914";


        private Entity quitGameButton;
        private string quitGameDefaultId = "1964a244091-8e0646045f4ea2b0-f1233ec748ad7db1";
        private string quitGameHoverId = "1964a244088-1b8b73301f74e27f-b0a72b0b89be9e47";

        private UInt32 currentButton;
        private UInt32 lastButton;

        private Entity optionsMenu;
        private Entity h2pMenu;
        private Entity audioMenu;
        private Entity creditsMenu;
        private Entity credits;
        private Entity nanoAnim;

        private Entity h2pButton;
        private string h2pDefaultId = "196074ba185-cf0fe91a01deb1f6-a037b65f1db661b9";
        private string h2pHoverId = "196074ba19d-9baebe61e0224de7-874f7117ba60736f";

        private Entity audioButton;
        private string audioDefaultId = "196074ba19a-e80ebb1cc6d391b7-6da660b9cc59d161";
        private string audioHoverId = "196074ba196-c52413afe8cbaf11-4b6f88b9c5dd77f7";

        private Entity returnButton;
        private string returnDefaultId = "195910b0ea1-a8bc7efbf34d55a8-18d4f74498d3bac3";
        private string returnHoverId = "195910b0ea5-8aa972fab02b7296-41158445654b8cad";

        private Entity volumeSlider;
        private UI volumeSliderUI;
        private float volumeMin = 0.265f;
        private float volumeMax = 0.565f;
        public float volumeAmount = 100f;

        private Entity fadeOutScreen;
        private Animation fadeOutScreenAnimation;

        private float timer;
        private float loadSceneDelay = 4f;
        private bool startTimer;

        private bool wasMousePressed = false;
        private bool isMouseReleased = false;

        //Audio
        private string hoverAudio = "../Assets/Audio/UI SFX/UI_HOVER.wav";
        private string clickAudio = "../Assets/Audio/UI SFX/UI_BUTTON_PRESS.wav";

        protected override void OnInit()
        {
            startGameButton = FindEntityByName("Start Game Button");
            optionsButton = FindEntityByName("Options Button");
            creditsButton = FindEntityByName("Credits Button");
            quitGameButton = FindEntityByName("Quit Game Button");

            renderers.Add("Start Game", startGameButton?.GetComponent<Renderer>());
            renderers.Add("Options", optionsButton?.GetComponent<Renderer>());
            renderers.Add("Credits", creditsButton?.GetComponent<Renderer>());
            renderers.Add("Quit Game", quitGameButton?.GetComponent<Renderer>());

            Audio.PlaySound(ID, "../Assets/Audio/BGM/MainMenu_Audio_Final.wav", 0.5f);
            fadeOutScreen = FindEntityByName("Fade Out Screen");
            fadeOutScreen.IsActive = false;
            fadeOutScreenAnimation = fadeOutScreen?.GetComponent<Animation>();

            optionsMenu = FindEntityByName("Options Menu");
            creditsMenu = FindEntityByName("Credits Menu");
            credits = FindEntityByName("Credits");

            nanoAnim = FindEntityByName("NANO Menu Animation");

            h2pMenu = FindEntityByName("H2P Menu");
            h2pButton = FindEntityByName("H2P Button");
            renderers.Add("H2P", h2pButton?.GetComponent<Renderer>());

            audioMenu = FindEntityByName("Audio Menu");
            audioButton = FindEntityByName("Audio Button");
            renderers.Add("Audio", audioButton?.GetComponent<Renderer>());

            volumeSlider = FindEntityByName("Volume Slider");
            volumeSliderUI = volumeSlider?.GetComponent<UI>();
            volumeSliderUI?.SetPosition(new Vec3(volumeMax, 0.518f, 1f));

            returnButton = FindEntityByName("Return Button");
            renderers.Add("Return", returnButton?.GetComponent<Renderer>());

            ResetButtons();

            SetOptionsMenuActive(false);
            SetCreditsMenuActive(false);

            Input.UnhideCursor();
        }
        protected override void OnUpdate(float dt)
        {
            if(optionsMenu.IsActive && Input.IsKeyPressed(KeyCode.Escape)) SetOptionsMenuActive(false);
            if(creditsMenu.IsActive && Input.IsKeyPressed(KeyCode.Escape)) SetCreditsMenuActive(false);

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
            // Detect onReleaseHold
            if (isMouseReleased && !isCurrentlyPressed) //Mouse released
            {
                isMouseReleased = false;
            }

            // Store current key state for next frame
            wasMousePressed = isCurrentlyPressed;

            if (Input.GetMouseDown(0))
            {
                DoButtonFunction();
                //Audio.PlaySound(ID, clickAudio, 1f);
            }

            if (startTimer)
            {
                timer += dt;
                if (timer >= loadSceneDelay)
                {
                    startTimer = false;
                    Input.ChangeScenePath("../Assets/Scenes/NANO_Level1.scene");
                }
            }
        }
        private void SetOptionsMenuActive(bool state)
        {
            optionsMenu.IsActive = state;
            audioButton.IsActive = state;
            h2pButton.IsActive = state;
            returnButton.IsActive = state;

            nanoAnim.IsActive = state;

            renderers["Audio"].SetTextureToEntity(audioDefaultId);
            renderers["H2P"].SetTextureToEntity(h2pDefaultId);

            SetH2PMenuActive(state);
            SetAudioMenuActive(false);
        }
        private void SetAudioMenuActive(bool state)
        {
            Audio.PlaySound(this.ID, "../Assets/Audio/UI SFX/UI_BUTTON_PRESS.wav", 0.5f, false);
            audioMenu.IsActive = state;
            volumeSlider.IsActive = state;
            if (state) renderers["Audio"].SetTextureToEntity(audioHoverId);
            else renderers["Audio"].SetTextureToEntity(audioDefaultId);
        }
        private void SetH2PMenuActive(bool state)
        {
            Audio.PlaySound(this.ID, "../Assets/Audio/UI SFX/UI_BUTTON_PRESS.wav", 0.5f, false);
            h2pMenu.IsActive = state;
            if (state) renderers["H2P"].SetTextureToEntity(h2pHoverId);
            else renderers["H2P"].SetTextureToEntity(h2pDefaultId);
        }
        private void SetCreditsMenuActive(bool state)
        {
            Audio.PlaySound(this.ID, "../Assets/Audio/UI SFX/UI_BUTTON_PRESS.wav", 0.5f, false);
            creditsMenu.IsActive = state;
            credits.IsActive = state;
            returnButton.IsActive = state;
            nanoAnim.IsActive= state;
        }

        private void HoverFunctions()
        {
            UInt32 hoveredEntity = Renderer.GetClickedEntity(FrameBufferCode.OBJ_PICKING_UI);
            if (hoveredEntity == startGameButton.ID || hoveredEntity == optionsButton.ID ||
                hoveredEntity == creditsButton.ID || hoveredEntity == quitGameButton.ID ||
                hoveredEntity == audioButton.ID || hoveredEntity == h2pButton.ID || 
                hoveredEntity == volumeSlider.ID || hoveredEntity == returnButton.ID)
            {
                currentButton = hoveredEntity;

                //if (lastButton != hoveredEntity) Audio.PlaySound(ID, hoverAudio, 0.5f);

                lastButton = hoveredEntity;
            }

            ResetButtons();

            if (currentButton == startGameButton.ID) renderers["Start Game"]?.SetTextureToEntity(startGameHoverId);
            else if (currentButton == creditsButton.ID) renderers["Credits"]?.SetTextureToEntity(creditsHoverId);
            else if (currentButton == optionsButton.ID) renderers["Options"]?.SetTextureToEntity(optionsHoverId);
            else if (currentButton == quitGameButton.ID) renderers["Quit Game"]?.SetTextureToEntity(quitGameHoverId);
            else if (currentButton == audioButton.ID) renderers["Audio"]?.SetTextureToEntity(audioHoverId);
            else if (currentButton == h2pButton.ID) renderers["H2P"]?.SetTextureToEntity(h2pHoverId);
            else if (currentButton == returnButton.ID) renderers["Return"]?.SetTextureToEntity(returnHoverId);
        }
        private void DoButtonFunction()
        {
            if (currentButton == startGameButton.ID)
            {
                Audio.StopClip(ID, "../Assets/Audio/BGM/MainMenu_Audio_Final.wav");

                Audio.PlaySound(ID, "../Assets/Audio/UI SFX/MAIN MENU_START.wav", 1f);
                startTimer = true;
                timer = 0f;
                fadeOutScreen.IsActive = true;
                fadeOutScreenAnimation.PlayAnimation(true, true, true, false);
            }
            else if (currentButton == creditsButton.ID)
            {
                SetCreditsMenuActive(true);
            }
            else if (currentButton == optionsButton.ID)
            {
                SetOptionsMenuActive(true);
            }
            else if (currentButton == quitGameButton.ID)
            {
                Input.QuitGame();
            }
            else if (currentButton == audioButton.ID)
            {
                SetAudioMenuActive(true);
                SetH2PMenuActive(false);
            }
            else if (currentButton == h2pButton.ID)
            {
                SetH2PMenuActive(true);
                SetAudioMenuActive(false);
            }
            else if (currentButton == returnButton.ID)
            {
                if (optionsMenu.IsActive) SetOptionsMenuActive(false);
                else SetCreditsMenuActive(false);

            }
        }
        private void ResetButtons()
        {
            renderers["Start Game"]?.SetTextureToEntity(startGameDefaultId);
            renderers["Options"]?.SetTextureToEntity(optionsDefaultId);
            renderers["Credits"]?.SetTextureToEntity(creditsDefaultId);
            renderers["Quit Game"]?.SetTextureToEntity(quitGameDefaultId);
            renderers["Return"]?.SetTextureToEntity(returnDefaultId);
            if (!h2pMenu.IsActive) renderers["H2P"]?.SetTextureToEntity(h2pDefaultId);
            if (!audioMenu.IsActive) renderers["Audio"]?.SetTextureToEntity(audioDefaultId);
        }
    }
}
