/*********************************************************************
 * \file	UISystem.cpp
 * \brief
 *      This file defines the UISystem class. The UISystem class is
 *		responsible for handling the UI & Textbox components of the
 *		entities.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	26 November 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#include "UISystem.hpp"
#include "../ECS/ECSManager.hpp"
#include "../Components/Textbox.hpp"
#include "../Components/UI.hpp"
#include "../Components/Renderer.hpp"
#include "Timer.hpp"
#include <string>
#include "../Scene/SceneManager.hpp"
#include "../AssetManager.hpp"


UISystem::UISystem()
{
}

UISystem::~UISystem()
{
}

void UISystem::Init()
{
    //// Set to false at the start to update all UI components
    //for (auto const& entity : m_entities) {
    //    auto& uiComponent = ECSManager::GetInstance().GetComponent<UI>(entity);
    //    uiComponent.isUpdated = false;
    //}

    // References for easy readability
    auto& graphicsManager = GraphicsManager::GetInstance();
    auto& ecsManager = ECSManager::GetInstance();

    SceneManager& sm = SceneManager::GetInstance();
    for (auto const& entity : m_entities) {
        auto& ui = ecsManager.GetComponent<UI>(entity);
        if (ui.isUpdated) continue;

        auto renderer0pt = ecsManager.TryGetComponent<Renderer>(entity);
        auto textbox0pt = ecsManager.TryGetComponent<Textbox>(entity);
        // Textbox has priority over renderer
        if (textbox0pt.has_value()) {
            SetTextbox(entity);
        }
        else if (renderer0pt.has_value()) {
            auto& renderer = renderer0pt->get();
            float sizeX = ui.size.x * 2 * ui.scale.x;
            float sizeY = ui.size.y * 2 * ui.scale.y;
            Vec3 pos = NormalisedScreenToNDC(ui.position);

            graphicsManager.meshes[renderer.currentMeshID].vertices[0].position =
                Vec3(pos.x, pos.y + sizeY, pos.z);

            graphicsManager.meshes[renderer.currentMeshID].vertices[1].position =
                Vec3(pos.x + sizeX, pos.y + sizeY, pos.z);

            graphicsManager.meshes[renderer.currentMeshID].vertices[2].position =
                Vec3(pos.x + sizeX, pos.y, pos.z);

            graphicsManager.meshes[renderer.currentMeshID].vertices[3].position =
                Vec3(pos.x, pos.y, pos.z);

            graphicsManager.SetBatchUpdateFlag(renderer.currentMeshID, false);
            graphicsManager.SetBatchSortFlag(renderer.currentMeshID, false);

        }
        ui.isUpdated = true;

        if (sm.useLoadingScreen && sm.isLoading && entity > sm.loadingScreenEntities.size() - 1) {
            SetVisibility(entity, false);
        }
    }
}

//static float frameSplit = 0.f;
void UISystem::Update(double)
{
    //frameSplit += dt;
    //std::cout << frameSplit << std::endl;
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
        auto& ui = ecsManager.GetComponent<UI>(entity);
        if (ui.isUpdated) continue;

        auto renderer0pt = ecsManager.TryGetComponent<Renderer>(entity);
        auto textbox0pt = ecsManager.TryGetComponent<Textbox>(entity);
        // Textbox has priority over renderer
        if (textbox0pt.has_value()) {
			SetTextbox(entity);
		}
        else if (renderer0pt.has_value()) {
            auto& renderer = renderer0pt->get();
            float sizeX = ui.size.x * 2 * ui.scale.x;
            float sizeY = ui.size.y * 2 * ui.scale.y;
            Vec3 pos = NormalisedScreenToNDC(ui.position);

            graphicsManager.meshes[renderer.currentMeshID].vertices[0].position =
				Vec3(pos.x, pos.y + sizeY, pos.z);

            graphicsManager.meshes[renderer.currentMeshID].vertices[1].position =
				Vec3(pos.x + sizeX, pos.y + sizeY, pos.z);

            graphicsManager.meshes[renderer.currentMeshID].vertices[2].position =
				Vec3(pos.x + sizeX, pos.y, pos.z);

            graphicsManager.meshes[renderer.currentMeshID].vertices[3].position =
				Vec3(pos.x, pos.y, pos.z);

            graphicsManager.SetBatchUpdateFlag(renderer.currentMeshID, false);
            graphicsManager.SetBatchSortFlag(renderer.currentMeshID, false);
        }
        ui.isUpdated = true;
    }

    // temp
    //for (auto const& entity : m_entities) {
    //    auto textboxComponent = ecsManager.TryGetComponent<Textbox>(entity);

    //    // Note: currently isUpdated bool does nothing
    //    if (textboxComponent.has_value()) {
    //        SetTextbox(entity);
			
    //        //textboxComponent->get().text = "FPS: " + std::to_string(Timer::GetFPS());
    //        //frameSplit = 0.f;
    //    }
    //}
}

void UISystem::Exit()
{
}

void UISystem::SetTextbox(Entity entity)
{
    auto ui0pt = ECSManager::GetInstance().TryGetComponent<UI>(entity);
	auto textbox0pt = ECSManager::GetInstance().TryGetComponent<Textbox>(entity);
	if (!textbox0pt.has_value() || !ui0pt.has_value()) return;

    // References for easier readability
    auto& textbox = textbox0pt->get();
    auto& ui = ui0pt->get();
    auto& graphicsManager = GraphicsManager::GetInstance();

    if (textbox.text.empty()) {
        // If the textbox is empty, hide all its meshes
        for (size_t i = 0; i < textbox.meshIDs.size(); ++i) {
            graphicsManager.SetVisibilityToMesh(textbox.meshIDs[i], false);
        }
        return;
    }

	// Create local copies of the variables
    float scale = ui.scale.x;
    Vec3 pos = ui.position;
    std::string text = textbox.text;
	std::string fontUUID = textbox.fontUUID;
    Vec3 color = textbox.color;

    std::string::iterator c;

	auto font = AssetManager::GetInstance().Get<Font>(fontUUID);

    // Scale the text to fit the screen
    static const float baseWidth = 1920.0f;
    static const float baseHeight = 1080.0f;
    auto [screenWidth, screenHeight] = Application::GetWindowSize();

    screenWidth = 1920; screenHeight = 1080; // temp

    float relativeScaleX = screenWidth / baseWidth;
    float relativeScaleY = screenHeight / baseHeight;
    float relativeScale = (relativeScaleX < relativeScaleY) ? relativeScaleX : relativeScaleY;
    scale *= relativeScale;

    pos.x *= screenWidth;
    pos.y *= screenHeight;

	float totalWidth = 0.f;

    size_t freeMeshIndex = 0;
    std::vector<size_t> newMeshIDs;
    for (c = text.begin(); c != text.end(); ++c) {
        if (*c < 0 || *c > 127) continue;
        FontCharacter& ch = font->characters[*c];

        float xpos = pos.x + ch.bearing.x * scale;
        float ypos = pos.y - (ch.size.y - ch.bearing.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        w = (2.0f * w / screenWidth);
        h = (2.0f * h / screenHeight);

        // Convert screen coordinates to normalized device coordinates (NDC)
        xpos = (2.0f * xpos / screenWidth) - 1.0f;
        ypos = (2.0f * ypos / screenHeight) - 1.0f;

        //size_t meshID = graphicsManager.LoadTextCharacterMesh();

        size_t meshID;
        // If the textbox already has assigned meshIDs, reuse them
        if (freeMeshIndex < textbox.meshIDs.size()) {
			meshID = textbox.meshIDs[freeMeshIndex++];
            graphicsManager.SetVisibilityToMesh(meshID, true);
		}
        // Otherwise, create a new mesh
		else {
			meshID = graphicsManager.LoadTextCharacterMesh();
            newMeshIDs.push_back(meshID);
		}

        SetCharacterToMesh(meshID, fontUUID, *c);

        auto& mesh = graphicsManager.meshes[meshID]; mesh;

        graphicsManager.meshes[meshID].vertices[0].position = Vec3(xpos, ypos + h, pos.z);
        graphicsManager.meshes[meshID].vertices[1].position = Vec3(xpos + w, ypos + h, pos.z);
        graphicsManager.meshes[meshID].vertices[2].position = Vec3(xpos + w, ypos, pos.z);
        graphicsManager.meshes[meshID].vertices[3].position = Vec3(xpos, ypos, pos.z);

        float texWidth = ch.size.x / font->maxGlyphWidth;
        float texHeight = ch.size.y / font->maxGlyphHeight;

        graphicsManager.meshes[meshID].vertices[0].texCoord = Vec2(0.0f, texHeight);
        graphicsManager.meshes[meshID].vertices[1].texCoord = Vec2(texWidth, texHeight);
        graphicsManager.meshes[meshID].vertices[2].texCoord = Vec2(texWidth, 0.0f);
        graphicsManager.meshes[meshID].vertices[3].texCoord = Vec2(0.0f, 0.0f);

        graphicsManager.SetColorToMesh(meshID, Vec4(color, 1.f));

        graphicsManager.SetBatchUpdateFlag(meshID);

		float increment = (ch.advance >> 6) * scale;
        pos.x += increment;
		totalWidth += increment;
    }	

	// If there are more meshIDs than needed, hide the excess
    for (size_t i = freeMeshIndex; i < textbox.meshIDs.size(); ++i) {
        graphicsManager.SetVisibilityToMesh(textbox.meshIDs[i], false);
		//graphicsManager.RemoveFromBatch(GraphicsManager::BatchIndex::UI_TEXT_BATCH, textbox.meshIDs[i]);
	}
    // If there are new meshIDs created, add them to the textbox's meshIDs
    if (!newMeshIDs.empty()) {
        textbox.meshIDs.insert(textbox.meshIDs.end(), newMeshIDs.begin(), newMeshIDs.end());
		graphicsManager.SetBatchUpdateFlag(GraphicsManager::BatchIndex::UI_TEXT_BATCH, true);
    }

	if (textbox.centerAligned) {
		totalWidth = (2.0f * totalWidth) / screenWidth; // Convert to NDC
		// Center the text
		for (size_t i = 0; i < textbox.meshIDs.size(); ++i) {
			graphicsManager.meshes[textbox.meshIDs[i]].vertices[0].position.x -= totalWidth / 2.f;
			graphicsManager.meshes[textbox.meshIDs[i]].vertices[1].position.x -= totalWidth / 2.f;
			graphicsManager.meshes[textbox.meshIDs[i]].vertices[2].position.x -= totalWidth / 2.f;
			graphicsManager.meshes[textbox.meshIDs[i]].vertices[3].position.x -= totalWidth / 2.f;
		}
	}
}

void UISystem::SetCharacterToMesh(size_t meshID, std::string fontUUID, char character)
{
    auto& graphicsManager = GraphicsManager::GetInstance();
    if (meshID >= graphicsManager.meshes.size()) return;

	auto font = AssetManager::GetInstance().Get<Font>(fontUUID);

	// Ensure the font and character are valid
	if (font == nullptr) {
		graphicsManager.SetTextureToMesh(meshID, -1, -1);
		Logger::Instance().Log(Logger::Level::ERR, "[UISystem] SetCharacterToMesh: Invalid font ID");
		return;
	}
	if (character < 0 || character > 127) {
		graphicsManager.SetTextureToMesh(meshID, -1, -1);
		Logger::Instance().Log(Logger::Level::ERR, "[UISystem] SetCharacterToMesh: Invalid character ID");
		return;
	}

	const auto& fontChar = font->characters[character];

    // Set the texture array index and layer index for the mesh
    graphicsManager.SetTextureToMesh(
        meshID,
        static_cast<int>(font->texArrayIndex),
        static_cast<int>(fontChar.texLayerIndex)
    );
    graphicsManager.SetVisibilityToMesh(
        meshID,
        true
    );
}

void UISystem::SetPosition(Entity entity, Vec3 position)
{
	auto ui = ECSManager::GetInstance().TryGetComponent<UI>(entity);
    if (!ui.has_value()) return;

    ui->get().position = position;
    ui->get().isUpdated = false;
}

void UISystem::SetScale(Entity entity, Vec2 scale)
{
	auto ui = ECSManager::GetInstance().TryGetComponent<UI>(entity);
	if (!ui.has_value()) return;

    ui->get().scale = scale;
    ui->get().isUpdated = false;
}

void UISystem::SetSize(Entity entity, Vec2 size)
{
	auto ui = ECSManager::GetInstance().TryGetComponent<UI>(entity);
	if (!ui.has_value()) return;

    ui->get().size = size;
    ui->get().isUpdated = false;
}

void UISystem::SetText(Entity entity, std::string text)
{
	auto textbox = ECSManager::GetInstance().TryGetComponent<Textbox>(entity);
	auto ui = ECSManager::GetInstance().TryGetComponent<UI>(entity);
	if (!textbox.has_value() || !ui.has_value()) return;

	textbox->get().text = text;
    ui->get().isUpdated = false;
}

void UISystem::SetFont(Entity entity, std::string fontUUID)
{
	auto textbox = ECSManager::GetInstance().TryGetComponent<Textbox>(entity);
	auto ui = ECSManager::GetInstance().TryGetComponent<UI>(entity);
	if (!textbox.has_value() || !ui.has_value()) return;

	textbox->get().fontUUID = fontUUID;
    ui->get().isUpdated = false;
}

void UISystem::SetColor(Entity entity, Vec3 color)
{
	auto textbox = ECSManager::GetInstance().TryGetComponent<Textbox>(entity);
    auto ui = ECSManager::GetInstance().TryGetComponent<UI>(entity);
	if (!textbox.has_value() || !ui.has_value()) return;

    textbox->get().color = color;
    ui->get().isUpdated = false;
}

void UISystem::SetVisibility(Entity entity, bool visible)
{
    auto ui = ECSManager::GetInstance().TryGetComponent<UI>(entity);
    if (!ui.has_value()) return;

    auto renderer = ECSManager::GetInstance().TryGetComponent<Renderer>(entity);
    auto textbox = ECSManager::GetInstance().TryGetComponent<Textbox>(entity);

    if (textbox.has_value()) {
        for (auto const& meshID : textbox->get().meshIDs) {
            GraphicsManager::GetInstance().SetVisibilityToMesh(meshID, visible);
		}
	}
    else if (renderer.has_value()) {
        GraphicsManager::GetInstance().SetVisibilityToMesh(renderer->get().currentMeshID, visible);
	}
}

void UISystem::SetCenterAlignment(Entity entity, bool centerAligned)
{
	auto& ui = ECSManager::GetInstance().GetComponent<UI>(entity);
	auto textbox = ECSManager::GetInstance().TryGetComponent<Textbox>(entity);
	if (!textbox.has_value()) return;

    textbox->get().centerAligned = centerAligned;
	ui.isUpdated = false;
}

Vec3 UISystem::NormalisedScreenToNDC(Vec3 screenPos)
{
    return Vec3(
        (2.0f * screenPos.x) - 1.0f,
		(2.0f * screenPos.y) - 1.0f,
		screenPos.z // Z value is not changed
    );
}