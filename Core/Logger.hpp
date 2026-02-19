/*********************************************************************
 * \file		Logger.hpp
 * \brief		Manages the logging of different informations
 *              into the console, as well as generate a log after
 *              every execution
 *
 * \author		Yap Zi Yang Irwen, y.ziyangirwen, 2301345 | Irwin Lim Jun, irwinjun.l, 2301305
 * \email		y.ziyangirwen@digipen.edu | irwinjun.l@digipen.edu
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its contents without the
 *				prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#ifndef LOGGER_HPP
#define LOGGER_HPP

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // Prevents Windows.h from defining min and max macros
#ifdef APIENTRY
#undef APIENTRY
#endif

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <mutex>

#define BLUE			9
#define GREEN			10
#define RED				12
#define YELLOW			14
#define WHITE			15

/**
 * \class Logger
 * \brief Singleton class responsible for logging messages to both the console and a log file.
 *
 * The Logger class provides functionality for logging messages with different levels (DEBUG, INFO, WARN, ERR).
 * The log messages are written to both the console and a file. Additionally, the logger can be configured to
 * only display messages of a certain severity level or higher.
 */
class Logger {
public:
    /**
     * \enum Level
     * \brief Represents the severity level of log messages.
     */
    enum class Level {
        DEBUG,  /**< Debug messages */
        INFO,   /**< Informational messages */
        WARN,   /**< Warning messages */
        ERR     /**< Error messages */
    };

    // Delete copy constructor and assignment operator to enforce singleton pattern
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    /**
     * \brief Retrieves the singleton instance of the Logger class.
     * \return A reference to the Logger instance.
     */
    inline static Logger& Instance() {
        static Logger instance;
        return instance;
    }

    /**
     * \brief Destructor for the Logger class.
     *
     * Closes the log file if it is open, and handles any necessary cleanup in debug mode.
     */
    ~Logger() {
        if (fileStream.is_open()) {
            fileStream.close();
        }
#ifdef _DEBUG
        // To prevent timezone database allocations from being reported as memory leaks.
        std::chrono::get_tzdb_list().~tzdb_list();
#endif
    }

    /**
     * \brief Sets the logging level for the Logger.
     *
     * Messages below the specified log level will not be logged.
     * \param level The minimum severity level for log messages.
     */
    void SetLevel(Level level) {
        logLevel = level;
    }

    /**
     * \brief Logs a message to the console and the log file.
     *
     * This template function accepts a variable number of arguments and formats them as part of the log message.
     * The log message is time-stamped and formatted according to the severity level.
     *
     * \tparam Args Variadic template parameters representing the parts of the log message.
     * \param level The severity level of the log message.
     * \param args The parts of the message to log.
     */
#ifdef INSTALLER
    template<typename... Args>
    void Log(Level, const Args&...) {}
#else
    template<typename... Args>
    void Log(Level level, const Args&... args) {
        static const size_t MAX_LOGS = 100;

        if (level < logLevel) {
            return;
        }

        std::ostringstream stream;

        // Time stamping
        std::chrono::local_time<std::chrono::system_clock::duration> currTime
            = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());

        stream << std::format("{:%Y-%m-%d %X}", currTime);

        switch (level) {
        case Level::DEBUG:
            SetConsoleTextAttribute(m_hConsole, WHITE);
            stream << " [DEBUG] ";
            break;
        case Level::INFO:
            SetConsoleTextAttribute(m_hConsole, BLUE);
            stream << " [INFO] ";
            break;
        case Level::WARN:
            SetConsoleTextAttribute(m_hConsole, YELLOW);
            stream << " [WARN] ";
            break;
        case Level::ERR:
            SetConsoleTextAttribute(m_hConsole, RED);
            stream << " [ERROR] ";
            break;
        }

        (stream << ... << args);

        // Output to console and file
        std::string logMessage = stream.str();

        // Add to log buffer (thread-safe)
        {
            std::lock_guard<std::mutex> lock(logMutex);

            logBuffer.push_back(logMessage);

            // Ensure buffer size does not exceed MAX_LOGS
            if (logBuffer.size() > MAX_LOGS) {
                logBuffer.erase(logBuffer.begin(), logBuffer.begin() + (logBuffer.size() - MAX_LOGS));
            }
        }



        if (fileStream.is_open()) {
            fileStream << logMessage << std::endl;
        }
    }
#endif

    /**
    * \brief Retrieves a thread-safe copy of the log buffer.
    *
    * The log buffer contains recent log messages and is used to display logs in a UI or for debugging purposes.
    * \return A copy of the log messages stored in the buffer.
    */
    std::vector<std::string> GetSafeLogBuffer() {
        std::lock_guard<std::mutex> lock(logMutex); // Ensure thread safety
        return logBuffer; // Return a copy of the log buffer
    }

    /**
    * \brief Clears the log buffer.
    *
    * This method ensures thread safety while clearing the stored log messages.
    */
    void ClearBuffer() {
        std::lock_guard<std::mutex> lock(logMutex);
        logBuffer.clear();
    }



private:
    std::ofstream fileStream; /**< The output stream for writing log messages to a file. */
    Level logLevel = Level::DEBUG; /**< The minimum severity level for log messages. */

    std::vector<std::string> logBuffer;  // Log cache for UI
    mutable std::mutex logMutex;         // Thread safety for logBuffer

    HANDLE m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE); /**< Handle for setting the console text color. */

    /**
     * \brief Private constructor for the Logger class.
     *
     * The constructor initializes the log file in the "Logs" directory and handles any initialization required
     * for the logging system.
     */
    Logger() {
#ifndef INSTALLER
        std::filesystem::path logDir = "../Logs";
        if (!std::filesystem::exists(logDir)) {
            std::filesystem::create_directories(logDir);
        }
        fileStream.open(logDir / "log.txt", std::ios::app);
        if (!fileStream.is_open()) {
            Log(Level::WARN, "Unable to create log files for this session");
        }
#endif
    }
};

#endif // LOGGER_HPP
