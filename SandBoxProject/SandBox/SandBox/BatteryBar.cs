using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ScriptCore;

namespace SandBox
{
    public class BatteryBar : Entity
    {
        Entity tmpPlayer;
        PlayerNew player;

        UI ui;

        protected override void OnInit()
        {
            tmpPlayer = FindEntityByName("Player");
            player = tmpPlayer?.As<PlayerNew>();
            ui = GetComponent<UI>();
            UpdateBattery();
        }

        public void UpdateBattery()
        {
            ui?.SetScale(new Vec2(player.currentBattery / 100, 1));
        }
    }
}
