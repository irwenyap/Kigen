#include "LayersPanel.hpp"

#include "../../Layers/LayerManager.hpp"
#include "../../ECS/ECSManager.hpp"


//static std::array<bool, MAX_LAYERS* MAX_LAYERS> collisionMatrix = { false };

LayersPanel::LayersPanel()
{
	name = "Layers";
	show = true;
}

void LayersPanel::Update()
{
	ImGui::Begin(name.c_str(), nullptr);

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));

    ImGui::PushID("Layer Names");
    if (ImGui::CollapsingHeader("Layer Names")) {
        for (int i = 0; i < MAX_LAYERS; i++) {
            //char label[16];
            //snprintf(label, sizeof(label), "Layer %d", i);
            //ImGui::InputText(label, &layerNames[i][0], layerNames[i].capacity() + 1);
        }
    }
    ImGui::PopID();

    ImGui::PushID("Visibility");
    if (ImGui::CollapsingHeader("Visibility", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
        for (Layer i = 0; i < MAX_LAYERS; i++) {
            //char label[32];
            //snprintf(label, sizeof(label), "Layer %d: %s", i, LayerManager::GetInstance().layerNames[i].c_str());
            bool& isVisible = LayerManager::GetInstance().layerVisibility[i];
            if (ImGui::Checkbox(LayerManager::GetInstance().layerNames[i].c_str(), &isVisible)) {
                ECSManager::GetInstance().renderSystem->UpdateVisibilityLayer(i, isVisible);
            }
        }
    }
    ImGui::PopID();

    if (ImGui::CollapsingHeader("Collision")) {
        static Layer selectedLayer = 0;

        ImGui::PushID("Layer Selection");
        ImGui::Text("Layer Selection");
        ImGui::Separator();
        for (Layer i = 0; i < MAX_LAYERS; i++) {
            char buttonLabel[16];
            snprintf(buttonLabel, sizeof(buttonLabel), "Layer %d", i);

            if (ImGui::Button(buttonLabel))
                selectedLayer = i;

            // Wrap buttons into a grid
            if ((i + 1) % 4 != 0)
                ImGui::SameLine();
        }
        ImGui::PopID();

        ImGui::Separator();
        char selectedLayerText[64];
        if (selectedLayer < MAX_LAYERS)
            snprintf(selectedLayerText, sizeof(selectedLayerText), "Selected Layer %d: %s", selectedLayer, LayerManager::GetInstance().layerNames[selectedLayer].c_str());
        ImGui::Text(selectedLayerText);

        // Collision Matrix
        ImGui::PushID("Collision");
        for (Layer i = 0; i < MAX_LAYERS; i++) {
            char collisionLabel[16];
            snprintf(collisionLabel, sizeof(collisionLabel), "Layer %d", i);

            Layer index = static_cast<size_t>(selectedLayer * MAX_LAYERS + i);
            if (index < MAX_LAYERS * MAX_LAYERS) {
                bool* collisionFlag = &LayerManager::GetInstance().collisionMatrix[selectedLayer * MAX_LAYERS + i];
                if (ImGui::Checkbox(collisionLabel, collisionFlag)) {
                    // Toggle both directions in the matrix for symmetry
                    LayerManager::GetInstance().collisionMatrix[i * MAX_LAYERS + selectedLayer] = *collisionFlag;
                }
            }

            if ((i + 1) % 4 != 0)
                ImGui::SameLine();
        }
        ImGui::PopID();

        ImGui::NewLine();
        if (ImGui::Button("Collide With All")) {
            for (int i = 0; i < MAX_LAYERS; i++) {
                LayerManager::GetInstance().collisionMatrix[selectedLayer * MAX_LAYERS + i] = true;
                // Toggle both directions in the matrix for symmetry
                LayerManager::GetInstance().collisionMatrix[i * MAX_LAYERS + selectedLayer] = true;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Collide With None")) {
            for (int i = 0; i < MAX_LAYERS; i++) {
                LayerManager::GetInstance().collisionMatrix[selectedLayer * MAX_LAYERS + i] = false;
                // Toggle both directions in the matrix for symmetry
                LayerManager::GetInstance().collisionMatrix[i * MAX_LAYERS + selectedLayer] = false;
            }
        }
    }
    ImGui::PopStyleColor(3);
	ImGui::End();
}
