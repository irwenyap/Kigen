/*********************************************************************
 * \file	ButtonCode.cs
 * \brief	Define button codes in the C# Script Engine.
 *
 * \author	woonli.wong, 2301308
 * \email	woonli.wong@digipen.edu
 * \date	1 April 2025

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
    public enum ButtonCode
    {
        // From glfw3.h

        /* Gamepad */
        GamepadButtonA = 0,
        GamepadButtonB = 1,
        GamepadButtonX = 2,
        GamepadButtonY = 3,
        GamepadLeftBumper = 4,
        GamepadRightBumper = 5,
        GamepadButtonBack = 6,
        GamepadButtonStart = 7,
        GamepadButtonGuide = 8,
        GamepadButtonLeftStick = 9,
        GamepadButtonRightStick = 10,
        GamepadDpadUp = 11,
        GamepadDpadRight = 12,
        GamepadDpadDown = 13,
        GamepadDpadLeft = 14
    }
}
