/*********************************************************************
 * \file	UI.cs
 * \brief	Setups UI.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	27 November 2024

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
    public class UI : Component
    {
        // UISystem_SetPosition
        public void SetPosition(Vec3 pos)
        {
            InternalCalls.UISystem_SetPosition(Entity.ID, pos);
        }

        // UISystem_SetScale
        public void SetScale(Vec2 size)
        {
            InternalCalls.UISystem_SetScale(Entity.ID, size);
        }

        // UISystem_SetSize
        public void SetSize(Vec2 size)
        {
            InternalCalls.UISystem_SetSize(Entity.ID, size);
        }

        // UISystem_SetText
        public void SetText(string text)
        {
            InternalCalls.UISystem_SetText(Entity.ID, text);
        }

        // UISystem_SetFont
        public void SetFont(string font)
        {
            InternalCalls.UISystem_SetFont(Entity.ID, font);
        }

        // UISystem_SetColor
        public void SetColor(Vec3 color)
        {
            InternalCalls.UISystem_SetColor(Entity.ID, color);
        }

        // UISystem_SetVisibility
        public void SetVisibility(bool visible)
        {
            InternalCalls.UISystem_SetVisibility(Entity.ID, visible);
        }

        // UISystem_SetCenterAlign
        public void SetCenterAlignment(bool centerAlign)
        {
            InternalCalls.UISystem_SetCenterAlignment(Entity.ID, centerAlign);
        }
    }
}
