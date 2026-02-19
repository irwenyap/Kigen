using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class CameraScript : Entity
    {
        public float yOffset = 2, xOffset = 0;
        private Vec2 offset, initialOffset;
        public float followSpeed = 0.1f;
        public float cameraShakeX, cameraShakeY;
        private float cameraShakeXInitial, cameraShakeYInitial;
        private float cameraShakeDuration;

        private Vec2 cameraShakeMin, cameraShakeMax;
        private bool cameraShaking;
        private float cameraShakeTimer;

        private float cameraZoomDuration;
        private float currentZoom;
        private float zoomAmount;
        private float targetZoom;
        public float initialCameraZoom = 1f;
        private bool cameraZooming;
        private float cameraZoomTimer;

        public Camera camera;
        private Transform target;
        private Transform playerTransform;
        private Transform transform;
        protected override void OnInit()
        {
            #region Components
            playerTransform = target = FindEntityByName("Player")?.GetComponent<Transform>();

            transform = GetComponent<Transform>();
            camera = GetComponent<Camera>();
            #endregion

            initialOffset = new Vec2(xOffset, yOffset);
            cameraShakeMin = new Vec2(-cameraShakeX, -cameraShakeY);
            cameraShakeMax = new Vec2(cameraShakeX, cameraShakeY);
            currentZoom = initialCameraZoom;

            cameraShakeXInitial = cameraShakeX;
            cameraShakeYInitial = cameraShakeY;

            camera.SetBloom(6);
        }

        protected override void OnUpdate(float dt)
        {
            if (cameraShaking)
            {
                Vec2 currentPos = new Vec2(CalculatePosition().x, CalculatePosition().y);
                Vec2 randomVec = Vec2.GetRandom(currentPos + cameraShakeMin, currentPos + cameraShakeMax);
                transform.Translation = new Vec3(randomVec.x, randomVec.y, transform.Translation.z);

                cameraShakeTimer += dt;
                if (cameraShakeTimer >= cameraShakeDuration) cameraShaking = false;
            }
            else
            {
                if (target != null)
                {
                    transform.Translation = Lerp(transform.Translation,
                        CalculatePosition(), followSpeed * dt);
                }
            }

            if (cameraZooming)
            {
                cameraZoomTimer += dt;
                zoomAmount = Lerp(currentZoom, targetZoom, cameraZoomTimer / cameraZoomDuration);
                camera.SetZoom(zoomAmount);
                if (cameraZoomTimer >= cameraZoomDuration)
                {
                    cameraZooming = false;
                    currentZoom = targetZoom;
                }
            }
        }
        public void CameraShake(float duration)
        {
            if (cameraShaking) return;
            cameraShakeTimer = 0f;
            cameraShaking = true;
            cameraShakeDuration = duration;
        }
        public void ChangeCameraShake(float magnitude)
        {
            cameraShakeX = magnitude;
            cameraShakeY = magnitude;
        }
        public void ResetCameraShake()
        {
            cameraShakeX = cameraShakeXInitial;
            cameraShakeY = cameraShakeYInitial;
        }
        public void CameraZoom(float duration)
        {
            if (cameraZooming) return;
            cameraZoomTimer = 0f;
            cameraZoomDuration = duration;
            cameraZooming = true;
        }
        private Vec3 CalculatePosition()
        {
            //Calculates the position the camera should be at after all transformations
            return new Vec3(target.Translation.x + initialOffset.x + offset.x,
                    target.Translation.y + initialOffset.y + offset.y,
                    target.Translation.z);
        }
        public void ChangeTarget(Transform target) => this.target = target;
        public void ResetTarget() => target = playerTransform;
        public void ChangeOffset(Vec2 offset) => this.offset = offset;
        public void ResetOffset() => offset = new Vec2(0f);
        public void ChangeZoom(float zoom, float duration)
        {
            if(zoom == 0 || duration == 0) return;
            targetZoom = zoom;
            CameraZoom(duration);
        }
        public void ResetZoom(float duration)
        {
            if(duration == 0) return;
            targetZoom = initialCameraZoom;
            CameraZoom(duration);
        }

    }
}
