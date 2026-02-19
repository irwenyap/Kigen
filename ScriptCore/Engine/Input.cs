/*********************************************************************
 * \file	Input.cs
 * \brief	Defines a Input class that setup the input system for the Scripting Engine.
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
    public class Input
    {

        public static bool IsKeyDown(KeyCode key) 
        {
            return InternalCalls.Input_IsKeyDown(key);
        }

        public static bool IsKeyPressed(KeyCode key)
        {
            return InternalCalls.Input_IsKeyPressed(key);
        }

        public static void ChangeScenePath(string path)
        {
            InternalCalls.ChangeScene(path);
        }

        public static void QuitGame() {
            InternalCalls.QuitGame();
        }

        public static void HideCursor()
        {
            InternalCalls.HideCursor();
        }

        public static void UnhideCursor()
        {
            InternalCalls.UnhideCursor();
        }

        public static int GetWindowWidth()
        {
            return InternalCalls.GetWindowWidth();
        }

        public static int GetWindowHeight()
        {
            return InternalCalls.GetWindowHeight();
        }

        public static double GetFPS()
        {
            return InternalCalls.GetFPS();
        }

        public static bool GetMouseDown(int button)
        {
            return InternalCalls.Input_GetMouseDown(button);
        }

        public static bool GetMouseHold(int button)
        {
            return InternalCalls.Input_GetMouseHold(button);
        }

        public static float GetMouseX()
        {
            float x;
            InternalCalls.Input_GetMouseX(out x);
            return x;
        }

        public static float GetMouseY()
        {
            float y;
            InternalCalls.Input_GetMouseY(out y);
            return y;
        }

        public static Vec2 GetGamepadLeftStick()
        {
            InternalCalls.Input_GetGamepadLeftStick(out Vec2 direction);
            return direction;
        }

        public static bool IsGamepadButtonDown(ButtonCode button)
        {
            return InternalCalls.Input_IsGamepadButtonDown(button);
        }

        public static bool IsGamepadButtonPressed(ButtonCode button)
        {
            return InternalCalls.Input_IsGamepadButtonPressed(button);
        }

        public static bool IsGamepadConnected()
        {
            return InternalCalls.Input_IsGamepadConnected();
        }

        public static bool GetAnyKeyDown()
        {
            return InternalCalls.Input_GetAnyKeyDown();
        }
    }
}
