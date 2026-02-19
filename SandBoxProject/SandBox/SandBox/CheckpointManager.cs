using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class CheckpointManager : Entity
    {
        public int checkpointAmount;
        private int currentCheckpoint;
        private List<Checkpoint> checkpoints = new List<Checkpoint>();
        private List<CheckpointTransition> transitions = new List<CheckpointTransition>();
        private PlayerNew player;
        protected override void OnInit()
        {
            player = FindEntityByName("Player")?.As<PlayerNew>();
            InitializeCheckpoints();
        }
        protected override void OnUpdate(float dt)
        {
            //if (Input.IsKeyPressed(KeyCode.X)) NextCheckpoint();
            //if (Input.IsKeyPressed(KeyCode.Z)) PreviousCheckpoint();
        }
        private void NextCheckpoint()
        {
            player.TeleportPlayer(checkpoints[currentCheckpoint + 1].GetComponent<Transform>());
        }
        private void PreviousCheckpoint()
        {
            player.TeleportPlayer(checkpoints[currentCheckpoint - 1].GetComponent<Transform>());
        }
        public void LastCheckpoint()
        {
            player.TeleportPlayer(checkpoints[currentCheckpoint].GetComponent<Transform>());
        }
        public void ReachedCheckpoint(Checkpoint point)
        {
            for (int i = 0; i < checkpoints.Count; i++)
            {
                checkpoints[i]?.ResetCheckpoint();
                if (checkpoints[i] == point)
                {
                    currentCheckpoint = i;
                    transitions[i].StartTransition();
                }
            }
        }
        public void InitializeCheckpoints()
        {
            for (int i = 1; i <= checkpointAmount; i++)
            {
                Entity checkpointEntity = FindEntityByName($"Checkpoint{i}");
                Checkpoint checkpoint = checkpointEntity?.As<Checkpoint>();

                Entity checkpointTransitionEntity = FindEntityByName($"CheckpointTransition{i}");
                CheckpointTransition checkpointTransition = checkpointTransitionEntity?.As<CheckpointTransition>();

                if (checkpoint != null && checkpointTransition != null)
                {
                    checkpoint.GiveReference(this, player);
                    checkpoints.Add(checkpoint);

                    checkpointTransition.GiveReference(checkpoint);
                    transitions.Add(checkpointTransition);
                }
            }
        }
    }
}
