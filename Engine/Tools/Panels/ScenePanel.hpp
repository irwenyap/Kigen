/*********************************************************************
 * \file	ScenePanel.hpp
 * \brief	Defines a ScenePanel class that manages everything related to the scene editor panel in the workspace.
 *
 * \author	wengkhong.thang, 2301372, irwinjun.l, 2301305
 * \email	wengkhong.thang@digipen.edu, irwinjun.l@digipen.edu
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#pragma once
#include <filesystem>
#include "../EditorPanel.hpp"
//#include "../../Graphics/FrameBuffer.hpp"
//#include "../../Graphics/Texture.hpp"

class ScenePanel : public EditorPanel {
public:

    /**
    * @brief Constructs a `ScenePanel` instance.
    *
    * Initializes the panel's name and visibility state. The texture ID
    * is set to nullptr until a valid texture is provided.
    */
    ScenePanel();     

    void Init();

    /**
     * @brief Updates the contents of the scene panel.
     *
     * This method is responsible for rendering the panel's contents
     * using ImGui. It displays the current frame rate, the percentage
     * of time spent in each system (Physics, Graphics, Audio, Misc),
     * and the rendered scene if a valid texture ID is provided.
     */
    void Update() override;   

    void Exit();

    /**
     * @brief Sets the texture ID to be displayed in the panel.
     *
     * @param texture Pointer to the texture ID to be used for rendering the scene.
     */
    void SetTextureId(int* textureID);

    void RenderImguiImage();

    //int* textureID;// texture id buffer of the scene
    std::string textureUUID;// texture id buffer of the scene
    std::filesystem::path scenePath;

    GLuint tempTexture2D; // temporary texture used to send to imgui
    Vec2 viewportSize;
};
