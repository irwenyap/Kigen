/*********************************************************************
 * \file	    Logger.cs
 * \brief	    Provides a logging utility for the engine.
 *
 *              This class allows logging of messages with different severity
 *              levels (DEBUG, INFO, WARN, ERROR) to the internal logging system.
 *
 * \author	    Irwin Lim Jun
 * \email	    irwinjun.l@digipen.edu
 * \date	    25th January 2025
 *
 * \copyright   Copyright (C) 2024 DigiPen Institute of Technology.
 *              Reproduction or disclosure of this file or its contents without the
 *              prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;

namespace ScriptCore
{
    /**
    * \class Logger
    * \brief Provides static logging functionality.
    *
    * The Logger class serves as an interface to the internal logging system.
    * It allows users to log messages of varying severity levels, which are
    * handled internally by the engine.
    */
    public class Logger
    {
        private static int logCount = 0;
        private static Stopwatch stopwatch = Stopwatch.StartNew();
        private static long lastLoggedTimeMs = 0;

        // Frame-based throttling
        private static int logFrameInterval = 5; // Only log every 5 frames
        private static int frameCount = 0;

        /**
        * \brief Logs a message with the specified severity level.
        *
        * This function forwards the log message to the internal logging system.
        *
        * \param message The message to be logged.
        * \param level The severity level of the log message.
        */
        public static void Log(string message, LogLevel level)
        {
            long currentTimeMs = stopwatch.ElapsedMilliseconds;

            // Proper throttling: Allow logging only if at least 16ms has passed OR log count is below 100
            if (logCount >= 100 && (currentTimeMs - lastLoggedTimeMs < 16))
                return; // Skip logging this frame

            lastLoggedTimeMs = currentTimeMs;
            logCount++;

            // Log only every N frames (prevents frame-by-frame log spam)
            if (frameCount++ % logFrameInterval != 0)
                return;

            InternalCalls.Logger_Log(message, (int)level);
        }
    }

    /**
    * \enum LogLevel
    * \brief Represents the severity levels of log messages.
    *
    * This enum defines different levels of log messages, ranging from
    * debugging information to errors.
    */
    public enum LogLevel
    {
        DEBUG = 0,
        INFO = 1,
        WARN = 2,
        ERROR = 3
    }
}
