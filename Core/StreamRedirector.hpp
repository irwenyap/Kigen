/*********************************************************************
 * \file	    StreamRedirector.hpp
 * \brief	    Redirects output streams (e.g., std::cout) to a logging system.
 *
 *              This class captures stream output and forwards it to the Logger
 *              while ensuring thread safety. It supports temporarily bypassing
 *              redirection and reinstating it as needed.
 *
 * \author	    Irwin Lim Jun, irwinjun.l, 2301305
 * \email	    irwinjun.l@digipen.edu
 * \date	    25th January 2025
 * 
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/
#ifndef STREAMREDIRECTOR_HPP
#define STREAMREDIRECTOR_HPP

#include <iostream>
#include <streambuf>
#include <string>
#include <mutex>
#include "Logger.hpp"

/**
 * \class StreamRedirector
 * \brief Redirects an output stream (such as std::cout) to a log.
 *
 * The StreamRedirector class captures and redirects output streams to a log system,
 * allowing the redirection of messages to be logged while maintaining thread safety.
 */
class StreamRedirector : public std::streambuf {
public:
    const size_t MAX_BUFFER_SIZE = 1024; // Prevents excessive memory use

    /**
    * \brief Constructs a StreamRedirector object and redirects the given stream.
    *
    * The original stream buffer is replaced with the StreamRedirector to capture output.
    * \param stream The output stream to redirect (e.g., std::cout).
    */
    explicit StreamRedirector(std::ostream& stream)
        : originalStream(stream), originalBuffer(stream.rdbuf(this)) {
    }

    /**
    * \brief Destructor for StreamRedirector.
    *
    * Restores the original stream buffer to prevent interference after destruction.
    */
    ~StreamRedirector() override {
        try {
            // Restore the original stream buffer
            originalStream.rdbuf(originalBuffer);
        }
        catch (...) {
            std::cerr << "Error restoring original buffer in StreamRedirector destructor" << std::endl;
        }
    }

    /**
    * \brief Temporarily bypasses the stream redirection.
    *
    * This allows the original stream to function normally without being intercepted by StreamRedirector.
    */
    void Bypass() {
        originalStream.rdbuf(originalBuffer);
    }

    /**
    * \brief Reinstate the stream redirection.
    *
    * After bypassing, this function re-enables stream redirection to the StreamRedirector.
    */
    void Reinstate() {
        originalStream.rdbuf(this);
    }

protected:

    /**
    * \brief Handles character output overflow.
    *
    * This function is called when characters are written to the stream. It buffers the input
    * and flushes the log on detecting a newline.
    * \param c The character to be written.
    * \return The written character or EOF on failure.
    */
    int overflow(int c) override {
        if (c == EOF) return EOF;

        try {
            std::lock_guard<std::mutex> lock(bufferMutex); // Thread-safe access
            char ch = static_cast<char>(c);
            buffer += ch;

            if (buffer.size() > MAX_BUFFER_SIZE || ch == '\n') {
                flushBuffer();  // Force flush if buffer is too large or a newline is encountered
            }

            return c;
        }
        catch (const std::exception& e) {
            std::cerr << "StreamRedirector exception in overflow(): " << e.what() << std::endl;
            return EOF;
        }
        catch (...) {
            std::cerr << "Unknown error in StreamRedirector::overflow()" << std::endl;
            return EOF;
        }
    }

private:

    /**
    * \brief Flushes the buffered log messages.
    *
    * When a newline character is detected, this function ensures that all buffered
    * messages are logged and the buffer is cleared.
    */
    void flushBuffer() {
        try {
            if (!buffer.empty()) {
                // Log the buffer content to the logger
                // Temporarily disable redirection to avoid recursion
                Bypass();  // Disable stream redirection
                Logger::Instance().Log(Logger::Level::INFO, buffer); // Log directly
                buffer.clear();  // Clear the buffer after flushing
                Reinstate();  // Re-enable stream redirection
            }
        }
        catch (const std::exception& e) {
            std::cerr << "StreamRedirector exception in flushBuffer(): " << e.what() << std::endl;
        }
        catch (...) {
            std::cerr << "Unknown error in StreamRedirector::flushBuffer()" << std::endl;
        }
    }

    std::ostream& originalStream;       // Reference to the original stream
    std::streambuf* originalBuffer;     // Pointer to the original stream buffer
    std::string buffer;                 // Buffer for collecting characters
    std::mutex bufferMutex;             // Mutex for thread-safe access
};

#endif // !STREAMREDIRECTOR_HPP
