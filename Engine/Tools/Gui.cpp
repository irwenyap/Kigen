/*********************************************************************
 * \file	    Gui.cpp
 * \brief	    Defines a Gui class that contains the logic to render 
 *              and setup the Graphic User Interface (IMGUI).
 *
 * \author		wengkhong.thang, 2301372 (wengkhong.thang@digipen.edu)
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date	    29 September 2024

 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#include "Gui.hpp"
#include "../Engine/EventManager.hpp"

#include "Workspace.hpp"
#include "Panels/HierachyPanel.hpp"
#include "Panels/ScenePanel.hpp"
#include "Panels/ObjectEditorPanel.hpp"
#include "Panels/AssetBrowserPanel.hpp"
#include "Panels/LayersPanel.hpp"
#include "Panels/LoggerPanel.hpp"
#include "../Utility/EngineState.hpp"
#include "../Engine/Scene/SceneManager.hpp"
#include "Scripting/ScriptEngine.hpp"
#include "../Audio/AudioManager.hpp"
#include "Panels/GameViewPanel.hpp"


extern EngineState engineState;

//Panels
static HierachyPanel hp{};
ScenePanel sp{};
LayersPanel lp{};
AssetBrowserPanel abp{};
LoggerPanel lgp{};
GameViewPanel gvp{};
float color{ 1.f }, size{ 1.f };
ObjectEditorPanel op(&color,&size);
Gui::Entity* selectedObject = nullptr;  // This will point to the currently selected object

static void ApplyCherryBlossomTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark();

    // Customize Colors (JAPANESE)
    style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);               // Off-white
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.12f, 0.10f, 1.00f);            // Deep brown
    style.Colors[ImGuiCol_Border] = ImVec4(0.55f, 0.47f, 0.35f, 0.75f);              // Muted beige
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.28f, 0.25f, 0.20f, 1.00f);             // Light brown
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.58f, 0.45f, 0.39f, 0.60f);      // Cherry blossom pink
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.55f, 0.50f, 0.75f);       // Stronger cherry blossom pink
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.30f, 0.25f, 0.20f, 1.00f);             // Deep brown
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.45f, 0.35f, 0.30f, 1.00f);       // Medium brown
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.30f, 0.25f, 0.20f, 0.60f);    // Collapsed background
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.22f, 0.20f, 0.17f, 0.70f);           // Dark brown
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.17f, 0.15f, 0.13f, 1.00f);         // Very dark brown
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.45f, 0.35f, 0.30f, 0.60f);       // Medium brown
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.55f, 0.45f, 0.40f, 0.80f);// Light brown
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.65f, 0.55f, 0.50f, 0.80f); // Light brown active
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.75f, 0.45f, 0.40f, 1.00f);           // Muted red
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.65f, 0.35f, 0.35f, 0.80f);          // Cherry red
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.85f, 0.40f, 0.40f, 1.00f);    // Bright cherry red
    style.Colors[ImGuiCol_Button] = ImVec4(0.35f, 0.28f, 0.22f, 1.00f);              // Earthy brown
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.45f, 0.35f, 0.30f, 1.00f);       // Light brown hover
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.55f, 0.45f, 0.40f, 1.00f);        // Cherry blossom
    style.Colors[ImGuiCol_Header] = ImVec4(0.35f, 0.28f, 0.25f, 1.00f);              // Medium brown header
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.65f, 0.45f, 0.40f, 0.80f);       // Cherry blossom hover
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.75f, 0.55f, 0.50f, 1.00f);        // Cherry blossom active
    style.Colors[ImGuiCol_Separator] = ImVec4(0.55f, 0.45f, 0.40f, 1.00f);           // Muted beige
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.65f, 0.50f, 0.45f, 1.00f);    // Light beige
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.75f, 0.55f, 0.50f, 1.00f);     // Light cherry blossom
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.60f, 0.30f, 0.30f, 0.35f);      // Cherry pink selection
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.12f, 0.10f, 1.00f);             // Deep brown popup
    style.Colors[ImGuiCol_Tab] = ImVec4(0.30f, 0.20f, 0.15f, 1.00f);                 // Darker brown tab
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.50f, 0.40f, 0.30f, 1.00f);          // Light brown tab
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.55f, 0.45f, 0.35f, 1.00f);           // Medium brown active tab
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.15f, 0.12f, 1.00f);        // Darker background
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.30f, 0.25f, 0.20f, 1.00f);  // Dark brown active

    // Adjust rounding and padding for a clean, minimal look
    style.FrameRounding = 4.0f;
    style.WindowRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.ScrollbarRounding = 3.0f;
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 4);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f); // Centered title
}


static void ApplyVibrantRedTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark();

    // Customize Colors
    style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.90f, 0.80f, 1.00f);                // Light gold
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);            // Near-black
    style.Colors[ImGuiCol_Border] = ImVec4(0.45f, 0.35f, 0.10f, 0.60f);              // Golden brown
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.10f, 0.10f, 1.00f);             // Dark brownish-red
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.75f, 0.20f, 0.20f, 0.60f);      // Red
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.30f, 0.30f, 0.75f);       // Bright red
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.10f, 0.10f, 1.00f);             // Dark red
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.75f, 0.15f, 0.15f, 1.00f);       // Bright red
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.10f, 0.10f, 0.60f);    // Dark background
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.05f, 0.05f, 1.00f);           // Very dark red
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.05f, 0.05f, 1.00f);         // Near-black
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.75f, 0.20f, 0.20f, 0.80f);       // Deep red
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.85f, 0.30f, 0.30f, 0.90f);// Bright red
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.95f, 0.40f, 0.40f, 1.00f); // Very bright red
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.95f, 0.85f, 0.20f, 1.00f);           // Gold
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.95f, 0.85f, 0.20f, 1.00f);          // Gold
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.95f, 0.40f, 1.00f);    // Light gold
    style.Colors[ImGuiCol_Button] = ImVec4(0.85f, 0.20f, 0.20f, 1.00f);              // Red button
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.95f, 0.30f, 0.30f, 1.00f);       // Lighter red button
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.75f, 0.15f, 0.15f, 1.00f);        // Darker red
    style.Colors[ImGuiCol_Header] = ImVec4(0.15f, 0.35f, 0.15f, 1.00f);              // Deep green
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.45f, 0.25f, 0.80f);       // Bright green
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.55f, 0.35f, 1.00f);        // Light green
    style.Colors[ImGuiCol_Separator] = ImVec4(0.55f, 0.45f, 0.35f, 1.00f);           // Gold-brown
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.85f, 0.65f, 0.45f, 1.00f);    // Gold
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.95f, 0.85f, 0.55f, 1.00f);     // Bright gold
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.85f, 0.20f, 0.20f, 0.35f);      // Soft red selection
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.05f, 0.05f, 1.00f);             // Dark popup
    style.Colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);                 // Dark gray tab
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.75f, 0.20f, 0.20f, 1.00f);          // Red tab
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.85f, 0.30f, 0.30f, 1.00f);           // Bright red active tab
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.05f, 0.05f, 1.00f);        // Near black
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.45f, 0.15f, 0.15f, 1.00f);  // Dark red

    // Adjust rounding and padding for a clean, minimal look
    style.FrameRounding = 4.0f;
    style.WindowRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.ScrollbarRounding = 3.0f;
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 4);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f); // Centered title
}

void Gui::Init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark();


    // Customize specific colors
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.45f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.55f, 0.80f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.35f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.60f, 0.85f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.65f, 0.90f, 1.00f);

    style.FrameRounding = 4.0f;
    //style.WindowRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.ScrollbarRounding = 3.0f;
    //style.FramePadding = ImVec2(6, 4);
    //style.ItemSpacing = ImVec2(8, 4);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f); // Centered title

    //ImGui::GetIO().FontGlobalScale = 2.f;
    //ImVec4 transparent(0, 0, 0, 0); // Fully transparent color

    //// Customize collapsible header colors
    //style.Colors[ImGuiCol_Header] = transparent;         // Default state
    //style.Colors[ImGuiCol_HeaderHovered] = transparent;  // When hovered
    //style.Colors[ImGuiCol_HeaderActive] = transparent;   // When active/collapsed

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    //ImGuiIO& io = ImGui::GetIO();

    // Load a font with size 24.0f
    io.Fonts->AddFontFromFileTTF("../Library/Fonts/Helvetica.ttf", 14.0f);

    // Rebuild font texture
    ImGui_ImplOpenGL3_CreateFontsTexture();

    Workspace::AddPanel(&hp);
    //Workspace::AddPanel(&ip);
    Workspace::AddPanel(&abp);
    Workspace::AddPanel(&gvp);
    Workspace::AddPanel(&sp);
    Workspace::AddPanel(&op);
    Workspace::AddPanel(&lp);
    Workspace::AddPanel(&lgp);
#ifndef INSTALLER
    abp.Init();
#endif

    // Set the AssetBrowserPanel as the user pointer for the GLFW window
    glfwSetWindowUserPointer(window, &abp);

    // Register the drop callback
    glfwSetDropCallback(window, AssetBrowserPanel::DropCallback);
    
    sp.Init();

}

void Gui::Update(int textureID) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    ImFont* myFont = io.Fonts->Fonts[0]; // Get the first loaded font
    ImGui::PushFont(myFont);

    sp.SetTextureId(&textureID); // Set framebuffer

    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistent = true;
    bool opt_fullscreen = opt_fullscreen_persistent;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // Configure fullscreen window
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("MainWindow", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();
    if (opt_fullscreen) ImGui::PopStyleVar(2);

    // MenuBar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene")) {}

            if (ImGui::MenuItem("Open Scene", "", false, false)) {}

            ImGui::Separator();

            if (ImGui::MenuItem("Save", "Ctrl + S")) {
                SceneManager::GetInstance().SaveScene();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", "", false, false)) {}

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl + Z", false, false)) {}
            if (ImGui::MenuItem("Redo", "Ctrl + Y", false, false)) {}

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Entity")) {        
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Theme")) {
            if (ImGui::MenuItem("Cherry Blossom"))
                ApplyCherryBlossomTheme();

            if (ImGui::MenuItem("Vibrant Red"))
                ApplyVibrantRedTheme();

            ImGui::EndMenu();
        }
        /*if (ImGui::BeginMenu("Script")) { // using filewatcher
            if (ImGui::MenuItem("Reload Assembly"))
                ScriptEngine::ReloadAssembly();

            ImGui::EndMenu();
        }*/
        ImGui::EndMenuBar();
    }

    if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S)) {
        SceneManager::GetInstance().SaveScene();
    }

    // Toolbar
    float toolbarHeight = 37.0f;
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight())); // Position below MenuBar
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, toolbarHeight));
    ImGui::Begin("Toolbar", nullptr, 
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Button colors based on engineState
    auto SetButtonColor = [](EngineState state) {
        if (state == engineState) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.7f, 0.0f, 1.0f)); // Green for active
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.9f, 0.0f, 1.0f)); // Lighter green for hover
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.6f, 0.0f, 1.0f)); // Darker green for active press
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Button)); // Default color
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered)); // Default hover
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive)); // Default active press
        }
        };

    {
        // Center-align the buttons
        float availableWidth = ImGui::GetContentRegionAvail().x;
        float buttonTotalWidth = 200.0f;
        float offsetX = (availableWidth - buttonTotalWidth) / 2.0f;
        if (offsetX > 0) ImGui::SetCursorPosX(offsetX);

        SetButtonColor(EngineState::PLAYING);
        if (ImGui::Button("Play", ImVec2(60, 20))) {
            if (engineState == EngineState::STOPPED) SceneManager::GetInstance().SaveTempScene();
            engineState = EngineState::PLAYING;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        // Pause Button
        SetButtonColor(EngineState::PAUSED);
        if (ImGui::Button("Pause", ImVec2(60, 20))) {
            engineState = EngineState::PAUSED;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        // Stop Button
        SetButtonColor(EngineState::STOPPED);
        if (ImGui::Button("Stop", ImVec2(60, 20)) && (engineState == EngineState::PLAYING || engineState == EngineState::PAUSED)) {
            SceneManager::GetInstance().ReloadScene();
            engineState = EngineState::STOPPED;
            AudioManager::GetInstance().StopAll();
        }
        ImGui::PopStyleColor(3);
    }
    ImGui::End();
    ImGui::PopFont();

    // Dockspace: Render below Toolbar
    float dockspaceStartY = ImGui::GetFrameHeight() + toolbarHeight;
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    ImGui::SetNextWindowPos(ImVec2(0, dockspaceStartY));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - dockspaceStartY));
    ImGui::Begin("DockSpace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    style.WindowMinSize.x = minWinSizeX;

    // Render panels
    for (EditorPanel* panel : Workspace::panels) {
        panel->Update();
    }
    ImGui::End(); // End DockSpace

    ImGui::End(); // End MainWindow

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void Gui::Exit() {
    sp.Exit();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}