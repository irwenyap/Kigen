using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class FPS : Entity
    {
        public UI ui;
        bool displayFPS = false;
        protected override void OnInit()
        {
            ui = GetComponent<UI>();
            ui.SetFont("19362e7adb5-f5d82ff0869c4802-662bdabeda82c652");
            ui.SetVisibility(false);
        }

        protected override void OnUpdate(float dt)
        {
            if (Input.IsKeyPressed(KeyCode.F1))
            {
                displayFPS = !displayFPS;
            }

            if (displayFPS)
            {
                Logger.Log(((int)Input.GetFPS()).ToString(), LogLevel.WARN);
                ui.SetText("FPS: " + (int)Input.GetFPS());
            }
            else
            {
                ui.SetVisibility(false);
            }
        }
    }
}
