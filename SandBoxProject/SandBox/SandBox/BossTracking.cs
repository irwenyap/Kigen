using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class BossTracking : Entity
    {
        private Entity player;
        private Transform playerTransform;

        private Transform transform;
        private Renderer renderer;

        private int lastDistanceGroup = 0;

        protected override void OnInit()
        {
            player = FindEntityByName("Player");
            playerTransform = player?.GetComponent<Transform>();

            renderer = GetComponent<Renderer>();
            transform = GetComponent<Transform>();

            renderer?.SetTextureToEntity("195bd7176d3-ad02d2bd99ded142-a801443dd4da1c4b"); //Set Idle State

            float initialDistanceX = Math.Abs(playerTransform.Translation.x - transform.Translation.x);
            lastDistanceGroup = (int)(initialDistanceX / 129);
        }

        protected override void OnUpdate(float dt)
        {
            float distanceX = playerTransform.Translation.x - transform.Translation.x;

            if (Input.IsKeyDown(KeyCode.G))
            {
                Logger.Log($"{distanceX}", LogLevel.DEBUG);
            }

            //Calculate the current group
            int currentGroup = (int)(Math.Abs(distanceX) / 129f);

            if(currentGroup != lastDistanceGroup)
            {
                //Audio.PlaySound("../Assets/Audio/Character SFX/NANO_LAND_CHARGEDBLOCK.wav", 0.5f);
                lastDistanceGroup = currentGroup;
            }

            UpdateSprite(distanceX);
        }

        private void UpdateSprite(float distance)
        {
            if (distance < -1253.5) renderer?.SetTextureToEntity("195bd71772f-4345bd840a8de775-bf8840b6c8a6e1c"); //Left F110
            else if (distance < -1171) renderer?.SetTextureToEntity("195bd71772b-f2a19bfd684b8453-21e182e2d2ab228a"); //Left F109
            else if (distance < -1106.5) renderer?.SetTextureToEntity("195bd717726-e146a8dad34e1786-cb9dcc00db59d"); //Left F108
            else if (distance < -1042) renderer?.SetTextureToEntity("195bd717722-1567369a2dc7901e-b361b6dd8ea21ee0"); //Left F107
            else if (distance < -977.5) renderer?.SetTextureToEntity("195bd71771e-a4887dd4d4423730-fdae0dd331624173"); //Left F106
            else if (distance < -913) renderer?.SetTextureToEntity("195bd71771a-46167bb314807233-db911be0016739c6"); //Left F105
            else if (distance < -848.5) renderer?.SetTextureToEntity("195bd717716-84bb9044a49eb687-77ff74eda48039f9"); //Left F104
            else if (distance < -784) renderer?.SetTextureToEntity("195bd717712-65adaa88ec2a1cbf-80dd84067652780c"); //Left F103
            else if (distance < -719.5) renderer?.SetTextureToEntity("195bd71770e-829ca4da50992509-1ff5d8e8fe8d2fbf"); //Left F102
            else if (distance < -655) renderer?.SetTextureToEntity("195bd71770a-b089c5cf7fb2db36-2866e801d0029bf2"); //Left F101
            else if (distance < -590.5) renderer?.SetTextureToEntity("195bd717705-144643f8d39505-1aed410a8ac01f45"); //Left F100
            else if (distance < -526) renderer?.SetTextureToEntity("195bd717701-6cbd974c6f65d735-b7a9568d61f5f1c0"); //Left F099
            else if (distance < -461.5) renderer?.SetTextureToEntity("195bd7176fd-4e8c9e45d873c5a9-1f5ad8304b61453"); //Left F098
            else if (distance < -397) renderer?.SetTextureToEntity("195bd7176f8-3e92e603e7be1109-28d31e678b20acea"); //Left F097
            else if (distance < -332.5) renderer?.SetTextureToEntity("195bd7176f4-bf0b754995f22d1b-5d82555c9082e1fd"); //Left F096
            else if (distance < -268) renderer?.SetTextureToEntity("195bd7176ef-210b76ca244e4182-ab6abb7832842b04"); //Left F095
            else if (distance < -203.5) renderer?.SetTextureToEntity("195bd7176ea-f4d522720bebffc0-5f07f1661e764a37"); //Left F094
            else if (distance < -139) renderer?.SetTextureToEntity("195bd7176e6-f25f013b87588325-6f75351be50dd7e"); //Left F093
            else if (distance < -74.5) renderer?.SetTextureToEntity("195bd7176e1-c205bb992b1784f0-a2f9ac5f610ebed1"); //Left F092
            else if (distance < -10) renderer?.SetTextureToEntity("195bd7176dc-2b53254ef9d7c990-8e8ef5b4c9f6498"); //Left F091
            else if (distance >= -10 && distance <= 10) renderer?.SetTextureToEntity("195bd7176d3-ad02d2bd99ded142-a801443dd4da1c4b"); //Center F090
            else if (distance > 10 && distance <= 74.5) renderer?.SetTextureToEntity("195bd79bd60-9ffbbe108a38217b-78b90c2e0092f7be"); //Right F051
            else if (distance > 74.5 && distance <= 139) renderer?.SetTextureToEntity("195bd79bd65-cbb5277cf8c4e755-7047fd152f1d8b8b"); //Right F052
            else if (distance > 139 && distance <= 203.5) renderer?.SetTextureToEntity("195bd79bd69-6de8643a0c064a8b-d12fa832a6e2d3d8"); //Right F053
            else if (distance > 203.5 && distance <= 268) renderer?.SetTextureToEntity("195bd79bd6e-2a68bd41e5f4bc8e-cf440e38d2c4fc3d"); //Right F054
            else if (distance > 268 && distance <= 332.5) renderer?.SetTextureToEntity("195bd79bd72-3df17f705849dd6-f118d73ee562692a"); //Right F055
            else if (distance > 332.5 && distance <= 397) renderer?.SetTextureToEntity("195bd79bd76-f4bab9744f9ee0a7-274daa3d78b80677"); //Right F056
            else if (distance > 397 && distance <= 461.5) renderer?.SetTextureToEntity("195bd79bd7a-d7e57edbcdac896a-1d2b745474c49244"); //Right F057
            else if (distance > 461.5 && distance <= 526) renderer?.SetTextureToEntity("195bd79bd7f-f02707dab0c46529-4736c949b7378099"); //Right F058
            else if (distance > 526 && distance <= 590.5) renderer?.SetTextureToEntity("195bd79bd84-e1aaf0917aba3f32-5199746c16fb73e6"); //Right F059
            else if (distance > 590.5 && distance <= 655) renderer?.SetTextureToEntity("195bd79bd88-8b490edfd18f8cf0-edef8a7834deec86"); //Right F060
            else if (distance > 655 && distance <= 719.5) renderer?.SetTextureToEntity("195bd79bd8d-370ad4002473d0d2-8a5de385d7f7ecb9"); //Right F061
            else if (distance > 719.5 && distance <= 784) renderer?.SetTextureToEntity("195bd79bd92-b251e54c24709dc0-1c452570da1b26a0"); //Right F062
            else if (distance > 784 && distance <= 848.5) renderer?.SetTextureToEntity("195bd79bd98-2fcb071af35b7c70-ca57c6b61f51833"); //Right F063
            else if (distance > 848.5 && distance <= 913) renderer?.SetTextureToEntity("195bd79bd9d-c84545ee81239fd5-8de356951896c7b2"); //Right F064
            else if (distance > 913 && distance <= 977.5) renderer?.SetTextureToEntity("195bd79bda3-6c6a3d715db6591b-29e5afa2bb54a905"); //Right F065
            else if (distance > 977.5 && distance <= 1042) renderer?.SetTextureToEntity("195bd79bda8-55fb0a75988e4fd8-e659f299bee6a3cc"); //Right F066
            else if (distance > 1042 && distance <= 1106.5) renderer?.SetTextureToEntity("195bd79bdb1-d55fda49fa0817c4-12642665395305de"); //Right F067
            else if (distance > 1106.5 && distance <= 1171) renderer?.SetTextureToEntity("195bd79bdb1-d55fda49fa0817c4-12642665395305de"); //Right F068
            else if (distance > 1171 && distance <= -1253.5) renderer?.SetTextureToEntity("195bd79bdb5-34bede25f72de23a-b10f7b47c130ebb1"); //Right F069
            else renderer?.SetTextureToEntity("195bd79bdba-5e2e2175f3187007-fa6b1cbadfe5bdef"); // Right F070

        }
    }
}
