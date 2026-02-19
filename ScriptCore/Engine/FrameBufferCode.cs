/*********************************************************************
 * \file	FrameBufferCode.cs
 * \brief	Define framebuffer codes in the C# Script Engine.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	02 May 2025

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
    public enum FrameBufferCode
    {
        GAME = 0,
        OBJ_PICKING_ENGINE = 1,
        BRIGHT = 2,
        HORIBLUR = 3,
        VERTBLUR = 4,
        COMBINE = 5,
        VIGNETTE = 6,
        GLITCH = 7,
        UI = 8,
        ENGINE = 9,
        OBJ_PICKING_GAME = 10,
        GAME_FINAL = 11,
        OBJ_PICKING_UI = 12,
        MAX_FRAMEBUFFERS = 13
    }
}
