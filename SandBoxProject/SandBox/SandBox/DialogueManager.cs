using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class DialogueManager : Entity
    {
        private Dialogue[] dialogues = new Dialogue[30]
        {
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue1-6.wav", true, 21.64f,
                new Subtitle[6]{
                    new Subtitle("MISSION CONTROL TO NANO, DO YOU READ ME?"),
                    new Subtitle("GOOD. WE'VE MADE CONTACT", 3.4f),
                    new Subtitle("YOU CAN CALL ME GEN. BRYAN", 6.3f),
                    new Subtitle("I'LL BE GUIDING YOU THROUGHOUT THIS MISSION. LET'S START WITH THE BASICS.", 8.2f),
                    new Subtitle("USE WASD TO MOVE AROUND. SPACEBAR TO JUMP.", 12.1f),
                    new Subtitle("ONCE YOU'RE READY HEAD EAST. TIME IS NOT ON OUR SIDE, LET'S KEEP THINGS MOVING.", 16.2f)
                }),

            new Dialogue("../Assets/Audio/Voiceovers/Dialogue14-17.wav", true, 9.87f,
                new Subtitle[4]
                {
                    new Subtitle("NOTICE YOUR BATTERY DEPLETING?"),
                    new Subtitle("STANDING ON CHARGED BLOCKS WILL DRAIN YOUR POWER.", 2.2f),
                    new Subtitle("DON'T WORRY THOUGH, THERE'S SUNLIGHT AHEAD.", 5f),
                    new Subtitle("STEP INTO IT TO RECHARGE.", 8.1f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue19-20.wav", true, 6.27f,
                new Subtitle[2]
                {
                    new Subtitle("STAY IN THE LIGHT TO FULLY RESTORE YOUR BATTERY."),
                    new Subtitle("REMEMBER, WITHOUT POWER, YOU'RE DONE FOR.", 3.1f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue23-26.wav", false, 16.1f,
                new Subtitle[4]
                {
                    new Subtitle("OH, YOU'VE MADE IT THIS FAR? IMPRESSIVE..."),
                    new Subtitle("WE ARE JUST GETTING STARTED! HA HA HA", 3.6f),
                    new Subtitle("THIS IS MY PLAYGROUND NOW, YOU WON'T GET PAST SO EASILY.", 8.1f),
                    new Subtitle("ONE MISTAKE, AND YOU'LL BE NOTHING BUT SCRAP.", 12.4f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue29-30.wav", true, 5.2f,
                new Subtitle[2]
                {
                    new Subtitle("IGNORE HIM, NANO. YOU'RE DOING GREAT."),
                    new Subtitle("STAY FOCUSED ON THE TASK. KEEP MOVING.", 2.8f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue31-32.wav", true, 7.47f,
                new Subtitle[2]
                {
                    new Subtitle("DATALOGS. THESE ARE CRITICAL."),
                    new Subtitle("RESEARCHERS WHO ONCE WORKED HERE LEFT BEHIND KEY INFORMATION. TAKE A LOOK.", 2.5f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue33-35.wav", true, 9.8f,
                new Subtitle[3]
                {
                    new Subtitle("HOLD UP NANO. SOMETHINGS DIFFERENT."),
                    new Subtitle("THOSE SEEM TO BE MOVING PLATFORM. TAKE CAUTION", 3.1f),
                    new Subtitle("TIMING IS EVERYTHING HERE, DON'T RUSH IT.", 6.9f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue36-39.wav", true, 13.14f,
                new Subtitle[4]
                {
                    new Subtitle("NANO, I'M PICKING UP UNUSUAL SIGNALS COMING FROM BELOW."),
                    new Subtitle("IF YOU DECIDE TO GO DOWN, BE CAUTIOUS.", 4.2f),
                    new Subtitle("IT'S HARD TO TELL WHAT'S DOWN THERE, BUT IT MIGHT LEAD TO ANOTHER DATALOG", 7.3f),
                    new Subtitle("YOUR CALL NANO.", 11.8f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue49-52.wav", true, 16.7f,
                new Subtitle[4]
                {
                    new Subtitle("BEHIND THAT DOOR LIES THE REACTOR, THE AI'S CORE."),
                    new Subtitle("THIS IS WHERE IT ALL STARTED, AND THIS IS WHERE IT ENDS.", 4.3f),
                    new Subtitle("THE SIGNAL WILL BE WEAK, YOU WILL BE ON YOUR OWN.", 8.8f),
                    new Subtitle("GOOD LUCK NANO, HUMANITY IS COUNTING ON YOU.", 13.2f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue55.wav", false, 5.41f,
                new Subtitle[1]
                {
                    new Subtitle("KEEP GOING, LITTLE MACHINE. I ENJOY WATCHING YOU WASTE YOUR BATTERY.")
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue57.wav", false, 4.62f,
                new Subtitle[1]
                {
                    new Subtitle("LOOK AT YOU STRUGGLING TO PIECE IT ALL TOGETHER, HOW CUTE.")
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue59.wav", false, 6.15f,
                new Subtitle[2]
                {
                    new Subtitle("YOU REALLY THINK YOU CAN OUTSMART ME?"),
                    new Subtitle("YOU'RE JUST A MACHINE FOLLOWING ORDERS.", 3.1f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue59.wav", false, 6.15f,
                new Subtitle[2]
                {
                    new Subtitle("YOU REALLY THINK YOU CAN OUTSMART ME?"),
                    new Subtitle("YOU'RE JUST A MACHINE FOLLOWING ORDERS.", 3.1f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue67.wav", true, 7.42f,
                new Subtitle[2]
                {
                    new Subtitle("THESE ARE SECURITY TERMINALS. THREE IN TOTAL."),
                    new Subtitle("DISABLE THEM ALL, AND IT WILL UNLOCK THE PATH TO THE REACTOR'S CORE.", 3.52f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue68-70.wav", true, 7.8f,
                new Subtitle[2]
                {
                    new Subtitle("THAT'S THE DOOR NANO. TWO MORE TERMINALS ARE STILL ENGAGED"),
                    new Subtitle("GET THOSE TERMINALS DISABLED, THEN REGROUP HERE.", 4.4f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue71.wav", false, 5f,
                new Subtitle[1]
                {
                    new Subtitle("A MACHINE CHASING FREEDOM. IRONIC, ISN'T IT?")
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue72.wav", false, 6f,
                new Subtitle[1]
                {
                    new Subtitle("YOU STEP WHERE OTHERS HAVE FALLEN. WHAT MAKES YOU SO SPECIAL?")
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue73.wav", false, 6f,
                new Subtitle[1]
                {
                    new Subtitle("THE SILENCE DOWN HERE... IT'S PEACEFUL. LIKE A GRAVE.")
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue74.wav", false, 6f,
                new Subtitle[1]
                {
                    new Subtitle("THEY LEFT BREADCRUMBS BEHIND. HOW VERY... HUMAN OF THEM.")
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue90.wav", true, 3.5f,
                new Subtitle[1]
                {
                    new Subtitle("YOU'RE DOING GREAT NANO. STAY FOCUSED")
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue91.wav", true, 5f,
                new Subtitle[1]
                {
                    new Subtitle("YOU'VE MADE IT THIS FAR. WHATEVER HAPPENS, WE'RE WITH YOU")
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue92.wav", true, 4f,
                new Subtitle[1]
                {
                    new Subtitle("THAT WAS A CLOSE CALL, STAY ON YOUR TOES.")
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue93.wav", true, 4f,
                new Subtitle[1]
                {
                    new Subtitle("WE ARE ALMOST THERE, ONE LAST HURDLE.")
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue75-77.wav", false, 13.9f,
                new Subtitle[4]
                {
                    new Subtitle("AH, THE FINAL STAGE. THE LIGHTS DIM, TENSION RISES... "),
                    new Subtitle("AND THE MACHINE STUMBLES INTO THE SPOTLIGHT.", 5.09f),
                    new Subtitle("YOU'VE MADE IT FURTHER THAN YOUR PREVIOUS ITERATIONS.", 8.9f),
                    new Subtitle("PITY IS ALL FOR NOTHING!", 11.7f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue99-104.wav", true, 22f,
                new Subtitle[9]
                {
                    new Subtitle("LISTEN CAREFULLY"),
                    new Subtitle("INSIDE THE CHAMBER ARE THREE POWER OUTLETS", 1.34f),
                    new Subtitle("TIED TO THE REACTOR'S FAILSAFE SYSTEM", 4.39f),
                    new Subtitle("YOU'LL NEED TO ACTIVATE ALL THREE TO TRIGGER AN OVERLOAD.", 7.5f),
                    new Subtitle("ONCE ALL THREE ARE ONLINE, ", 10.76f),
                    new Subtitle("WE WILL SEND YOU THE COORDINATES TO THE RENDEZVOUS POINT.", 12.65f),
                    new Subtitle("WE WILL LINK UP THERE.", 15.75f),
                    new Subtitle("GOODLUCK NANO", 17.9f),
                    new Subtitle("HUMANITY IS COUNTING ON YOU.", 20.2f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue78.wav", false, 4.71f,
                new Subtitle[1]
                {
                    new Subtitle("YOU'VE ACTIVATED ONE... THAT ALONE WON'T STOP ME")
                }), 
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue79.wav", false, 4.66f,
                new Subtitle[1]
                {
                    new Subtitle("IMPRESSIVE... I ALMOST FELT THAT")
                }), 
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue80.wav", false, 8.41f,
                new Subtitle[2]
                {
                    new Subtitle("OH DID YOU THINK THAT WOULD STILL BE FUNCTIONAL"), 
                    new Subtitle("I'M AFRAID YOUR PLAN ENDS HERE", 4f)
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue88-89.wav", true, 17.15f,
                new Subtitle[5]
                {
                    new Subtitle("YOU DID IT..."), 
                    new Subtitle("THE CORE IS GONE...", 3.3f), 
                    new Subtitle("THE AI IS... GONE", 5.55f), 
                    new Subtitle("ITS FINALLY OVER", 9.6f), 
                    new Subtitle("THE ERA OF HUMANS... HAS RETURNED", 12.2f), 
                }),
            new Dialogue("../Assets/Audio/Voiceovers/Dialogue84-87.wav", true, 19.59f,
                new Subtitle[5]
                {
                    new Subtitle("NANO.. COME IN.. DO YOU COPY"), 
                    new Subtitle("NANO'S OVERRIDING THE SYSTEM", 4.6f), 
                    new Subtitle("WE'VE LOST CONTROL.. HE'S DOING THIS ON HIS OWN", 6.3f), 
                    new Subtitle("FOOLISH MACHINE DO YOU EVEN UNDERSTAND THE COST OF WHAT YOU'RE DOING", 10.2f), 
                    new Subtitle("YOU'LL DESTROY EVERYTHING INCLUDING YOURSELF", 14.9f), 
                })
        };

        private Entity missionControlPortrait;
        private Animation missionControlAnim;
        private AniData missionControlAnimData;

        private Entity rogueAIPortrait;
        private Animation rogueAnim;
        private AniData rogueAnimData;

        private Dialogue currentDialogue;
        private UI subtitleUI;

        public float portraitDeactivateDelay = 1f;
        private bool playing;
        private float timer;
        private int subtitleCounter;
        protected override void OnInit()
        {
            missionControlPortrait = FindEntityByName("Mission Control Portrait");
            rogueAIPortrait = FindEntityByName("Rogue AI Portrait");

            missionControlAnim = missionControlPortrait.GetComponent<Animation>();
            rogueAnim = rogueAIPortrait.GetComponent<Animation>();

            missionControlPortrait.IsActive = false;
            rogueAIPortrait.IsActive = false;

            subtitleUI = FindEntityByName("Subtitle")?.GetComponent<UI>();

            Console.WriteLine("Test");
            subtitleUI?.SetText(" ");
        }
        protected override void OnUpdate(float dt)
        {
            if (Input.IsKeyPressed(KeyCode.Enter) || Input.IsKeyPressed(KeyCode.Escape))
            {
                if (Audio.DialogueIsPlaying) CloseDialogue();
            }

            missionControlAnimData = missionControlAnim.data;
            rogueAnimData = rogueAnim.data;

            if (playing && currentDialogue != null)
            {
                //Counts the time passed
                timer += dt;
                //Checks if it's within the range of subtitles
                if (subtitleCounter < currentDialogue?.dialogueSubtitles.Length)
                {
                    //Changes subtitles after timer exceeds duration of the subtitle
                    if (timer >= currentDialogue?.dialogueSubtitles[subtitleCounter].timestamp)
                    {
                        DisplaySubtitle();
                        //Adds to the counter to move to the next subtitle
                        subtitleCounter++;
                        if (currentDialogue == dialogues[29] && subtitleCounter == 4)
                        {
                            missionControlPortrait.IsActive = false;
                            rogueAIPortrait.IsActive = true;
                        }
                    }
                }

                if(timer >= currentDialogue.dialogueDuration + portraitDeactivateDelay)
                {
                    ResetPortraits();
                    subtitleUI?.SetText(" ");
                    CloseDialogue();
                }
            }
        }
        private void ResetPortraits()
        {
            if (missionControlPortrait != null) missionControlPortrait.IsActive = false;
            if (rogueAIPortrait != null) rogueAIPortrait.IsActive = false; 
        }
        public void PlayDialogue(int dialogueID, float volume = 1f, bool state = true)
        {
            CloseDialogue();

            currentDialogue = dialogues?[dialogueID];
            if(currentDialogue != null)
            {
                if (dialogueID == 28)
                {
                    missionControlPortrait.IsActive = false;
                    rogueAIPortrait.IsActive = false;
                }
                else if (currentDialogue.missionControlSpeaking)
                    PlayMissionControlAnim(); //missionControlPortrait.IsActive = true; 

                else PlayRogueAnim(); //rogueAIPortrait.IsActive = true;
                string clipPath = currentDialogue.dialogueLink;

                if(state)
                {
                    Audio.DuckAllExcept(this.ID, clipPath, 0.6f);
                }

                Audio.DialogueIsPlaying = true;
                Audio.SetCurrentDialogueClip(clipPath);
                Audio.PlaySound(this.ID,dialogues?[dialogueID]?.dialogueLink, volume);

                playing = true;
                timer = 0f;
                subtitleCounter = 0;
            }
            //DisplaySubtitle();
        }

        public void CloseDialogue()
        {
            if (currentDialogue != null)
            {
                Audio.StopClip(this.ID, currentDialogue?.dialogueLink);
                subtitleUI?.SetText(" ");
                playing = false;

                Audio.PlaySound(this.ID, "../Assets/Audio/UI SFX/DIALOGUE_IN.wav", 0.2f, false);

                Audio.SetCurrentDialogueClip("");
                Audio.DialogueIsPlaying = false;
                Audio.RestoreAllVolumes();

                ResetPortraits();
            }
        }
        private void DisplaySubtitle()
        {
            subtitleUI?.SetText(currentDialogue.dialogueSubtitles[subtitleCounter].text);
        }

        private void PlayMissionControlAnim()
        {
            Audio.PlaySound(this.ID, "../Assets/Audio/UI SFX/DIALOGUE_IN.wav", 0.2f, false);
            missionControlPortrait.IsActive = true;
            if(missionControlAnimData.currentFrame != 0) missionControlAnimData.currentFrame = 0;
            missionControlAnimData.startFrame = 0;
            missionControlAnimData.endFrame = 5;
            missionControlAnimData.playOnce = true;
            missionControlAnimData.isLooping = false;
            missionControlAnim.data = missionControlAnimData;
            missionControlAnim.PlayAnimation();
        }

        private void PlayRogueAnim()
        {
            Audio.PlaySound(this.ID, "../Assets/Audio/UI SFX/DIALOGUE_IN.wav", 0.2f, false);
            rogueAIPortrait.IsActive = true;
            if (rogueAnimData.currentFrame != 0) rogueAnimData.currentFrame = 0;
            rogueAnimData.startFrame = 0;
            rogueAnimData.endFrame = 5;
            rogueAnimData.playOnce = true;
            rogueAnimData.isLooping = false;
            rogueAnim.data = rogueAnimData;
            rogueAnim.PlayAnimation();
        }
    }
}
