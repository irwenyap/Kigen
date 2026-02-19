using System;
using System.Data.SqlClient;
using System.Diagnostics;
using ScriptCore;

namespace SandBox
{
    public class CCTVTracking : Entity
    {
        //Components
        Entity player;
        private Transform playerTransform;
        private Transform transform;
        public Renderer renderer;

        public bool TrackVertical;
        public bool CCTV2;

        protected override void OnInit()
        {
            //Player
            player = FindEntityByName("Player");

            playerTransform = player?.GetComponent<Transform>();

            //CCTV
            renderer = GetComponent<Renderer>();
            transform = GetComponent<Transform>();

            if (!TrackVertical)
            {
                if (!CCTV2)
                {
                    UpdateSpriteHorizontal(0);
                }
                else
                {
                    UpdateSprite2Horizontal(0);
                }

            }
            else
            {
                if (!CCTV2)
                {
                    UpdateSpriteVertical(0);
                }
                else
                {
                    UpdateSprite2Vertical(0);
                }
            }
        }

        protected override void OnUpdate(float dt)
        {
            float distanceX = playerTransform.Translation.x - transform.Translation.x;
            float distanceY = playerTransform.Translation.y - transform.Translation.y;

            if (distanceX < -1200 || distanceX > 1200) return;
            if (distanceY < -1200 || distanceY > 1200) return;

            if (!TrackVertical)
            {
                if(!CCTV2)
                {
                    UpdateSpriteHorizontal(distanceX);
                }
                else
                {
                    UpdateSprite2Horizontal(distanceX);
                }

            }
            else
            {
                if (!CCTV2)
                {
                    if (distanceX > 1)
                    {
                        UpdateSpriteVertical(distanceY);
                    }
                    else return;
                }
                else
                {
                    if (distanceX < 1)
                    {
                        UpdateSprite2Vertical(distanceY);
                    }
                    else return;
                }
            }
        }

        private void UpdateSpriteHorizontal(float distance)
        {
            if (distance < -300)
            { renderer?.SetTextureToEntity("19553238f24-4b29b1ee92d6a54c-42d535f44960cc0a"); } //L5
            else if (distance < -200)
            { renderer?.SetTextureToEntity("19553238f20-767d6e5e8318162-21006cee36c35f1d"); } //L4
            else if (distance < -150)
            { renderer?.SetTextureToEntity("19553238f1b-6a3c4ffdbf424084-259602bcf002ee38"); } //L3
            else if (distance < -100)
            { renderer?.SetTextureToEntity("19553238f16-386e5157bb086557-c2045bca931bee6b"); } //L2
            else if (distance < -50)
            { renderer?.SetTextureToEntity("19553238f10-24880836c3a996bd-ca756ae364915a9e"); } //L1
            else if (distance >= -50 && distance <= 50)
            { renderer?.SetTextureToEntity("19553238f08-4357be6927c25d0d-1e19c35195ff6054"); } //Center
            else if (distance > 50 && distance < 100)
            { renderer?.SetTextureToEntity("19553238f29-8b1a9a94b953d15a-6e58526aa9f25b38"); } //R1
            else if (distance > 100 && distance < 150)
            { renderer?.SetTextureToEntity("19553238f2e-2bab35adf894a0e3-8680f6ebe600271"); } //R2
            else if (distance > 150 && distance < 200)
            { renderer?.SetTextureToEntity("19553238f34-ce8aa1e96d3e47d9-1337ba911e80c79e"); } //R3
            else if (distance > 200 && distance < 300)
            { renderer?.SetTextureToEntity("19553238f39-ebd4a2db38e0e358-c47654757bc78dd7"); } //R4
            else
            { renderer?.SetTextureToEntity("19553238f3e-118ddea4d3bdf8d9-10d91e878fd56ea4"); } //R5
        }

        private void UpdateSprite2Horizontal(float distance)
        {
            if (distance < -300)
            { renderer?.SetTextureToEntity("19553238f61-1bc3a7274e8e29c9-53e6fe948b279130"); } //L5
            else if (distance < -200)
            { renderer?.SetTextureToEntity("19553238f5c-2c69bc90894078ef-f36c537713bf1ac3"); } //L4
            else if (distance < -150)
            { renderer?.SetTextureToEntity("19553238f56-a4e02b9784f4743b-73d12da0c9a8d982"); } //L3
            else if (distance < -100)
            { renderer?.SetTextureToEntity("19553238f50-56b12c5fc9dcd626-bb9084966a3dc3d5"); } //L2
            else if (distance < -50)
            { renderer?.SetTextureToEntity("19553238f4a-8147db1656da7d87-75c2c9aa57f7609c"); } //L1
            else if (distance >= -50 && distance <= 50)
            { renderer?.SetTextureToEntity("19553238f45-ce5f2ab28c79f49a-bf2c2e95641d0ad6"); } //Center
            else if (distance > 50 && distance < 100)
            { renderer?.SetTextureToEntity("19553238f66-a92f5da267334f07-3e28fdf8422a9cea"); } //R1
            else if (distance > 100 && distance < 150)
            { renderer?.SetTextureToEntity("19553238f6c-747407ba758e3f17-745dd0f6d5803a37"); } //R2
            else if (distance > 150 && distance < 200)
            { renderer?.SetTextureToEntity("19553238f71-8719304f6cee6d0-c0c09b08e98e1b04"); } //R3
            else if (distance > 200 && distance < 300)
            { renderer?.SetTextureToEntity("19553238f76-a7e7ba543458ff76-b84f8bf01818aed1"); } //R4
            else
            { renderer?.SetTextureToEntity("19553238f7b-3436500139613734-1c4d32e2755acd7e"); } //R5
        }

        private void UpdateSpriteVertical(float distance)
        {
            if (distance > 200)
            { renderer?.SetTextureToEntity("195531788f0-35fc2c094e99da08-4bcc0153657545b9"); } //T4
            else if (distance > 150)
            { renderer?.SetTextureToEntity("195531788ec-3090e0f9a4d5ec3e-a7c810674c63fccc"); } //T3
            else if (distance > 100)
            { renderer?.SetTextureToEntity("195531788e8-93efe60182d20e7e-46e06549d49eb47f"); } //T2
            else if (distance > 50)
            { renderer?.SetTextureToEntity("195531788e4-7166a84b5815d0f0-4f517462a61420b2"); } //T1
            else if (distance <= 50 && distance >= -50)
            { renderer?.SetTextureToEntity("195531788df-42a2c698abd0137-322a87c3f0454d08"); } //Center
            else if (distance < -50 && distance > -100)
            { renderer?.SetTextureToEntity("195531788c6-89377b9bf784773b-f08a6015065369cc"); } //B1
            else if (distance < -100 && distance > -150)
            { renderer?.SetTextureToEntity("195531788cf-3e6715f22c6ff2dc-34161d1e02c16f05"); } //B2
            else if (distance < -150 && distance > -200)
            { renderer?.SetTextureToEntity("195531788d5-b9d7c38a2cfe1d40-9813c41060038db2"); } //B3
            else
            { renderer?.SetTextureToEntity("195531788da-2e7669e31715b486-16d5e9e6a961de33"); } //B4
        }

        private void UpdateSprite2Vertical(float distance)
        {
            if (distance > 200)
            { renderer?.SetTextureToEntity("19553238fa7-f1dcf28f0f364b78-8cd664d8e16d5"); } //T4
            else if (distance > 150)
            { renderer?.SetTextureToEntity("19553238fa2-77e710dc5d34fe93-985f47646e77e430"); } //T3
            else if (distance > 100)
            { renderer?.SetTextureToEntity("19553238f9d-9576bc3a8eb4e3ba-37e49c46f70f6dc3"); } //T2
            else if (distance > 50)
            { renderer?.SetTextureToEntity("19553238f97-23a5f53e69039ed1-15c7aa53c7146616"); } //T1
            else if (distance <= 50 && distance >= -50)
            { renderer?.SetTextureToEntity("19553238f92-7c270c65ae04cd44-bc0a5d176a377014"); } //Center
            else if (distance < -50 && distance > -100)
            { renderer?.SetTextureToEntity("19553238f80-90e3fce05888f84a-7c59793ce9d29508"); } //B1
            else if (distance < -100 && distance > -150)
            { renderer?.SetTextureToEntity("19553238f85-e6709b1e4d225f47-68f83858fef5ebc1"); } //B2
            else if (distance < -150 && distance > 200)
            { renderer?.SetTextureToEntity("19553238f89-3e20f75405b5e26e-2138e1635e61016e"); } //B3
            else
            { renderer?.SetTextureToEntity("19553238f8e-556c7a93e270efa2-d3517b47bc616b67"); } //B4
        }
    }
}
