#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>

#define BLUE			9
#define GREEN			10
#define RED				12
#define YELLOW			14
#define WHITE			15

class Logger {
public:
    enum class Level {
        DEBUG,
        INFO,
        WARN,
        ERR
    };
    // Prevent copying and assignment
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Singleton design pattern
    inline static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }

    // Close filestream on destruction
    ~Logger() {
        if (fileStream.is_open()) {
            fileStream.close();
        }
#ifdef _DEBUG
        // to prevent the tzdb allocations from being reported as memory leaks
        std::chrono::get_tzdb_list().~tzdb_list();
#endif
    }

    void SetLevel(Level level) {
        logLevel = level;
    }

    template<typename... Args>
    void Log(Level level, const Args&... args) {
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

        // Append message
        (stream << ... << args);

        // Output to console and file
        std::string logMessage = stream.str();
        std::cout << logMessage << std::endl;
        fileStream << logMessage << std::endl;
    }

private:
    std::ofstream fileStream;
    Level logLevel = Level::DEBUG;

    HANDLE m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Private constructor for singleton
    Logger() {
        fileStream.open("log.txt", std::ios::app);
    }
};

//#define LogWarn Log(Logger::Level::WARN, ...args)

#endif // LOGGER_HPP
