/*********************************************************************
 * \file	RenderSystem.cpp
 * \brief
 *      This file defines the `RenderSystem` class, which is responsible
 *      for managing rendering operations in a graphics application. It includes
 *      functionalities for handling meshes, textures, animations, and background
 *      settings, while integrating with a graphics manager for rendering.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \author	Wong Woon Li, woonli.wong, 2301308
 * \email   woonli.wong@digipen.edu
 * \date	5 November 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#include "RenderSystem.hpp"
#include <string>

#include "Math.hpp"
//#include "Animation.hpp"
#include "GraphicsManager.hpp"
#include "../Application.hpp"
#include "../ECS/ECSManager.hpp"

#include "Logger.hpp"
// Components
#include "../Components/Transform.hpp"
#include "../Components/Renderer.hpp"
#include "../Components/Animation.hpp"
#include "../Components/UI.hpp"
#include "../Components/Textbox.hpp"

#include "../Input/InputManager.hpp"

#include "../AssetManager.hpp"
#include "../Scene/SceneManager.hpp"

RenderSystem::RenderSystem()
{
    
}

RenderSystem::~RenderSystem()
{
    Exit();
}

void RenderSystem::Init() {
    // IMPORTANT: Call this only after all meshes/models have been added
    // Check that GraphicsManager hasn't been initialized yet.
    // It may have been initialized by InitLoadingScreen() if there is a loading screen for the current scene.
    if (!isGMInitialized) {
        GraphicsManager::GetInstance().Init();
        isGMInitialized = true;
    }
    //if (!runOnce) {
    //    // Load fonts
    //    LoadFont("19362e7adb5-f5d82ff0869c4802-662bdabeda82c652"); // Exo2-Regular assigned app font ID 0
    //    LoadFont("19362e7addd-433dbcd9e6704b6b-d989932085e7eb53"); // Pixellari assigned app font ID 1
    //    runOnce = true;
    //}
    LoadFont("19362e7adb5-f5d82ff0869c4802-662bdabeda82c652"); // Exo2-Regular assigned app font ID 0
    LoadFont("19362e7addd-433dbcd9e6704b6b-d989932085e7eb53"); // Pixellari assigned app font ID 1

    // Load textures
    // IMPORTANT: Load any textures before adding meshes that use them
    //auto& textureArray = Texture::GetTextureArray();
    //auto& graphicsManager = GraphicsManager::GetInstance();

    // To load from file, either
    // Complete GraphicsManager::LoadMesh(std::string const& meshPath) and call:
    // AddMesh(RenderSystem::MeshType::LOAD_FROM_FILE, "../Assets/quad.obj");
    // Or, edit the switch case for LOAD_FROM_FILE in RenderSystem::AddMesh() to read the file and call:
    // GraphicsManager::LoadMesh(std::vector<Vertex> const& vertices, std::vector<unsigned int> const& indices)
    // _________________________________________________________________________________________________________

    SceneManager& sm = SceneManager::GetInstance();
    if (sm.isLoading) {
        // Loading bar variables.
        // Visually updates the loading bar every few entities initialized.
        size_t entitiesToLoad = m_entities.size();
        size_t entitiesLoaded = 0;
        size_t updateInterval = 50; // Update loading bar every 20 entities.
        float incrementPerUpdate = (float)updateInterval / (float)entitiesToLoad * sm.incrementPerSystemLoaded; // How much to increase the loading bar size by each update.
        float currentPercent = (float)(sm.numSystemsLoaded) * sm.incrementPerSystemLoaded;
        for (auto const& entity : m_entities) {
            auto& renderer = ECSManager::GetInstance().GetComponent<Renderer>(entity);
            if (!renderer.isInitialized) {
                renderer.isInitialized = true;
                std::pair<size_t, size_t> mesh = AddMesh(renderer.mesh, static_cast<size_t>(renderer.sortingLayer));
                renderer.currentMeshID = mesh.first;
                renderer.currentMeshDebugID = mesh.second;
                SetTextureToMesh(mesh.first, renderer.uuid);

                // If there is a loading screen for the current scene,
                // Set all the entities to invisible first so that only the loading screen is visible when loading.
                if (sm.useLoadingScreen ||
                    (!sm.useLoadingScreen && !ECSManager::GetInstance().GetEntityManager().GetActive(entity)))
                {
                    SetVisibility(mesh.first, false);
                }

                SetColorToEntity(entity, EncodeColor(entity));
            }

            // Update the loading screen.
            if (++entitiesLoaded % updateInterval == 0) {
                currentPercent += incrementPerUpdate;
                sm.UpdateLoadingScreen(currentPercent);
            }
        }

        for (auto& batch : GraphicsManager::GetInstance().batches) {
            // On init, sort the batches
            GraphicsManager::GetInstance().SortBatch(batch);
        }
    }
    else {
        for (auto const& entity : sm.loadingScreenEntities) {
            auto& renderer = ECSManager::GetInstance().GetComponent<Renderer>(entity);
            if (!renderer.isInitialized) {
                renderer.isInitialized = true;
                std::pair<size_t, size_t> mesh = AddMesh(renderer.mesh, static_cast<size_t>(renderer.sortingLayer));
                renderer.currentMeshID = mesh.first;
                renderer.currentMeshDebugID = mesh.second;
                SetTextureToMesh(mesh.first, renderer.uuid);
                if (!ECSManager::GetInstance().GetEntityManager().GetActive(entity))
                    SetVisibility(mesh.first, false);
                SetColorToEntity(entity, EncodeColor(entity));
            }
        }

        for (auto& batch : GraphicsManager::GetInstance().batches) {
            // On init, sort the batches
            GraphicsManager::GetInstance().SortBatch(batch);
        }
    }
}

void RenderSystem::UpdateEntitiesVisibility() {
    for (auto const& entity : m_entities) {
        SetVisibility(entity, ECSManager::GetInstance().GetEntityManager().GetActive(entity));
    }
}

void RenderSystem::Update()
{
    // References for easy readability
    auto& graphicsManager = GraphicsManager::GetInstance();
    auto& ecsManager = ECSManager::GetInstance();

    SceneManager& sm = SceneManager::GetInstance();
    std::set<Entity> entities = m_entities;
    // If scene manager is still loading the current scene
    if (sm.isLoading) {
        entities = sm.loadingScreenEntities;
    }

    for (auto const& entity : entities) {

        auto& renderer = ecsManager.GetComponent<Renderer>(entity);
        if (renderer.isInitialized) {
            auto transform0pt = ecsManager.TryGetComponent<Transform>(entity);
            auto ui0pt = ecsManager.TryGetComponent<UI>(entity);

            // Entity is a world object, update its mesh based on the transform component
            // If the entity has a UI component, its mesh is already updated by the UI system
            if (transform0pt.has_value() && !ui0pt.has_value()) {
                auto& transform = transform0pt->get(); // Reference to improve readability
                if (renderer.isDirty || renderer.isAnimated) { // Only update the mesh if the transform component has been updated
                    if (graphicsManager.meshes[renderer.currentMeshID].vertices.size() == graphicsManager.meshes[renderer.currentMeshID].modelSpacePosition.size()) {
                        for (size_t i = 0; i < graphicsManager.meshes[renderer.currentMeshID].vertices.size(); ++i) {
                            graphicsManager.meshes[renderer.currentMeshID].vertices[i].position =
                                transform.modelToWorldMtx * graphicsManager.meshes[renderer.currentMeshID].modelSpacePosition[i];
                        }
                        graphicsManager.SetBatchUpdateFlag(renderer.currentMeshID, false);
                        transform.updated = false; // Reset the updated flag
                    }
                    graphicsManager.SetBatchUpdateFlag(renderer.currentMeshID, false);
                    renderer.isDirty = false;
                    //transform.updated = false; // Reset the updated flag
                }
            }

            if (graphicsManager.debugMode) {
                graphicsManager.RefreshMeshCollision(renderer.currentMeshID, renderer.currentMeshDebugID, entity);
            }

            // If the renderer's sorting layer was changed
            if (renderer.sortingLayerChanged) {
                renderer.sortingLayerChanged = false;
                // Move the renderer's mesh from the previous sorting layer batch to the current sorting layer batch.
                graphicsManager.RemoveFromBatch(static_cast<GraphicsManager::BatchIndex>(renderer.prevSortingLayer), renderer.currentMeshID);
                graphicsManager.AddToBatch(static_cast<GraphicsManager::BatchIndex>(renderer.sortingLayer), renderer.currentMeshID);
            }
        }
    }

    // Sort the batches if they are not sorted
    for (auto& batch : graphicsManager.batches) {
        

        if (!batch.isSorted) graphicsManager.SortBatch(batch);
        if (!batch.isUpdated) graphicsManager.UpdateBatch(batch);

        if (batch.id == GraphicsManager::BatchIndex::UI_TEXT_BATCH) {
            std::cout << "";
            batch.UpdateBuffers();
        }
    }

    graphicsManager.Render();
} 

void RenderSystem::Exit() {
    for (auto& batch : GraphicsManager::GetInstance().batches) {
		batch.isSorted = false;
        batch.isUpdated = false;
	}
    GraphicsManager::GetInstance().Exit();
    isGMInitialized = false;
}

std::pair<size_t, size_t> RenderSystem::AddMesh(MeshType mtype, std::string const& path, std::vector<Vertex> const& vertices)
{
    // This function returns the ID of the main mesh and the ID of the collision box mesh respectively
    std::pair<size_t, size_t> id;

    switch (mtype) {
    case MeshType::LOAD_FROM_FILE:
        id.first = GraphicsManager::GetInstance().LoadMesh(path);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    case MeshType::TRIANGLE:
        id.first = GraphicsManager::GetInstance().LoadTriangleMesh(0, vertices);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    case MeshType::LINE:
        id.first = GraphicsManager::GetInstance().LoadLineMesh(0, vertices);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    case MeshType::QUAD:
        id.first = GraphicsManager::GetInstance().LoadQuadMesh(0, vertices);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    case MeshType::SPHERE:
        id.first = GraphicsManager::GetInstance().LoadSphereMesh(0, vertices);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    case MeshType::TEXTCHAR:
        id.first = GraphicsManager::GetInstance().LoadTextCharacterMesh(GraphicsManager::BatchIndex::UI_TEXT_BATCH, vertices);
        id.second = static_cast<size_t>(-1);
        break;
    case MeshType::QUAD_UI:
        id.first = GraphicsManager::GetInstance().LoadQuadMesh(GraphicsManager::BatchIndex::UI_TEXTURE_BATCH, vertices);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    case MeshType::VIDEO_UI:
        id.first = GraphicsManager::GetInstance().LoadQuadMesh(GraphicsManager::BatchIndex::UI_VIDEO_TEXTURE_BATCH, vertices);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    }

    return id;
}

std::pair<size_t, size_t> RenderSystem::AddMesh(int mtype, size_t batchID, std::string const& path, std::vector<Vertex> const& vertices)
{
    // This function returns the ID of the main mesh and the ID of the collision box mesh respectively
    std::pair<size_t, size_t> id;

    switch (mtype) {
    case MeshType::LOAD_FROM_FILE:
        id.first = GraphicsManager::GetInstance().LoadMesh(path);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    case MeshType::TRIANGLE:
        id.first = GraphicsManager::GetInstance().LoadTriangleMesh(batchID, vertices);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    case MeshType::LINE:
        id.first = GraphicsManager::GetInstance().LoadLineMesh(batchID, vertices);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    case MeshType::QUAD:
        id.first = GraphicsManager::GetInstance().LoadQuadMesh(batchID, vertices);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    case MeshType::SPHERE:
        id.first = GraphicsManager::GetInstance().LoadSphereMesh(batchID, vertices);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    case MeshType::TEXTCHAR:
        id.first = GraphicsManager::GetInstance().LoadTextCharacterMesh(GraphicsManager::BatchIndex::UI_TEXT_BATCH, vertices);
        id.second = static_cast<size_t>(-1);
        break;
    case MeshType::QUAD_UI:
        id.first = GraphicsManager::GetInstance().LoadQuadMesh(GraphicsManager::BatchIndex::UI_TEXTURE_BATCH, vertices);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    case MeshType::VIDEO_UI:
        id.first = GraphicsManager::GetInstance().LoadQuadMesh(GraphicsManager::BatchIndex::UI_VIDEO_TEXTURE_BATCH, vertices);
        id.second = GraphicsManager::GetInstance().LoadMeshCollision(id.first);
        break;
    }

    return id;
}

Mesh& RenderSystem::GetMesh(size_t id)
{
	return id >= GraphicsManager::GetInstance().meshes.size() ? GraphicsManager::GetInstance().meshes.back() : GraphicsManager::GetInstance().meshes[id];
}

size_t RenderSystem::GetID(Mesh& mesh)
{
	return mesh.id;
}

void RenderSystem::SetTextureToMesh(size_t meshID, std::string texID)
{
    //if (meshID >= GraphicsManager::GetInstance().meshes.size() || 
    //    GraphicsManager::GetInstance().textures.find(texID) == GraphicsManager::GetInstance().textures.end()) {
    //    //Logger::Instance().Log(Logger::Level::ERR, "[RenderSystem] SetTextureToMesh: Invalid mesh or texture ID");
    //    return;
    //}

    //GraphicsManager::GetInstance().meshes[meshID].SetTexture(
    //    static_cast<int>(GraphicsManager::GetInstance().textures[texID].texArrayIndex),
    //    static_cast<int>(GraphicsManager::GetInstance().textures[texID].texLayerIndex)
    //);

    if (meshID >= GraphicsManager::GetInstance().meshes.size()) {
        //Logger::Instance().Log(Logger::Level::ERR, "[RenderSystem] SetTextureToMesh: Invalid mesh or texture ID");
        return;
    }

	int texArrayIndex = AssetManager::GetInstance().Get<Texture>(texID) == nullptr ?
		-1 : static_cast<int>(AssetManager::GetInstance().Get<Texture>(texID)->texArrayIndex);

	int texLayerIndex = AssetManager::GetInstance().Get<Texture>(texID) == nullptr ?
		-1 : static_cast<int>(AssetManager::GetInstance().Get<Texture>(texID)->texLayerIndex);

    GraphicsManager::GetInstance().SetTextureToMesh(
        meshID, 
        texArrayIndex,
        texLayerIndex
    );
}

void RenderSystem::SetTextureToEntity(Entity entity, std::string texID)
{
    if (AssetManager::GetInstance().Get<Texture>(texID) == nullptr) return;
    auto& renderer = ECSManager::GetInstance().GetComponent<Renderer>(entity);
    SetTextureToMesh(renderer.currentMeshID, texID);
    renderer.uuid = texID;
}

void RenderSystem::SetColorToMesh(size_t const& meshID, Vec4 color) 
{
    if (meshID >= GraphicsManager::GetInstance().meshes.size()) return;
    GraphicsManager::GetInstance().SetColorToMesh(meshID, color);
}

void RenderSystem::SetColorToEntity(Entity entity, Vec4 color)
{
    auto& renderer = ECSManager::GetInstance().GetComponent<Renderer>(entity);
    SetColorToMesh(renderer.currentMeshID, color);
}

void RenderSystem::SetDebugMode(bool val)
{
   GraphicsManager::GetInstance().debugMode = val;
}

void RenderSystem::SetVisibility(size_t const& meshID, bool val)
{
	if (meshID >= GraphicsManager::GetInstance().meshes.size()) {
		std::cout << "Invalid mesh ID" << std::endl;
		return;
	}
    GraphicsManager::GetInstance().SetVisibilityToMesh(meshID, val);
}

void RenderSystem::SetVisibility(Entity entity, bool val)
{
	auto& renderer = ECSManager::GetInstance().GetComponent<Renderer>(entity);
	SetVisibility(renderer.currentMeshID, val);
}

void RenderSystem::UpdateVisibilityLayer(uint8_t layer, bool visible)
{
    for (auto& entt : m_entities) {
        if (ECSManager::GetInstance().GetEntityManager().GetLayer(entt) != layer) continue;

        auto& r = ECSManager::GetInstance().GetComponent<Renderer>(entt);
        SetVisibility(r.currentMeshID, visible);
    }
}

void RenderSystem::LoadFont(std::string const& path)
{
	// Early exit if the font path is empty
	if (path.empty()) {
		Logger::Instance().Log(Logger::Level::ERR, "[RenderSystem] LoadFont: Invalid font path");
		return;
	}
	// Load the font using the GraphicsManager
	if (!GraphicsManager::GetInstance().LoadFont(path)) {
		Logger::Instance().Log(Logger::Level::ERR, "[RenderSystem] LoadFont: Failed to load font");
	}
}

Vec4 RenderSystem::EncodeColor(Entity entity) 
{
    unsigned char r = static_cast<unsigned char>((entity & 0xFF0000) >> 16);
    unsigned char g = static_cast<unsigned char>((entity & 0x00FF00) >> 8);
    unsigned char b = static_cast<unsigned char>(entity & 0x0000FF);

    return Vec4(r / 255.f, g / 255.f, b / 255.f, 1.f);
}

Entity RenderSystem::DecodeColor(Vec4 color)
{
	unsigned char r = static_cast<unsigned char>(color.r * 255);
	unsigned char g = static_cast<unsigned char>(color.g * 255);
	unsigned char b = static_cast<unsigned char>(color.b * 255);

	return (r << 16) | (g << 8) | b;
}

Entity RenderSystem::GetClickedEntity(int fbo) 
{
    auto& graphicsManager = GraphicsManager::GetInstance();

	if (fbo >= graphicsManager.frameBuffers.size()) {
		return static_cast<Entity>(-1);
	}

    int mouseX = static_cast<int>(InputManager::GetInstance().GetMouseX());
    int mouseY = static_cast<int>(InputManager::GetInstance().GetMouseY());

	// Note: Currently picks from engine's OBJ_PICKING_ENGINE frame buffer
    Vec4 pixelColor = graphicsManager.GetPixelColor(
        graphicsManager.frameBuffers[fbo], mouseX, mouseY);

    return DecodeColor(pixelColor);
}
