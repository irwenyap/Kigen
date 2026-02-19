/*********************************************************************
 * \file	    LoggerPanel.cpp
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

 // IMGUI
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"
#include "../ImGui/imgui_internal.h"
#include "../ImGui/cpp/imgui_stdlib.h"

#include "LoggerPanel.hpp"



LoggerPanel::LoggerPanel()
    : showDebug(true), showInfo(true), showWarn(true), showError(true), loggerOpacity(1.0f) {
    name = "Logger";
    show = true;
}

void LoggerPanel::Update() {
    if (!show) return; // If the panel is not visible, skip rendering
    ImGui::SetNextWindowBgAlpha(loggerOpacity); // Set background opacity to 100%
    ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    RenderFilters(); // Render filter and search options
    ImGui::Separator();
    RenderLogs();    // Render the log messages

    ImGui::End();
}

void LoggerPanel::RenderFilters() {
    // Filters for log levels
    ImGui::Text("Filters:");
    ImGui::SameLine();
    ImGui::Checkbox("Debug", &showDebug);
    ImGui::SameLine();
    ImGui::Checkbox("Info", &showInfo);
    ImGui::SameLine();
    ImGui::Checkbox("Warn", &showWarn);
    ImGui::SameLine();
    ImGui::Checkbox("Error", &showError);


    // Clear logs button
    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        Logger::Instance().ClearBuffer();
    }
}

void LoggerPanel::RenderLogs() {
    static std::vector<std::string> cachedLogs;
    cachedLogs.clear();

    auto logs = Logger::Instance().GetSafeLogBuffer(); // Fetch a thread-safe copy of the log buffer

    if (logs.size() != cachedLogs.size()) {
        cachedLogs = logs; // Only update if new logs are available
    }


    ImGui::BeginChild("LogOutput", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

    // Keep track of whether we should scroll to the bottom
    bool shouldScroll = (ImGui::GetScrollY() >= ImGui::GetScrollMaxY());

    for (const auto& log : logs) {
        // Apply filters and display logs as before
        if (log.find("[DEBUG]") != std::string::npos && !showDebug) continue;
        if (log.find("[INFO]") != std::string::npos && !showInfo) continue;
        if (log.find("[WARN]") != std::string::npos && !showWarn) continue;
        if (log.find("[ERROR]") != std::string::npos && !showError) continue;

        // Display logs with colors based on log level
        if (log.find("[DEBUG]") != std::string::npos) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", log.c_str());
        }
        else if (log.find("[INFO]") != std::string::npos) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", log.c_str());
        }
        else if (log.find("[WARN]") != std::string::npos) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", log.c_str());
        }
        else if (log.find("[ERROR]") != std::string::npos) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", log.c_str());
        }
    }

    // **Automatically scroll to the bottom when new messages appear**
    if (shouldScroll) 
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
}

void LoggerPanel::ClearLogs() {
    Logger::Instance().ClearBuffer();
}
