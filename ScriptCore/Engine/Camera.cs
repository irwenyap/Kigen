/*********************************************************************
 * \file	Camera.cs
 * \brief	Defines a Camera class that links the Camera Engine into C#.
 *
 * \author	Tan Yong Chin, t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	07 March 2025

Copyright(C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ScriptCore
{
    public class Camera : Component
    {
        public void SetZoom(float zoom)
        {
            InternalCalls.CameraSystem_SetZoom(Entity.ID, zoom);
        }

        public void SetActive()
        {
            InternalCalls.CameraSystem_SetActive(Entity.ID);
        }

        public void Disable()
        {
            InternalCalls.CameraSystem_Disable(Entity.ID);
        }

        public void SetMain()
        {
            InternalCalls.CameraSystem_SetMain(Entity.ID);
        }

        public void SetBloom(float bloom)
        {
            InternalCalls.CameraSystem_SetBloom(bloom);
        }

    }
}
