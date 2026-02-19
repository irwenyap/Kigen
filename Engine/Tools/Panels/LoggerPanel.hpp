/*********************************************************************
 * \file	    LoggerPanel.hpp
 * \brief	    Defines a LoggerPanel class for displaying logs in the workspace.
 *
 * \author	    Irwin Lim Jun, irwinjun.l, 2301305
 * \email	    irwinjun.l@digipen.edu
 * \date	    20th January 2025
 * 
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#pragma once

#include "../EditorPanel.hpp"
#include "../Core/Logger.hpp"
#include <string>

class LoggerPanel : public EditorPanel {
public:
    /**
     * @brief Constructs a `LoggerPanel` instance.
     *
     * Sets up the default name and visibility state of the logger panel.
     */
    LoggerPanel();

    /**
     * @brief Updates the logger panel.
     *
     * Renders the logger panel using ImGui, displaying logs and providing
     * options to filter, search, and clear logs.
     */
    void Update() override;

private:
    void RenderFilters();  // Render filter controls (e.g., toggles for log levels)
    void RenderLogs();     // Render the log output
    void ClearLogs();      // Clear all logs from the buffer

    bool showDebug = true;  // Filter for DEBUG logs
    bool showInfo = true;   // Filter for INFO logs
    bool showWarn = true;   // Filter for WARN logs
    bool showError = true;  // Filter for ERR logs
    std::string searchQuery; // Search bar input
    std::mutex logMutex;                 // Thread safety for local buffer

    float loggerOpacity = 1.0f; // Logger window opacity
};
