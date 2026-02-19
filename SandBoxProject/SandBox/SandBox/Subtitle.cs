using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class Subtitle
    {
        public string text;
        public float timestamp; 

        public Subtitle(string text = "", float timestamp = 0)
        {
            this.text = text;
            this.timestamp = timestamp;
        }
    }
}
