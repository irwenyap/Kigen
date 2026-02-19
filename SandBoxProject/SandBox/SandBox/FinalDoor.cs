using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class FinalDoor : Entity
    {
        private string[] doorStates = new string[5]
        {
            "194cfdac1c3-1654f2a9ebff8df1-4494c6864fdb90c7", //Locked 
            "194cfdac1bb-3d4edbb263dba973-2df50b81f1c19031", //Locked 1/3
            "194cfdac1bf-cc05bb1ad03310dd-7ffd818aa6fa09ac", //Locked 2/3
            "194cfdac1c1-e904768d6ced15b8-6145d595e35fe95b", //Locked 3/3
            "19590bd8b3a-e4563dd953a7936d-16b02ee766332e89" //Unlocked
        };
        private int counter = 0;
        private Renderer renderer;
        private bool interactable;
        private PlayerNew player;
        private Dictionary<DatalogTrigger, bool> datalogs = new Dictionary<DatalogTrigger, bool>();
        public int datalogTriggerAmount;
        private DialogueManager dialogueManager;
        private Entity interactUI;
        private FinalDoorHandPrint handPrint;
        public void SetDoorState(int state)
        {
            renderer?.SetTextureToEntity(doorStates[state]);
        }
        protected override void OnInit()
        {
            renderer = GetComponent<Renderer>();
            SetDoorState(0);
            for (int i = 1; i <= datalogTriggerAmount; i++)
            {
                Entity datalog = FindEntityByName("DatalogTrigger" + i);
                datalogs?.Add(datalog?.As<DatalogTrigger>(), false);
                Console.WriteLine("Adding datalog, current number of datalogs is " + datalogs?.Count);
            }
            player = FindEntityByName("Player")?.As<PlayerNew>();
            dialogueManager = FindEntityByName("Dialogue Manager")?.As<DialogueManager>();
            interactUI = FindEntityByName("Final Door Interact UI");
            handPrint = FindEntityByName("Hand print")?.As<FinalDoorHandPrint>();
        }
        protected override void OnUpdate(float dt)
        {
            if (interactable)
            {
                // Gamepad X button to interact with door
                if(Input.IsKeyPressed(KeyCode.E) || Input.IsGamepadButtonPressed(ButtonCode.GamepadButtonX)) OpenDoor();
            }
        }
        public void CompleteDatalog()
        {
            //if (datalog != null)
            //{
            //    if (datalogs != null) datalogs[datalog] = true; //Sets the completion status of the datalog to true 
            //    counter++;
            //    Console.WriteLine("Completed datalog");
            //}

            ////Checks all the values, if any are false, returns out of the function
            //foreach (KeyValuePair<DatalogTrigger, bool> kvp in datalogs)
            //{
            //    if (!kvp.Value)
            //    {
            //        return;
            //    }
            //}
            renderer.SetTextureToEntity(doorStates[++counter]);
        }
        private void OpenDoor()
        {
            if (counter >= datalogTriggerAmount)
            {
                handPrint.IsActive = false;
                NextLevel();
            }
            else handPrint.StartBlink();
        }
        private void NextLevel()
        {
            //dialogueManager?.PlayDialogue(8);
            //renderer?.SetTextureToEntity(doorStates[datalogs.Count + 1]);
            Input.ChangeScenePath("../Assets/Scenes/NANO_Level2.scene");
        }
        protected override void OnTriggerEnter(AABBCollider2D collider)
        {
            if (collider != null)
            {
                if (collider.Entity.ID == player?.ID)
                {
                    //datalogManager?.CompleteDatalog();
                    if (player != null)
                    {
                        interactable = true;
                        interactUI.IsActive = true;
                    }
                }
            }
        }

        protected override void OnTriggerExit(AABBCollider2D collider)
        {
            if (collider != null)
            {
                if (collider.Entity.ID == player?.ID)
                {
                    if (player != null)
                    {
                        interactable = false;
                        interactUI.IsActive = false;
                    }
                }
            }
        }
    }
}
