using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class Dialogue
    {
        public string dialogueLink;
        public bool missionControlSpeaking;
        public float dialogueDuration;
        public Subtitle[] dialogueSubtitles;
        public Dialogue(string dialogueLink, bool missionControlSpeaking, float dialogueDuration, Subtitle[] dialogueSubtitles)
        {
            this.dialogueLink = dialogueLink;
            this.missionControlSpeaking = missionControlSpeaking;
            this.dialogueDuration = dialogueDuration;
            this.dialogueSubtitles = dialogueSubtitles;
        }
    }
}
