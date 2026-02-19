/*********************************************************************
 * \file	Render.cs
 * \brief	Defines a Render class that links the Render Engine into C#.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	08 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
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
    public class Renderer : Component
    {

        public void SetVisibility(bool b)
        {
            InternalCalls.RenderSystem_SetVisibility(Entity.ID, b);
        }

        public static void SetDebugMode(bool b)
        {
            InternalCalls.RenderSystem_SetDebugMode(b);
        }

        public void SetTextureToEntity(string texID)
        {
            InternalCalls.RenderSystem_SetTextureToEntity(Entity.ID, texID);
        }

        static public UInt32 GetClickedEntity(FrameBufferCode fbo = FrameBufferCode.OBJ_PICKING_ENGINE)
        {
            return InternalCalls.RenderSystem_GetClickedEntity(fbo);
        }


    }
}
