using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ScriptCore;

namespace SandBox
{
    public class ChargedBlock : Block
    {
        public bool drainingPlayer = false;

        public Dictionary<Block, bool> connections = new Dictionary<Block, bool>();
        public int unchargedBlockSet, unchargedBlockCount;
        public float drainRate;
        UnchargedBlock UCB; //Reference to Uncharged Block 1
        Entity tmpUCB;

        //Reference to Player
        Entity tmpPlayer;
        PlayerNew player;

        private Transform transform;
        private float timer;
        protected override void OnInit()
        {
            //Uncharged Blocks
            connections?.Add(this, false);

            //Console.WriteLine("Checking if " + unchargedBlockSet + " is a valid set");
            if (unchargedBlockSet > 0)
            {
                //Console.WriteLine("Valid set found");
                for (int i = 1; i <= unchargedBlockCount; i++)
                {
                    tmpUCB = FindEntityByName($"UCB{unchargedBlockSet}_{i}");
                    connections?.Add(tmpUCB?.As<UnchargedBlock>(), false);
                    tmpUCB?.As<UnchargedBlock>().SetConnections(this);
                    //Console.WriteLine("Found ucb" + i);
                }
                //Console.WriteLine("Looked for Set " + unchargedBlockSet + " Connections found: " + connections?.Count);
            }
            //else if (unchargedBlockSet == 0) Console.WriteLine("0 detected no set to look for");
            //else Console.WriteLine("Set not found");

            //Player
            tmpPlayer = FindEntityByName("Player");
            player = tmpPlayer?.As<PlayerNew>();

            transform = GetComponent<Transform>();
        }

        protected override void OnUpdate(float dt)
        {
            if (drainingPlayer)
            {
                player?.ChangeBatteryLevel(-drainRate * dt);
            }
        }
        public void PlayerOnBlock(Block block, bool state)
        {
            if(block != null)
            {
                if(connections != null) connections[block] = state;
            }

            CheckContacts();
        }
        public void CheckContacts()
        {
            bool activate = false;

            foreach (KeyValuePair<Block, bool> kvp in connections)
            {
                if (kvp.Value)
                {
                    activate = true;
                }
            }

            foreach (KeyValuePair <Block, bool> kvp in connections)
            {
                if (kvp.Key is UnchargedBlock)
                {
                    UnchargedBlock ucb = kvp.Key as UnchargedBlock;
                    ucb?.Activate(activate);
                }
            }

            drainingPlayer = activate;
        }

        protected override void OnCollisionEnter(Collision collision)
        {
            if(collision.collider != null)
            {
                if(player != null)
                {
                    if (collision.collider.Entity.ID == player.ID)
                    {
                        //if (!drainingPlayer) Audio.PlaySound(this.ID, "../Assets/Audio/Block SFX/CB_ACTIVATE.wav", 0.4f);
                        PlayerOnBlock(this, true);
                    }
                }
            }
        }

        //protected override void OnCollisionStay(Collision collision)
        //{
        //    if (collision.collider != null)
        //    {
        //        if (player != null)
        //        {
        //            if (!Audio.ClipIsPlaying("../Assets/Audio/Block SFX/CB Loop.wav"))
        //            {
        //                Audio.PlaySound(this.ID, "../Assets/Audio/Block SFX/CB Loop.wav", 1f, true);
        //            }
        //            else return;
        //        }
        //    }
        //}
        protected override void OnCollisionExit(Collision collision)
        {
            if (collision.collider != null)
            {
                if (player != null)
                {
                    if (collision.collider.Entity.ID == player.ID)
                    {
                        PlayerOnBlock(this, false);
                        //Audio.StopSound("../Assets/Audio/Block SFX/CB Loop.wav");
                        //Audio.PlaySound(this.ID, "../Assets/Audio/Character SFX/NANO_LAND_CHARGEDBLOCK.wav", 1f);
                    }
                }
            }
        }
    }
}