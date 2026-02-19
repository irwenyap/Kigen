using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class CameraChangeTrigger : Entity
    {
        public float xOffset, yOffset;
        public float zoom;
        public uint lockTargetID;
        public float cameraZoomDuration;
        private PlayerNew player;
        private CameraScript camera;
        protected override void OnInit()
        {
            player = FindEntityByName("Player")?.As<PlayerNew>();
            camera = FindEntityByName("Main Camera")?.As<CameraScript>();
        }
        protected override void OnTriggerEnter(AABBCollider2D collider)
        {
            if (collider != null)
            {
                if (collider.Entity.ID == player?.ID)
                {
                    if (lockTargetID != 0) camera?.ChangeTarget(FindEntityByID(lockTargetID)?.GetComponent<Transform>());
                    camera?.ChangeOffset(new Vec2(xOffset, yOffset));
                    camera?.ChangeZoom(zoom, cameraZoomDuration == 0 ? 1f : cameraZoomDuration);
                }
            }
        }
        protected override void OnTriggerExit(AABBCollider2D collider)
        {
            if (collider != null)
            {
                if (collider.Entity.ID == player?.ID)
                {
                    Logger.Log("Leaving collider", LogLevel.INFO);
                    camera?.ResetTarget();
                    camera?.ResetOffset();
                    camera?.ResetZoom(cameraZoomDuration);
                }
            }
        }
    }
}
