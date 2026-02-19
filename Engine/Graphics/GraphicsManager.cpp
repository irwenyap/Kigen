/*********************************************************************
 * \file	    GraphicsManager.cpp
 * \brief
 *              This file defines the `GraphicsManager` class, which 
 *              manages rendering operations, including loading
 *              meshes, shaders, textures, and handling batch 
 *              rendering in a graphics application.
 *
 * \author		t.yongchin, 2301359 (t.yongchin@digipen.edu)
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \author		woonli.wong, 2301308 (woonli.wong@digipen.edu)
 * \date		29 November 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/
#include "GraphicsManager.hpp"

#include <filesystem>
#include <algorithm>

#include "../Utility/MetadataHandler.hpp"
#include "../AssetManager.hpp"
#include "Logger.hpp"
#include "Math.hpp"
#include "../ECS/ECSManager.hpp"
#include "../Components/Collider2D.hpp"
#include "../Components/Camera.hpp"
#include "../Utility/EngineState.hpp"
#include "../Layers/SortingLayer.hpp"
#include "../Layers/SortingLayerManager.hpp"

extern EngineState engineState;

GraphicsManager& GraphicsManager::GetInstance()
{
    static GraphicsManager graphicsManager;
    return graphicsManager;
}

GraphicsManager::GraphicsManager() : 
    shaders(), batches(), meshes(), frameBuffers(), debugMode(false), camera(), 
	readFramebuffer(0), drawFramebuffer(0), internalFormat(GL_RGBA8)
{
    //textures.reserve(2048);

    //auto [width, height] = Application::GetWindowSize();
    camera.Init(1920, 1080);

    LoadAllTextures("../Assets");

    // Enable blending
    glEnable(GL_BLEND);
    // Set the blending function for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Load shaders
	// Note: VERY IMPORTANT not to change the load order of the shaders
	// This is because ShaderIndex enum values are assigned based on this order of loading
	// Default shader (ID 0)
    LoadShader("Shaders/default");
	// Rendering to framebuffer (ID 1)
    LoadShader("Shaders/fbo");
	// Font rendering (ID 2)
    LoadShader("Shaders/ui_font");
	// Object picking in world (ID 3)
    LoadShader("Shaders/objectpicking");
	// Texture rendering in UI (ID 4)
    LoadShader("Shaders/ui_texture");
	// Bright pass (ID 5)
    LoadShader("Shaders/bright");
	// Horizontal blur (ID 6)
    LoadShader("Shaders/horiblur");
	// Vertical blur (ID 7)
    LoadShader("Shaders/vertiblur");
	// Combine (ID 8)
    LoadShader("Shaders/combi");
	// Vignette (ID 9)
    LoadShader("Shaders/vignette");
	// Glitch (ID 10)
    LoadShader("Shaders/glitch");
	// Final output (ID 11)
    LoadShader("Shaders/finaloutput");
	// Video player (ID 12)
    LoadShader("Shaders/YCrCbRGB");
	// Object picking in UI (ID 13)
    LoadShader("Shaders/objectpicking_ui");

	for (size_t i = 0; i < FrameBufferIndex::MAX_FRAMEBUFFERS; ++i) {
        frameBuffers.emplace_back();
		frameBuffers[i].frameTexture = AssetManager::GetInstance().CreateTexture("fbo" + std::to_string(i)).get();
	}

    /*
    GLint MaxTextureImageUnits;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureImageUnits);
	std::cout << "Max texture image units: " << MaxTextureImageUnits << std::endl;

    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    std::cout << "Max texture size: " << maxTextureSize << std::endl;

	GLint maxTextureArrayLayers;
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxTextureArrayLayers);
	std::cout << "Max texture array layers: " << maxTextureArrayLayers << std::endl;
    */
}

GraphicsManager::~GraphicsManager()
{
    for (auto& tempTex : tempTextures) {
        if (tempTex != 0) glDeleteTextures(1, &tempTex);
    }
    
    for (auto& frameBuffer : frameBuffers) {
		frameBuffer.Exit();
	}

    for (auto& shader : shaders) {
		shader.DeleteProgram();
	}

    FreeTextureArrays();

    // Delete the framebuffers used by CopyTextureLayer()
    if (readFramebuffer != 0) glDeleteFramebuffers(1, &readFramebuffer);
    if (drawFramebuffer != 0) glDeleteFramebuffers(1, &drawFramebuffer);
}

void GraphicsManager::Init() 
{
    //// Create batch 0 with render mode GL_TRIANGLES and polygon mode GL_FILL
    //LoadBatch(GL_TRIANGLES, GL_FILL);
    
    // Create 1 batch for each sorting layer.
    for (SortingLayer i = 0; i < MAX_SORTING_LAYERS; ++i) {
        LoadBatch(GL_TRIANGLES, GL_FILL);
    }
    // Create batch 1 with render mode GL_TRIANGLES and polygon mode GL_LINE
    LoadBatch(GL_LINES, GL_FILL); // For collision data
    // Create batch 2 with render mode GL_TRIANGLES and polygon mode GL_FILL
    LoadBatch(GL_TRIANGLES, GL_FILL); // For text
    // Create batch 3 with render mode GL_TRIANGLES and polygon mode GL_FILL
    LoadBatch(GL_TRIANGLES, GL_FILL); // Textures in UI
    // Create batch 4 similar to UI but for video rendering
    LoadBatch(GL_TRIANGLES, GL_FILL); // Textures in UI

	// Initialize all framebuffers
	for (size_t i = 0; i < FrameBufferIndex::MAX_FRAMEBUFFERS; ++i) {
		frameBuffers[i].Init();
	}

    // Reattach the framebuffer texture
    // Note: It is important to reattach the texture after the framebuffer has been initialized
    // as reallocation of the texture array will cause the previously attached texture to be invalid
    for (auto& fb : frameBuffers) {
        fb.Attach();
	}

    // Initialize batches
    for (auto& batch : batches) {
        // Initialize the batch
        batch.Init();
    }

    // Initialization of framebuffers used by CopyTextureLayer()
    if (readFramebuffer == 0) glGenFramebuffers(1, &readFramebuffer);
    if (drawFramebuffer == 0) glGenFramebuffers(1, &drawFramebuffer);
}

void GraphicsManager::Render()
{
    glClearColor(1.f, 1.f, 1.f, 1.f);
    // Clear the color buffer (and depth buffer if needed)

    auto [width, height] = Application::GetWindowSize();
    glViewport(0, 0, width, height);

    // Bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[FrameBufferIndex::GAME].fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Batches are rendered to the binded framebuffer
    // Render from first sorting layer batch to last.
    for (size_t k = BatchIndex::FIRST_SRTG_LAYER; k < BatchIndex::LAST_SRTG_LAYER + 1; ++k) {
        if (batches[k].IsEmpty()) continue;
        batches[k].RenderToBuffer(shaders[ShaderIndex::SHDR_DEFAULT],
            frameBuffers[FrameBufferIndex::GAME], 
            GetViewMatrixGame(), GetProjectionMatrixGame());
    }

	// Note: this entire scope should NOT be called when application is lauched in installer mode
	// This is to avoid the engine view from being rendered unnecessarily
#ifndef INSTALLER
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[FrameBufferIndex::ENGINE].fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (size_t k = BatchIndex::FIRST_SRTG_LAYER; k < BatchIndex::LAST_SRTG_LAYER + 1; ++k) {
            batches[k].RenderToBuffer(shaders[ShaderIndex::SHDR_DEFAULT],
                frameBuffers[FrameBufferIndex::ENGINE],
                GetViewMatrixEngine(), GetProjectionMatrixEngine());
        }
        if (debugMode) {
            batches[BatchIndex::DEBUG_BATCH].RenderToBuffer(shaders[ShaderIndex::SHDR_DEFAULT],
                frameBuffers[FrameBufferIndex::ENGINE],
                GetViewMatrixEngine(), GetProjectionMatrixEngine());
        }
        // Render UI to engine view framebuffer
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 0.f);
        batches[BatchIndex::UI_VIDEO_TEXTURE_BATCH].RenderToBuffer(shaders[ShaderIndex::SHDR_VIDEOPLAYER], frameBuffers[FrameBufferIndex::ENGINE]);
        batches[BatchIndex::UI_TEXTURE_BATCH].RenderToBuffer(shaders[ShaderIndex::SHDR_TEXTURE_UI], frameBuffers[FrameBufferIndex::ENGINE]);
        batches[BatchIndex::UI_TEXT_BATCH].RenderToBuffer(shaders[ShaderIndex::SHDR_FONT], frameBuffers[FrameBufferIndex::ENGINE]);
    }
#endif
    
    // Render UI
    // Note: I did not add view / projection matrices for UI rendering as it is not needed
    // UI will render based on screen coordinates and should not be affected by camera position
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[FrameBufferIndex::UI].fbo);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    batches[BatchIndex::UI_VIDEO_TEXTURE_BATCH].RenderToBuffer(shaders[ShaderIndex::SHDR_VIDEOPLAYER], frameBuffers[FrameBufferIndex::UI]);
    batches[BatchIndex::UI_TEXTURE_BATCH].RenderToBuffer(shaders[ShaderIndex::SHDR_TEXTURE_UI], frameBuffers[FrameBufferIndex::UI]);
    batches[BatchIndex::UI_TEXT_BATCH].RenderToBuffer(shaders[ShaderIndex::SHDR_FONT], frameBuffers[FrameBufferIndex::UI]);
    glClearColor(1.f, 1.f, 1.f, 1.f);

    Camera camComponent{};
    if (ECSManager::GetInstance().TryGetComponent<Camera>(activeCamera) != std::nullopt)
        camComponent = ECSManager::GetInstance().GetComponent<Camera>(activeCamera);
    // Bind bright pass framebuffer
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[FrameBufferIndex::BRIGHT].fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders[ShaderIndex::SHDR_BRIGHT].Use();
        auto& textureArray = Texture::GetTextureArray();
        size_t arrayIndex = frameBuffers[FrameBufferIndex::GAME].frameTexture->texArrayIndex;
        size_t layerIndex = frameBuffers[FrameBufferIndex::GAME].frameTexture->texLayerIndex;
        textureArray[arrayIndex].Bind(static_cast<int>(arrayIndex));


        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_BRIGHT].id_gl, "screenTexture"), static_cast<GLint>(arrayIndex));
        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_BRIGHT].id_gl, "layerIndex"), static_cast<GLint>(layerIndex));
        glUniform1f(glGetUniformLocation(shaders[ShaderIndex::SHDR_BRIGHT].id_gl, "vignetteStrength"), 1.0f);

        glBindVertexArray(frameBuffers[FrameBufferIndex::BRIGHT].quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    // bright pass

    // Hori blur pass
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[FrameBufferIndex::HORIBLUR].fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders[ShaderIndex::SHDR_HORIBLUR].Use();
        auto& textureArray = Texture::GetTextureArray();
        size_t arrayIndex = frameBuffers[FrameBufferIndex::BRIGHT].frameTexture->texArrayIndex;
        size_t layerIndex = frameBuffers[FrameBufferIndex::BRIGHT].frameTexture->texLayerIndex;
        textureArray[arrayIndex].Bind(static_cast<int>(arrayIndex));


        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_HORIBLUR].id_gl, "screenTexture"), static_cast<GLint>(arrayIndex));
        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_HORIBLUR].id_gl, "layerIndex"), static_cast<GLint>(layerIndex));

        glBindVertexArray(frameBuffers[FrameBufferIndex::HORIBLUR].quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    // end hori blur pass

    // Verti blur pass
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[FrameBufferIndex::VERTBLUR].fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders[ShaderIndex::SHDR_VERTBLUR].Use();
        auto& textureArray = Texture::GetTextureArray();
        size_t arrayIndex = frameBuffers[FrameBufferIndex::HORIBLUR].frameTexture->texArrayIndex;
        size_t layerIndex = frameBuffers[FrameBufferIndex::HORIBLUR].frameTexture->texLayerIndex;
        textureArray[arrayIndex].Bind(static_cast<int>(arrayIndex));


        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_VERTBLUR].id_gl, "screenTexture"), static_cast<GLint>(arrayIndex));
        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_VERTBLUR].id_gl, "layerIndex"), static_cast<GLint>(layerIndex));

        glBindVertexArray(frameBuffers[FrameBufferIndex::VERTBLUR].quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    // verti hori blur pass

    // combine
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[FrameBufferIndex::COMBINE].fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders[ShaderIndex::SHDR_COMBINE].Use();
        auto& textureArray = Texture::GetTextureArray();
        size_t OGarrayIndex = frameBuffers[FrameBufferIndex::GAME].frameTexture->texArrayIndex;
        size_t OGlayerIndex = frameBuffers[FrameBufferIndex::GAME].frameTexture->texLayerIndex;
        textureArray[OGarrayIndex].Bind(static_cast<int>(OGarrayIndex));
        size_t BarrayIndex = frameBuffers[FrameBufferIndex::VERTBLUR].frameTexture->texArrayIndex;
        size_t BlayerIndex = frameBuffers[FrameBufferIndex::VERTBLUR].frameTexture->texLayerIndex;
        textureArray[BarrayIndex].Bind(static_cast<int>(BarrayIndex));

        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_COMBINE].id_gl, "screenTexture"), static_cast<GLint>(OGarrayIndex));
        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_COMBINE].id_gl, "screenLayerIndex"), static_cast<GLint>(OGlayerIndex));

        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_COMBINE].id_gl, "blurTexture"), static_cast<GLint>(BarrayIndex));
        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_COMBINE].id_gl, "blurLayerIndex"), static_cast<GLint>(BlayerIndex));
        glUniform1f(glGetUniformLocation(shaders[ShaderIndex::SHDR_COMBINE].id_gl, "bloomIntensity"), static_cast<GLfloat>(camComponent.bloomIntensity));

        glBindVertexArray(frameBuffers[FrameBufferIndex::COMBINE].quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    // combine

    // vignette
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[FrameBufferIndex::VIGNETTE].fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders[ShaderIndex::SHDR_VIGNETTE].Use();
        auto& textureArray = Texture::GetTextureArray();
        size_t arrayIndex = frameBuffers[FrameBufferIndex::COMBINE].frameTexture->texArrayIndex;
        size_t layerIndex = frameBuffers[FrameBufferIndex::COMBINE].frameTexture->texLayerIndex;
        textureArray[arrayIndex].Bind(static_cast<int>(arrayIndex));

        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_VIGNETTE].id_gl, "screenTexture"), static_cast<GLint>(arrayIndex));
        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_VIGNETTE].id_gl, "layerIndex"), static_cast<GLint>(layerIndex));

        glUniform1f(glGetUniformLocation(shaders[ShaderIndex::SHDR_VIGNETTE].id_gl, "vignetteStrength"), static_cast<GLfloat>(camComponent.vignetteStrength));
        glUniform1f(glGetUniformLocation(shaders[ShaderIndex::SHDR_VIGNETTE].id_gl, "vignetteSoftness"), static_cast<GLfloat>(camComponent.vignetteSoftness));
        glUniform2f(glGetUniformLocation(shaders[ShaderIndex::SHDR_VIGNETTE].id_gl, "vignetteCenter"),
            static_cast<GLfloat>(camComponent.vignetteCenter.x), static_cast<GLfloat>(camComponent.vignetteCenter.y));

        glBindVertexArray(frameBuffers[FrameBufferIndex::VIGNETTE].quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    // vignette

    // glitch
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[FrameBufferIndex::GLITCH].fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders[ShaderIndex::SHDR_GLITCH].Use();
        auto& textureArray = Texture::GetTextureArray();
        size_t arrayIndex = frameBuffers[FrameBufferIndex::VIGNETTE].frameTexture->texArrayIndex;
        size_t layerIndex = frameBuffers[FrameBufferIndex::VIGNETTE].frameTexture->texLayerIndex;
        textureArray[arrayIndex].Bind(static_cast<int>(arrayIndex));

        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_GLITCH].id_gl, "screenTexture"), static_cast<GLint>(arrayIndex));
        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_GLITCH].id_gl, "layerIndex"), static_cast<GLint>(layerIndex));
        glUniform1f(glGetUniformLocation(shaders[ShaderIndex::SHDR_GLITCH].id_gl, "glitchIntensity"), 0.2f);
        glUniform1f(glGetUniformLocation(shaders[ShaderIndex::SHDR_GLITCH].id_gl, "time"), static_cast<GLfloat>(glfwGetTime()));

        glBindVertexArray(frameBuffers[FrameBufferIndex::GLITCH].quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    // glitch

    // Game + UI
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[FrameBufferIndex::GAME_FINAL].fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders[ShaderIndex::SHDR_FINAL].Use();
        auto& textureArray = Texture::GetTextureArray();
        size_t gArrayIndex = frameBuffers[FrameBufferIndex::VIGNETTE].frameTexture->texArrayIndex;
        size_t gLayerIndex = frameBuffers[FrameBufferIndex::VIGNETTE].frameTexture->texLayerIndex;
        textureArray[gArrayIndex].Bind(static_cast<int>(gArrayIndex));
        size_t uArrayIndex = frameBuffers[FrameBufferIndex::UI].frameTexture->texArrayIndex;
        size_t uLayerIndex = frameBuffers[FrameBufferIndex::UI].frameTexture->texLayerIndex;
        textureArray[uArrayIndex].Bind(static_cast<int>(uArrayIndex));

        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_FINAL].id_gl, "gameTexture"), static_cast<GLint>(gArrayIndex));
        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_FINAL].id_gl, "gameLayerIndex"), static_cast<GLint>(gLayerIndex));

        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_FINAL].id_gl, "uiTexture"), static_cast<GLint>(uArrayIndex));
        glUniform1i(glGetUniformLocation(shaders[ShaderIndex::SHDR_FINAL].id_gl, "uiLayerIndex"), static_cast<GLint>(uLayerIndex));

        glBindVertexArray(frameBuffers[FrameBufferIndex::GAME_FINAL].quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    // Game + UI
    
#ifndef INSTALLER
    // Render to the object picking framebuffer (engine)
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[FrameBufferIndex::OBJ_PICKING_ENGINE].fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Render from first sorting layer batch to last.
    for (size_t k = BatchIndex::FIRST_SRTG_LAYER; k < BatchIndex::LAST_SRTG_LAYER + 1; ++k) {
        batches[k].RenderToBuffer(shaders[ShaderIndex::SHDR_OBJ_PICKING_WORLD],
            frameBuffers[FrameBufferIndex::OBJ_PICKING_ENGINE],
            GetViewMatrixEngine(), GetProjectionMatrixEngine());
    }

    glDisable(GL_DEPTH_TEST);
    batches[BatchIndex::UI_TEXTURE_BATCH].RenderToBuffer(shaders[ShaderIndex::SHDR_OBJ_PICKING_UI],
        frameBuffers[FrameBufferIndex::OBJ_PICKING_ENGINE]);

#endif

    // Render to the object picking framebuffer (game)
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[FrameBufferIndex::OBJ_PICKING_GAME].fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Render from first sorting layer batch to last.
    for (size_t k = BatchIndex::FIRST_SRTG_LAYER; k < BatchIndex::LAST_SRTG_LAYER + 1; ++k) {
        batches[k].RenderToBuffer(shaders[ShaderIndex::SHDR_OBJ_PICKING_WORLD],
            frameBuffers[FrameBufferIndex::OBJ_PICKING_GAME],
            GetViewMatrixGame(), GetProjectionMatrixGame());
    }

	// Render the UI to the object picking framebuffers
    glDisable(GL_DEPTH_TEST);
    batches[BatchIndex::UI_TEXTURE_BATCH].RenderToBuffer(shaders[ShaderIndex::SHDR_OBJ_PICKING_UI],
        frameBuffers[FrameBufferIndex::OBJ_PICKING_GAME]);

    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[FrameBufferIndex::OBJ_PICKING_UI].fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    batches[BatchIndex::UI_TEXTURE_BATCH].RenderToBuffer(shaders[ShaderIndex::SHDR_OBJ_PICKING_UI],
        frameBuffers[FrameBufferIndex::OBJ_PICKING_UI]);

    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen

#ifdef INSTALLER
    // This renders to the screen
    RenderToScreen(ShaderIndex::SHDR_FRAMEBUFFER, FrameBufferIndex::GAME_FINAL);
    //RenderToScreen(ShaderIndex::FRAMEBUFFER, 1);
#else
    if (gameWindowMode != GameWindowMode::ENGINE) {
        RenderToScreen(ShaderIndex::SHDR_FRAMEBUFFER, FrameBufferIndex::GAME_FINAL);
    }
#endif // INSTALLER
}

void GraphicsManager::Exit()
{
	for (auto& batch : batches) {
		batch.Exit();
	}

 //   for (auto& frameBuffer : frameBuffers) {
	//	frameBuffer.Exit();
	//}

 //   FreeTextureArrays();

    batches.clear();
}

void GraphicsManager::LoadBatch(GLuint renderMode, GLuint polygonMode)
{
    batches.emplace_back(batches.size(), renderMode, polygonMode);
}

size_t GraphicsManager::LoadMesh(std::string const& meshPath)
{
    meshPath;
    return 0;
    // Load model from file
    // temp, add loading from file later
}

size_t GraphicsManager::LoadMesh(std::vector<Vertex> const& vertices, std::vector<unsigned int> const& indices)
{
    // This function adds to DEFAULT_BATCH
    BatchIndex batchID = FIRST_SRTG_LAYER;

    // Initialize the model space positions based on the vertices
    std::vector<Vec3> modelSpacePostions;
    for (const auto& vertex : vertices) modelSpacePostions.push_back(vertex.position);

    // Create a new mesh with the vertices and indices
    meshes.emplace_back(vertices, indices, modelSpacePostions, batchID);

    // Add the mesh ID to the batch
    AddToBatch(batchID, meshes.back().id);
    
    return meshes.back().id;
}

size_t GraphicsManager::LoadMeshCollision(size_t meshID)
{
    // This function adds to DEBUG_BATCH
    BatchIndex batchID = DEBUG_BATCH;

    // Ensure there is mesh data to create a collision box
    if (meshID >= meshes.size() || meshes[meshID].vertices.empty()) {
        std::cout << "ERROR: Invalid mesh for creating collision box" << std::endl;
        return static_cast<size_t>(-1);
    }

    // 2D Collision box mesh (ignore z-axis)
    Vec2 min = Vec2(meshes[meshID].vertices[0].position.x, meshes[meshID].vertices[0].position.y);
    Vec2 max = Vec2(meshes[meshID].vertices[0].position.x, meshes[meshID].vertices[0].position.y);

    for (const auto& vertex : meshes[meshID].vertices) {
        // Update min and max (ignoring z-axis)
        if (vertex.position.x < min.x) min.x = vertex.position.x;
        if (vertex.position.y < min.y) min.y = vertex.position.y;
        if (vertex.position.x > max.x) max.x = vertex.position.x;
        if (vertex.position.y > max.y) max.y = vertex.position.y;
    }

    // Initialize the vertices
    std::vector<Vertex> vertices{
        Vertex({min.x, max.y, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {}, {}),
        Vertex({max.x, max.y, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {}, {}),
        Vertex({max.x, min.y, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {}, {}),
        Vertex({min.x, min.y, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {}, {})
    };
    // Initialize the model space positions
    std::vector<Vec3> modelSpacePostions;
    for (const auto& vertex : vertices) modelSpacePostions.push_back(vertex.position);
    // Initialize the indices
    std::vector<unsigned int> indices{
        0, 1, // Bottom edge
        1, 2, // Right edge
        2, 3, // Top edge
        3, 0,  // Left edge
    };

    // Create a new mesh with the vertices and indices
    meshes.emplace_back(vertices, indices, modelSpacePostions, batchID);

    // Add the mesh ID to the batch
    AddToBatch(batchID, meshes.back().id);

    return meshes.back().id;
}

void GraphicsManager::RefreshMeshCollision(size_t meshID, size_t meshDebugID, Entity entity)
{
    // Ensure there is mesh data to create a collision box
    if (meshes[meshDebugID].vertices.size() != 4 || 
        meshes[meshDebugID].vertices.empty() || 
        meshes[meshID].vertices.empty() ||
        meshID == meshDebugID) {
        Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] Invalid mesh for creating collision box");
        return;
    }

    std::optional<AABBCollider2D> collider = ECSManager::GetInstance().TryGetComponent<AABBCollider2D>(entity);
    if (!collider.has_value()) {
        SetVisibilityToMesh(meshDebugID, false);
    }
    else {
        SetVisibilityToMesh(meshDebugID, true);
        // Update the position of the 2D collision box mesh (ignore z-axis)
        meshes[meshDebugID].vertices[0].position = Vec3{ collider.value().min.x, collider.value().max.y, 0.5f}; // Top-left
        meshes[meshDebugID].vertices[1].position = Vec3{ collider.value().max.x, collider.value().max.y, 0.5f }; // Top-right
        meshes[meshDebugID].vertices[2].position = Vec3{ collider.value().max.x, collider.value().min.y, 0.5f }; // Bottom-right
        meshes[meshDebugID].vertices[3].position = Vec3{ collider.value().min.x, collider.value().min.y, 0.5f }; // Bottom-left
        SetBatchUpdateFlag(meshDebugID, false);
    }
}

size_t GraphicsManager::LoadQuadMesh(size_t batchID, std::vector<Vertex> const& vertices)
{
    if (batchID >= batches.size()) {
		Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] Invalid batch ID: " + std::to_string(batchID));
		return static_cast<size_t>(-1);
	}
    
    std::vector<unsigned int> indices{
        0, 1, 2, 2, 3, 0
    };
    std::vector<Vec3> modelSpacePostions;
    if (vertices.size() != 4) {
        std::vector<Vertex> vertices_t = {
            Vertex({-0.25f,  0.25f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {}, {0.0f, 1.0f}), // Top left
            Vertex({ 0.25f,  0.25f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {}, {1.0f, 1.0f}), // Top right
            Vertex({ 0.25f, -0.25f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {}, {1.0f, 0.0f}), // Bottom right
            Vertex({-0.25f, -0.25f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {}, {0.0f, 0.0f})  // Bottom left
        };
        // Save the model space positions
        for (const auto& vertex : vertices_t) modelSpacePostions.push_back(vertex.position);

        meshes.emplace_back(vertices_t, indices, modelSpacePostions, batchID);
        AddToBatch(static_cast<BatchIndex>(batchID), meshes.back().id);
    }
    else {
        for (const auto& vertex : vertices) modelSpacePostions.push_back(vertex.position);

        meshes.emplace_back(vertices, indices, modelSpacePostions, batchID);
        AddToBatch(static_cast<BatchIndex>(batchID), meshes.back().id);
    }
    return meshes.back().id;
}

size_t GraphicsManager::LoadTriangleMesh(size_t batchID, std::vector<Vertex> const& vertices)
{
    if (batchID >= batches.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] Invalid batch ID: " + std::to_string(batchID));
        return static_cast<size_t>(-1);
    }

    std::vector<unsigned int> indices{
        0, 1, 2
    };
    std::vector<Vec3> modelSpacePostions;
    if (vertices.size() != 3) {
        std::vector<Vertex> vertices_t = {
            Vertex({ 0.25f,  0.25f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}),
            Vertex({ 0.25f, -0.25f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}),
            Vertex({-0.25f, -0.25f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f})
        };
        // Save the model space positions
        for (const auto& vertex : vertices_t) modelSpacePostions.push_back(vertex.position);

        meshes.emplace_back(vertices_t, indices, modelSpacePostions, batchID);
        AddToBatch(static_cast<BatchIndex>(batchID), meshes.back().id);
    }
    else {
        for (const auto& vertex : vertices) modelSpacePostions.push_back(vertex.position);

        meshes.emplace_back(vertices, indices, modelSpacePostions, batchID);
        AddToBatch(static_cast<BatchIndex>(batchID), meshes.back().id);
    }

    return meshes.back().id;
}

size_t GraphicsManager::LoadLineMesh(size_t batchID, std::vector<Vertex> const& vertices)
{
    if (batchID >= batches.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] Invalid batch ID: " + std::to_string(batchID));
        return static_cast<size_t>(-1);
    }

    std::vector<unsigned int> indices{
        0, 1
    };
    std::vector<Vec3> modelSpacePostions;
    if (vertices.size() != 2) {
        std::vector<Vertex> vertices_t = {
            Vertex({ 0.5f, 0.0f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {}, {}),
            Vertex({-0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {}, {})
        };
        // Save the model space positions
        for (const auto& vertex : vertices_t) modelSpacePostions.push_back(vertex.position);

        meshes.emplace_back(vertices_t, indices, modelSpacePostions, batchID);
        AddToBatch(static_cast<BatchIndex>(batchID), meshes.back().id);
    }
    else {
        for (const auto& vertex : vertices) modelSpacePostions.push_back(vertex.position);

        meshes.emplace_back(vertices, indices, modelSpacePostions, batchID);
        AddToBatch(static_cast<BatchIndex>(batchID), meshes.back().id);
    }

    return meshes.back().id;
}

size_t GraphicsManager::LoadSphereMesh(size_t batchID, std::vector<Vertex> const& vertices)
{
    if (batchID >= batches.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] Invalid batch ID: " + std::to_string(batchID));
        return static_cast<size_t>(-1);
    }

    const int segments = 314;
    const float radius = 0.25f;

    std::vector<unsigned int> indices;
    std::vector<Vec3> modelSpacePostions;
    for (int i = 1; i <= segments; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    if (vertices.empty()) {
        std::vector<Vertex> vertices_t;
        // Center
        vertices_t.push_back(Vertex({ 0.0f, 0.0f, 0.5f }, { 1.0f, 1.0f, 1.0f, 1.0f }));
        // Circle
        for (int i = 0; i <= segments; ++i) {
            float theta = 2.0f * 3.1415926f * float(i) / float(segments);
            float x = radius * cosf(theta);
            float y = radius * sinf(theta);
            vertices_t.push_back(Vertex({ x, y, 0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f }));
        }
        // Save the model space positions
        for (const auto& vertex : vertices_t) modelSpacePostions.push_back(vertex.position);

        meshes.emplace_back(vertices_t, indices, modelSpacePostions, batchID);
        AddToBatch(static_cast<BatchIndex>(batchID), meshes.back().id);
    }
    else {
        for (const auto& vertex : vertices) modelSpacePostions.push_back(vertex.position);

        meshes.emplace_back(vertices, indices, modelSpacePostions, batchID);
        AddToBatch(static_cast<BatchIndex>(batchID), meshes.back().id);
    }

    return meshes.back().id;
}

size_t GraphicsManager::LoadTextCharacterMesh(size_t batchID, std::vector<Vertex> const& vertices)
{
    if (batchID >= batches.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] Invalid batch ID: " + std::to_string(batchID));
        return static_cast<size_t>(-1);
    }

    std::vector<unsigned int> indices{
		0, 1, 2, 2, 3, 0
	};
    std::vector<Vec3> modelSpacePostions;
    if (vertices.size() != 4) {
        std::vector<Vertex> vertices_t = {
            Vertex({-0.1f,  0.1f, 0.8f}, {1.0f, 0.0f, 0.0f, 1.0f}, {}, {0.0f, 1.0f}),
            Vertex({ 0.1f,  0.1f, 0.8f}, {0.0f, 1.0f, 0.0f, 1.0f}, {}, {1.0f, 1.0f}),
            Vertex({ 0.1f, -0.1f, 0.8f}, {0.0f, 0.0f, 1.0f, 1.0f}, {}, {1.0f, 0.0f}),
            Vertex({-0.1f, -0.1f, 0.8f}, {1.0f, 0.0f, 0.0f, 1.0f}, {}, {0.0f, 0.0f})
            
		};
        // Save the model space positions
        for (const auto& vertex : vertices_t) modelSpacePostions.push_back(vertex.position);

        meshes.emplace_back(vertices_t, indices, modelSpacePostions, batchID);
        AddToBatch(static_cast<BatchIndex>(batchID), meshes.back().id);
	}
    else {
        for (const auto& vertex : vertices) modelSpacePostions.push_back(vertex.position);

        meshes.emplace_back(vertices, indices, modelSpacePostions, batchID);
        AddToBatch(static_cast<BatchIndex>(batchID), meshes.back().id);
	}

	return meshes.back().id;
}

size_t GraphicsManager::LoadShader(std::string const& path)
{
    shaders.emplace_back(*AssetManager::GetInstance().Load<Shader>(path, true));
    return shaders.back().id;
}

void GraphicsManager::RenderToScreen(size_t shaderIndex, size_t frameBuffIndex)
{
	shaders[shaderIndex].Use();
	
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Set the viewport to the framebuffer size
    auto [width, height] = Application::GetWindowSize();
    glViewport(0, 0, width, height);

    // Bind the framebuffer texture
    auto& textureArray = Texture::GetTextureArray();
    size_t arrayIndex = frameBuffers[frameBuffIndex].frameTexture->texArrayIndex;
    size_t layerIndex = frameBuffers[frameBuffIndex].frameTexture->texLayerIndex;
    textureArray[arrayIndex].Bind(static_cast<int>(arrayIndex));

    //glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(frameBuffers[frameBuffIndex].frameTexture->id));
    //glBindTexture(GL_TEXTURE_2D, frameBuffers[frameBuffIndex].frameTexture->id_gl);

    GLint loc = glGetUniformLocation(shaders[shaderIndex].id_gl, "screenTexture");
    if (loc != -1) {
        glUniform1i(loc, static_cast<GLint>(arrayIndex));  // Send to the shader
	}

    GLint loc2 = glGetUniformLocation(shaders[shaderIndex].id_gl, "layerIndex");
    if (loc2 != -1) {
		glUniform1i(loc2, static_cast<GLint>(layerIndex));  // Send to the shader
	}

    // Set the polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDisable(GL_BLEND);

    // Render the fullscreen quad
    glBindVertexArray(frameBuffers[frameBuffIndex].quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glEnable(GL_BLEND);

    // Unbind
    glBindVertexArray(0);
}

void GraphicsManager::FreeTextureArrays()
{
    auto& textureArrays = Texture::GetTextureArray();
	for (size_t i = 0; i < textureArrays.size(); ++i) {
        if (textureArrays[i].id_gl != 0) {
            glDeleteTextures(1, &textureArrays[i].id_gl);
        }
        textureArrays[i] = TextureArray();
	}
}

//size_t GraphicsManager::GetTextureID(Vertex& vertex) 
//{
//    for (const auto& pair : textures) {
//        if (pair.second.texArrayIndex == vertex.texArray && pair.second.texLayerIndex == vertex.texLayer) {
//            return pair.second.id;
//        }
//    }
//
//	return static_cast<size_t>(-1);
//}

bool GraphicsManager::LoadFont(std::string const& path)
{
	return AssetManager::GetInstance().Get<Font>(path) != nullptr;
}

GLuint GraphicsManager::GenerateTempTexture(const std::string& textureUUID)
{
    GLuint arrayIndex = static_cast<GLuint>(GraphicsManager::GetInstance().GetTextureArrayIndex(textureUUID));
    auto& textureArray = Texture::GetTextureArray();
    int width = textureArray[arrayIndex].width;
    int height = textureArray[arrayIndex].height;

    GLuint tempTexture2D = 0;
    glGenTextures(1, &tempTexture2D);
    glBindTexture(GL_TEXTURE_2D, tempTexture2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    
    tempTextures.push_back(tempTexture2D);
    return tempTexture2D;
}

void GraphicsManager::UpdateTempTexture(const std::string& textureUUID, GLuint tempTex)
{
    GLuint arrayIndex = static_cast<GLuint>(GraphicsManager::GetInstance().GetTextureArrayIndex(textureUUID));
    GLuint layerIndex = static_cast<GLuint>(GraphicsManager::GetInstance().GetTextureLayerIndex(textureUUID));
    GLuint arrayIDGL = static_cast<GLuint>(GraphicsManager::GetInstance().GetTextureArrayIDGL(textureUUID));

    auto& textureArray = Texture::GetTextureArray();
    int width = textureArray[arrayIndex].width;
    int height = textureArray[arrayIndex].height;

    /*if (openGLVersion.first >= 4 && openGLVersion.second >= 3) {
        glCopyImageSubData(arrayIDGL, GL_TEXTURE_2D_ARRAY, 0, 0, 0, layerIndex,
            tempTex, GL_TEXTURE_2D, 0, 0, 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 1);
	}
    else {
        CopyTextureLayer(arrayIDGL, GL_TEXTURE_2D_ARRAY, layerIndex, tempTex, GL_TEXTURE_2D, width, height);
    }*/

    CopyTextureLayer(arrayIDGL, layerIndex, tempTex, width, height);
}

void GraphicsManager::BindTempTexture(const std::string& textureUUID, GLuint tempTex)
{
    //int texWidth = GetTextureWidth(textureID);
    //int texHeight = GetTextureHeight(textureID);
    //texWidth; texHeight;

    glBindTexture(GL_TEXTURE_2D, tempTex);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
        GetTextureWidth(textureUUID),
        GetTextureHeight(textureUUID),
        0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

void GraphicsManager::UnbindTempTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GraphicsManager::CopyTextureLayer(GLuint srcTex, GLuint srcLayer, GLuint destTex, int width, int height)
{
    // The reason you generate here is because this function might be called in ImGUI before GraphicsManager::Init()
    if (readFramebuffer == 0) glGenFramebuffers(1, &readFramebuffer);
	if (drawFramebuffer == 0) glGenFramebuffers(1, &drawFramebuffer);

    // Set up read framebuffer and attach the source texture
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);
    glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, srcTex, 0, srcLayer);
    
    // Check the read framebuffer status
    if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		Logger::Instance().Log(Logger::Level::ERR, "Read framebuffer is not complete");
        return;
    }

    // Set up draw framebuffer and attach the destination texture
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTex, 0);

    // Check the draw framebuffer status
    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::Instance().Log(Logger::Level::ERR, "Draw framebuffer is not complete");
        return;
    }

    // Perform the blit operation
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Unbind framebuffers
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

GLuint GraphicsManager::CreateTextureView(GLuint textureArray, int layer)
{
    GLuint textureView;
    glGenTextures(1, &textureView);

    glTextureView(textureView, GL_TEXTURE_2D, textureArray, internalFormat, 0, 1, layer, 1);

    //glBindTexture(GL_TEXTURE_2D, textureView);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM: error = "GL_INVALID_ENUM"; break;
        case GL_INVALID_VALUE: error = "GL_INVALID_VALUE"; break;
        case GL_INVALID_OPERATION: error = "GL_INVALID_OPERATION"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
        case GL_OUT_OF_MEMORY: error = "GL_OUT_OF_MEMORY"; break;
        default: error = "UNKNOWN_ERROR"; break;
        }
        //std::cerr << "[OpenGL Error] (" << error << ") in function: " << functionName << std::endl;
    }

    return textureView;
}

size_t GraphicsManager::GetTextureArrayIndex(const std::string& uuid) const
{
    return AssetManager::GetInstance().Get<Texture>(uuid)->texArrayIndex;
    //return textures.find(uid)->second.texArrayIndex;
}

size_t GraphicsManager::GetTextureLayerIndex(const std::string& uuid) const
{
    return AssetManager::GetInstance().Get<Texture>(uuid)->texLayerIndex;
    //return textures.find(uuid)->second.texLayerIndex;
}

GLuint GraphicsManager::GetTextureArrayIDGL(const std::string& uuid) const
{
    auto& textureArrays = Texture::GetTextureArray();
    return textureArrays[AssetManager::GetInstance().Get<Texture>(uuid)->texArrayIndex].id_gl;
}

int GraphicsManager::GetTextureWidth(const std::string& uuid) const
{
    auto& textureArrays = Texture::GetTextureArray();
	return textureArrays[AssetManager::GetInstance().Get<Texture>(uuid)->texArrayIndex].width;
}

int GraphicsManager::GetTextureHeight(const std::string& uuid) const
{
    auto& textureArrays = Texture::GetTextureArray();
    return textureArrays[AssetManager::GetInstance().Get<Texture>(uuid)->texArrayIndex].height;
}

GLuint GraphicsManager::GetShaderIDGL(size_t index) const
{
    if (index >= shaders.size()) {
		Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] GetShaderIDGL: Invalid shader index");
		return 0;
	}
	return shaders[index].id_gl;
}

Vec4 GraphicsManager::GetPixelColor(FrameBuffer& framebuffer, int mouseX, int mouseY)
{
    auto [appWidth, appHeight] = Application::GetWindowSize();
    mouseX = static_cast<int>(mouseX * (framebuffer.width / static_cast<float>(appWidth)));
    mouseY = static_cast<int>((appHeight - mouseY) * (framebuffer.height / static_cast<float>(appHeight)));
    
    // Read the pixel color from the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

	// Read the pixel color
	unsigned char pixelData[4];
	glReadPixels(mouseX, mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

	// Unbind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return Vec4{ pixelData[0] / 255.f, pixelData[1] / 255.f, pixelData[2] / 255.f, pixelData[3] / 255.f };
}

void GraphicsManager::LoadAllTextures(const std::string& assetsFolder) {
    namespace fs = std::filesystem;

    for (const auto& entry : fs::recursive_directory_iterator(assetsFolder)) {
        const auto& path = entry.path();

        // Check if the file is a texture (e.g., .png, .jpg, etc.)
        if (entry.is_regular_file() && (path.extension() == ".png" || path.extension() == ".jpg")) {
            std::string texturePath = path.string();
            std::string metaFilePath = texturePath + ".meta";

            std::string textureUUID;

            // Check if the meta file exists
            if (fs::exists(metaFilePath)) {
                // Extract UUID from the meta file
                textureUUID = MetadataHandler::ParseUUIDFromMeta(metaFilePath);
            } else {
                // Generate a new meta file with a UUID
                textureUUID = MetadataHandler::GenerateCustomUUID(texturePath);
                MetadataHandler::GenerateMetaFile(texturePath);
            }

            //LoadTexture(texturePath);
        } else if (entry.is_regular_file() && (path.extension() == ".mpg")) {
            std::string texturePath = path.string();
            std::string metaFilePath = texturePath + ".meta";

            std::string textureUUID;

            // Check if the meta file exists
            if (fs::exists(metaFilePath)) {
                // Extract UUID from the meta file
                textureUUID = MetadataHandler::ParseUUIDFromMeta(metaFilePath);
            } else {
                // Generate a new meta file with a UUID
                textureUUID = MetadataHandler::GenerateCustomUUID(texturePath);
                MetadataHandler::GenerateMetaFile(texturePath);
            }

            //LoadTexture(texturePath);
        }
    }
}

bool GraphicsManager::AddToBatch(BatchIndex batchID, size_t meshID)
{
    if (batchID >= batches.size() ||
        meshID >= meshes.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[RenderSystem] AddToBatch: Invalid batch or mesh ID");
        return false;
    }

    BatchData& batch = batches[batchID];
    if (std::find(batch.meshIDs.begin(), batch.meshIDs.end(), meshID) != batch.meshIDs.end()) {
        Logger::Instance().Log(Logger::Level::WARN, "[RenderSystem] AddToBatch: Mesh already in batch");
        return false;
    }
    batch.meshIDs.push_back(meshID);
    meshes[meshID].batchID = batchID;
    batch.isSorted = false;   // Adding a mesh requires the batch to be sorted again
    batch.isUpdated = false;  // Adding a mesh requires the batch to be updated again

    return true;
    // Note: 
    // Technically, we also need to refresh the batch and update the batch's buffers when adding a mesh,
    // however, we will defer this until SortBatch is called.
    // This is to allow multiple meshes to be added before sorting & refreshing the batch.
}

bool GraphicsManager::RemoveFromBatch(BatchIndex batchID, size_t meshID)
{
    if (batchID >= batches.size() ||
        meshID >= meshes.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[RenderSystem] RemoveFromBatch: Invalid batch or mesh ID");
        return false;
    }
    BatchData& batch = batches[batchID];

    auto it = std::find(batch.meshIDs.begin(), batch.meshIDs.end(), meshID);
    if (it != batch.meshIDs.end()) {
        batch.meshIDs.erase(it);
        batch.isUpdated = false;  // Removing a mesh requires the batch to be updated again

        meshes[meshID].batchID = NO_BATCH;
        // Note: Calling RemoveFromBatch tells the graphic manager that the mesh is no longer in the batch,
        // but the mesh information still exists in the meshes vector.
        return true;
    }
	return false;
}

void GraphicsManager::SortBatch(BatchData& batch)
{
    //if (batch.sorted) Logger::Instance().Log(Logger::Level::INFO, "[GraphicsManager] SortBatch: Called on a sorted batch");
    //std::cout << "Sorting batch" << std::endl;
    std::sort(batch.meshIDs.begin(), batch.meshIDs.end(), [&](size_t a, size_t b) {
        // This assumes that all the vertices of a mesh have the same z value
        return meshes[a].vertices[0].position.z < meshes[b].vertices[0].position.z;
        }
    );

    batch.isSorted = true;
    UpdateBatch(batch); // You will need to update the batch after sorting
}

void GraphicsManager::UpdateBatch(BatchData& batch)
{
    //if (batch.updated) Logger::Instance().Log(Logger::Level::INFO, "[GraphicsManager] RefreshBatch: Called on an updated batch");

    batch.vertices.clear();
    batch.indices.clear();

    for (auto& meshID : batch.meshIDs) {
		auto& mesh = meshes[meshID];
        unsigned int vertexOffset = static_cast<unsigned int>(batch.vertices.size());
        batch.vertices.insert(batch.vertices.end(), mesh.vertices.begin(), mesh.vertices.end());

        for (const auto& index : mesh.indices) {
			batch.indices.push_back(index + vertexOffset);
		}
	}
    batch.isUpdated = true;

    // This updates the batch's VAO, VBO, and EBO
    batch.UpdateBuffers();
}

void GraphicsManager::SetTextureToMesh(size_t meshID, int texArrayIndex, int texLayerIndex)
{
    size_t batchID = meshes[meshID].batchID;
    if (batchID >= batches.size()) {
		Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] SetTexture: Invalid batch ID");
		return;
	}
    batches[batchID].isUpdated = false;

    for (auto& vertex : meshes[meshID].vertices) {
        vertex.texArray = texArrayIndex;
        vertex.texLayer = texLayerIndex;
    }
}

void GraphicsManager::SetColorToMesh(size_t meshID, Vec4 color)
{
    size_t batchID = meshes[meshID].batchID;
    if (batchID >= batches.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] SetTexture: Invalid batch ID");
        return;
    }
    batches[batchID].isUpdated = false;

    for (auto& vertex : meshes[meshID].vertices) {
        vertex.color = color;
    }
}

void GraphicsManager::SetVisibilityToMesh(size_t meshID, bool visibility)
{
    size_t batchID = meshes[meshID].batchID;
    if (batchID >= batches.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] SetTexture: Invalid batch ID");
        return;
    }
    batches[batchID].isUpdated = false;

    for (auto& vertex : meshes[meshID].vertices) {
        vertex.visible = static_cast<int>(visibility);
    }
}

void GraphicsManager::SetBatchUpdateFlag(BatchIndex batchID, bool flag)
{
    if (batchID >= batches.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] ToggleBatchUpdate: Invalid batch ID");
        return;
    }
    batches[batchID].isUpdated = flag;
}

void GraphicsManager::SetBatchUpdateFlag(size_t meshID, bool flag)
{
    if (meshID >= meshes.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] ToggleBatchUpdate: Invalid mesh ID");
        return;
    }
    size_t& batchID = meshes[meshID].batchID;
    if (batchID >= batches.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] ToggleBatchSort: Invalid batch ID");
        return;
    }
    batches[batchID].isUpdated = flag;
}

void GraphicsManager::SetBatchSortFlag(BatchIndex batchID, bool flag)
{
    if (batchID >= batches.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] ToggleBatchSort: Invalid batch ID");
        return;
    }
    batches[batchID].isSorted = flag;
}

void GraphicsManager::SetBatchSortFlag(size_t meshID, bool flag)
{
    if (meshID >= meshes.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] ToggleBatchSort: Invalid mesh ID");
        return;
    }
    size_t& batchID = meshes[meshID].batchID;
    if (batchID >= batches.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[GraphicsManager] ToggleBatchSort: Invalid batch ID");
        return;
    }
    batches[batchID].isSorted = flag;
}

glm::mat4 GraphicsManager::GetViewMatrixGame()
{
    auto entityCam = ECSManager::GetInstance().TryGetComponent<Camera>(activeCamera);
    return (entityCam.has_value() ? entityCam->get().viewMtx : glm::mat4(1.0f));
}

glm::mat4 GraphicsManager::GetProjectionMatrixGame()
{
    auto entityCam = ECSManager::GetInstance().TryGetComponent<Camera>(activeCamera);
    return (entityCam.has_value() ? entityCam->get().projectionMtx : glm::mat4(1.0f));
}

glm::mat4 GraphicsManager::GetViewMatrixEngine()
{
    return camera.GetViewMatrix();
}

glm::mat4 GraphicsManager::GetProjectionMatrixEngine()
{
    return camera.GetProjectionMatrix();
}

void GraphicsManager::SetInternalFormat(std::string _internalFormat)
{
    if (_internalFormat == "High" || _internalFormat == "GL_RGBA8") {
		this->internalFormat = GL_RGBA8;
    }
    /*else if (internalFormat == "High" || internalFormat == "GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC") {
        this->internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
    }
    else if (internalFormat == "Medium" || internalFormat == "GL_COMPRESSED_RGBA8_ETC2_EAC") {
		this->internalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC;
	}*/
	else if (_internalFormat == "Low" || _internalFormat == "GL_RGBA4") {
		this->internalFormat = GL_RGBA4;
    }
    else {
        this->internalFormat = GL_RGBA8;
		Logger::Instance().Log(Logger::Level::ERR, 
            "[GraphicsManager] SetInternalFormat: Invalid / Unsupported internal format"
        );
    }
}

GLenum GraphicsManager::GetInternalFormat() const
{
	return internalFormat;
}