/*********************************************************************
 * \file		AssetsBrowserPanel.cpp
 * \brief		Defines a AssetsBrowserPanel class that manages
 *				everything related to the assets in the workspace.
 *
 * \author		wengkhong.thang, 2301372 (wengkhong.thang@digipen.edu)
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \author		irwinjun.l, 2301305 (irwinjun.l@digipen.edu)
 * \date		08 November 2024

 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#include "AssetBrowserPanel.hpp"

#include "../Engine/Scene/SceneManager.hpp"
#include "../Engine/Utility/MetadataHandler.hpp"
#include "../Engine/Utility/Serializer.hpp"
#include "../Engine/ECS/ECSManager.hpp"
#include "../Engine/Components/Name.hpp"
#include "../../AssetManager.hpp"

#include <fstream>

static float leftPaneWidth = 200.0f;  // Width of folder tree
static float split_min = 100.0f;   // Minimum size
static float split_max = 500.0f;   // Maximum size

AssetBrowserPanel::AssetBrowserPanel() : fileTexViewID(0), folderTexViewID(0), prefabTexViewID(0), sceneTexViewID(0) {
	name = "Assets Browser";
	show = true;

	baseDirectory = "../Assets";
	currentDirectory = baseDirectory;
}

void AssetBrowserPanel::Init() {
	auto& fileTex = *AssetManager::GetInstance().Load<Texture>("../Library/Icons/icon_file.png", true);
	auto& folderTex = *AssetManager::GetInstance().Load<Texture>("../Library/Icons/icon_folder.png", true);
	auto& prefabTex = *AssetManager::GetInstance().Load<Texture>("../Library/Icons/icon_prefab.png", true);
	auto& sceneTex = *AssetManager::GetInstance().Load<Texture>("../Library/Icons/icon_scene.png", true);

	auto& textureArray = Texture::GetTextureArray();
	fileTexViewID = GraphicsManager::GetInstance().CreateTextureView(textureArray[fileTex.texArrayIndex].id_gl, (int)fileTex.texLayerIndex);
	folderTexViewID = GraphicsManager::GetInstance().CreateTextureView(textureArray[folderTex.texArrayIndex].id_gl, (int)folderTex.texLayerIndex);
	prefabTexViewID = GraphicsManager::GetInstance().CreateTextureView(textureArray[prefabTex.texArrayIndex].id_gl, (int)prefabTex.texLayerIndex);
	sceneTexViewID = GraphicsManager::GetInstance().CreateTextureView(textureArray[sceneTex.texArrayIndex].id_gl, (int)sceneTex.texLayerIndex);
}

void AssetBrowserPanel::DisplayFolderTree(const std::filesystem::path& folderPath)
{
	if (currentDirectory != std::filesystem::path(baseDirectory)) {
		if (ImGui::Button("<-")) {
			currentDirectory = currentDirectory.parent_path();
		}
	}

	if (ImGui::TreeNode(folderPath.filename().string().c_str())) {
		for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
			if (entry.is_directory()) {
				//currentDirectory = entry;
				DisplayFolderTree(entry.path());
			} else {
				ImGui::Text(entry.path().filename().string().c_str());
			}
		}
		ImGui::TreePop();
	}
}

void AssetBrowserPanel::DisplayFilesInFolder(const std::filesystem::path& folderPath) {
	static float padding = 16.0f;
	static float thumbnailSize = 128.0f;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::Columns(columnCount, 0, false);

	for (auto& directoryEntry : std::filesystem::directory_iterator(folderPath)) {
		const auto& path = directoryEntry.path();

		if (path.extension() == ".meta" || path.extension() == ".temp")
			continue;

		std::string filenameString = path.filename().string();
		ImGui::PushID(filenameString.c_str());

		if (directoryEntry.is_directory()) {
			ImGui::ImageButton("##btn", (ImTextureID)(intptr_t)folderTexViewID, { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				selectedAssetPath = path;
			}
		} else if (path.extension() == ".prefab") {
			ImGui::ImageButton("##btn", (ImTextureID)(intptr_t)prefabTexViewID, { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				selectedAssetPath = path;
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				// Set payload
				ImGui::SetDragDropPayload("PREFAB", path.string().c_str(), path.string().size());
				ImGui::Text("Dragging: %s", path.string().c_str());
				ImGui::EndDragDropSource();
			}
		} else if (path.extension() == ".scene") {
			ImGui::ImageButton("##btn", (ImTextureID)(intptr_t)sceneTexViewID, { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				selectedAssetPath = path;
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				selectedScenePath = path.string();
				showPopup = true;
			}
		} else if (!directoryEntry.is_directory() && (path.extension() == ".png")) {
			//MetadataHandler::GenerateMetaFile(path.string());

			//GraphicsManager::GetInstance().LoadTexture(path.string());

			// Get UUID
			std::string uuid = MetadataHandler::ParseUUIDFromMeta(path.string() + ".meta");

#ifdef DISPLAY_TEXTURE_ICONS
			//GraphicsManager::GetInstance().textures[uuid];

			//for (const auto& [uuid, texture] : GraphicsManager::GetInstance().textures) {
			ImGui::PushID(uuid.c_str()); // Use the UUID as a unique ID for the ImGui context

			// Display the texture as a button or image
			//if (ImGui::ImageButton((void*)(intptr_t)texture.id, ImVec2(64, 64))) {
				// You can handle clicks here if needed
			//}
			ImGui::ImageButton("##btn", (void*)(intptr_t)GraphicsManager::GetInstance().textures[uuid].GetID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
#endif // !DISPLAY_TEXTURE_ICONS

			ImGui::ImageButton("##btn", (ImTextureID)(intptr_t)fileTexViewID, { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				selectedAssetPath = path;
			}

			// Make the texture draggable
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				// Set payload
				ImGui::SetDragDropPayload("TEXTURE_UUID", uuid.c_str(), uuid.size());
				ImGui::Text("Dragging: %s", uuid.c_str());
				ImGui::EndDragDropSource();
			}
		} else if (!directoryEntry.is_directory() && (path.extension() == ".wav" || path.extension() == ".ogg")) {
			ImGui::ImageButton("##btn", (ImTextureID)(intptr_t)fileTexViewID, { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				selectedAssetPath = path;
			}

			MetadataHandler::GenerateMetaFile(path.string());
			//GraphicsManager::GetInstance().LoadTexture(path.string());

			// Get UUID
			std::string uuid = MetadataHandler::ParseUUIDFromMeta(path.string() + ".meta");

			// Make the audio draggable
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				// Set payload
				ImGui::SetDragDropPayload("AUDIO_UUID", uuid.c_str(), uuid.size());
				ImGui::Text("Dragging: %s", uuid.c_str());
				ImGui::EndDragDropSource();
			}
		} else if (!directoryEntry.is_directory() && (path.extension() == ".mpg")) {
			std::string uuid = MetadataHandler::ParseUUIDFromMeta(path.string() + ".meta");

			ImGui::ImageButton("##btn", (ImTextureID)(intptr_t)fileTexViewID, { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				selectedAssetPath = path;
			}

			// Make the texture draggable
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				// Set payload
				ImGui::SetDragDropPayload("VIDEO_UUID", uuid.c_str(), uuid.size());
				ImGui::Text("Dragging: %s", uuid.c_str());
				ImGui::EndDragDropSource();
			}
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			if (directoryEntry.is_directory())
				currentDirectory /= path.filename();
		}

		ImGui::TextWrapped(filenameString.c_str());
		ImGui::NextColumn();
		ImGui::PopID();
	}

	ImGui::Columns(1);

	ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
	ImGui::SliderFloat("Padding", &padding, 0, 32);
}

//Drag drop function implementation
void AssetBrowserPanel::RegisterDropCallback(GLFWwindow* window)
{
	glfwSetDropCallback(window, DropCallback);
}

void AssetBrowserPanel::RefreshAssets() {
	if (std::filesystem::exists(currentDirectory)) {
		std::cout << "Assets refreshed in: " << currentDirectory << std::endl;
	} else {
		if (currentDirectory != baseDirectory) {
			currentDirectory = currentDirectory.parent_path();
			std::cout << "Current directory invalid, resetting to parent directory: " << currentDirectory << std::endl;
		} else {
			currentDirectory = baseDirectory;
			std::cout << "Current directory invalid, resetting to base directory: " << baseDirectory << std::endl;
		}
	}
}

void AssetBrowserPanel::DropCallback(GLFWwindow* window, int count, const char** paths) {
	// Access the AssetBrowserPanel instance using the GLFW window's user pointer
	AssetBrowserPanel* panel = static_cast<AssetBrowserPanel*>(glfwGetWindowUserPointer(window));
	if (!panel) return;

	for (int i = 0; i < count; ++i) {
		const char* filePath = paths[i];
		std::filesystem::path sourcePath(filePath);

		std::string extension = sourcePath.extension().string(); // Get file extension

		// Check if the current folder is the "Audio" folder
		if (panel->currentDirectory.filename() == "Audio") {
			// Restrict file types to .ogg and .wav in the Audio folder
			if (extension == ".ogg" || extension == ".wav") {
				// Valid audio formats
				std::filesystem::path destinationPath = panel->currentDirectory / sourcePath.filename();
				try {
					std::filesystem::copy(sourcePath, destinationPath, std::filesystem::copy_options::overwrite_existing);
					std::cout << "Audio file copied to: " << destinationPath << std::endl;
				} catch (const std::filesystem::filesystem_error& e) {
					std::cerr << "Error copying file: " << e.what() << std::endl;
				}
			} else {
				// Unsupported file type for the Audio folder
				panel->errorMessage = "Unsupported file type: " + extension +
					"\nOnly .ogg and .wav files are allowed in the Audio folder.";
				panel->showErrorPopup = true; // Trigger the popup
				std::cout << "Unsupported file type in Audio folder: " << extension << std::endl;
			}
		} else {
			// In other folders, allow all file types
			std::filesystem::path destinationPath = panel->currentDirectory / sourcePath.filename();
			try {
				std::filesystem::copy(sourcePath, destinationPath, std::filesystem::copy_options::overwrite_existing);
				MetadataHandler::AddToUUIDMap(destinationPath.string());
				std::cout << "File copied to: " << destinationPath << std::endl;
			} catch (const std::filesystem::filesystem_error& e) {
				std::cerr << "Error copying file: " << e.what() << std::endl;
			}
		}
	}

	// Refresh the asset browser to display updated files
	panel->RefreshAssets();
}

void AssetBrowserPanel::Update()
{
	ImGui::Begin(name.c_str(), nullptr);
	// Create two columns
	ImGui::BeginChild("left_pane", ImVec2(leftPaneWidth, 0), true);
	DisplayFolderTree(currentDirectory);
	ImGui::EndChild();

	//Splitter (draggable)
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));

	ImGui::Button("##splitter", ImVec2(8.0f, -1));

	// Change cursor on hover
	if (ImGui::IsItemHovered())
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

	if (ImGui::IsItemActive()) {
		leftPaneWidth += ImGui::GetIO().MouseDelta.x;
		if (leftPaneWidth < split_min) leftPaneWidth = split_min;
		if (leftPaneWidth > split_max) leftPaneWidth = split_max;
	}
	ImGui::PopStyleColor(3);

	// Right pane (Icons/Content)
	ImGui::SameLine();
	ImGui::BeginChild("right_pane", ImVec2(0, 0), true);

	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
		// Trigger popup if delete key pressed
		if (ImGui::IsKeyPressed(ImGuiKey_Delete) && !selectedAssetPath.empty()) {
			confirmDeletePopup = true;
		}
	}

	DisplayFilesInFolder(currentDirectory);

	// CODE FOR RIGHT CLICKING
	if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("AssetContextMenu");
	}

	if (ImGui::BeginPopupContextWindow("AssetContextMenu")) {
		if (ImGui::MenuItem("Create New Folder")) {
			CreateNewFolder();
		}

		if (ImGui::MenuItem("Create New Scene")) {
			CreateNewFile();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Open in File Explorer")) {
			OpenDirectoryInFileExplorer(currentDirectory.relative_path().string());
		}

		//if (ImGui::MenuItem("Refresh")) {
		//	RefreshAssets();
		//}

		ImGui::EndPopup();
	}
	// END CODE FOR RIGHT CLICKING

	ImGui::EndChild();

	// Make this a drop target
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_HIERARCHY_OBJECT")) {
			uint32_t droppedEntt = *static_cast<uint32_t*>(payload->Data);
			Name& nameC = ECSManager::GetInstance().GetComponent<Name>(droppedEntt);
			std::string targetPath = currentDirectory.relative_path().string() + "/" + nameC.name + ".prefab";
			Serializer::GetInstance().SerializePrefab(targetPath, droppedEntt);
			MetadataHandler::GenerateMetaFile(targetPath);
			nameC.prefabID = MetadataHandler::ParseUUIDFromMeta(targetPath + ".meta");
			nameC.prefabPath = targetPath;
			//sceneEntities[droppedEntt].isPrefab = true;
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();

	if (confirmDeletePopup) {
		ImGui::OpenPopup("Confirm Delete");
		confirmDeletePopup = false;
	}

	// STEP 3: Draw popup content
	if (ImGui::BeginPopupModal("Confirm Delete", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Are you sure you want to delete:\n%s", selectedAssetPath.filename().string().c_str());
		ImGui::Separator();

		// User confirms deletion
		if (ImGui::Button("Delete", ImVec2(120, 0))) {
			try {
				// Delete the selected file
				std::filesystem::remove(selectedAssetPath);

				// Also delete the corresponding .meta file, if it exists
				std::filesystem::path metaPath = selectedAssetPath.string() + ".meta";
				if (std::filesystem::exists(metaPath)) {
					std::filesystem::remove(metaPath);
				}
				selectedAssetPath.clear();
				RefreshAssets();
			}
			catch (const std::filesystem::filesystem_error& e) {
				// Capture error and trigger error popup with details
				errorMessage = "Error deleting file:\n" + std::string(e.what());
				showErrorPopup = true;
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		// Deletion Cancelled
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	// Show confirmation pop-up if triggered
	if (showPopup) {
		ImGui::OpenPopup("Confirm Scene Change");
		showPopup = false;  // Reset the flag
	}

	if (ImGui::BeginPopupModal("Confirm Scene Change", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Are you sure you want to change the scene to:\n%s?", selectedScenePath.c_str());
		ImGui::Separator();

		if (ImGui::Button("Yes", ImVec2(120, 0))) {
			// Load the new scene
			selectedEntity = nullptr;
			SceneManager::GetInstance().LoadScene(selectedScenePath);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("No", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (showErrorPopup) {
		ImGui::OpenPopup("Error"); // Open the error popup
	}

	if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("%s", errorMessage.c_str()); // Display the error message
		ImGui::Separator();

		if (ImGui::Button("Close")) {
			ImGui::CloseCurrentPopup();    // Close the popup when "Close" is clicked
			showErrorPopup = false;       // Reset the flag
		}

		ImGui::EndPopup();
	}
}

void AssetBrowserPanel::CreateNewFolder() {
	std::filesystem::path newFolderPath = currentDirectory / "New Folder";

	int counter = 1;
	while (std::filesystem::exists(newFolderPath)) {
		newFolderPath = currentDirectory / ("New Folder (" + std::to_string(counter) + ")");
		counter++;
	}

	std::filesystem::create_directory(newFolderPath);
}

void AssetBrowserPanel::CreateNewFile() {
	std::filesystem::path newFilePath = currentDirectory / "New File.txt";

	int counter = 1;
	while (std::filesystem::exists(newFilePath)) {
		newFilePath = currentDirectory / ("New File (" + std::to_string(counter) + ").txt");
		counter++;
	}

	std::ofstream file(newFilePath);
	file.close();
}

void AssetBrowserPanel::OpenDirectoryInFileExplorer(const std::string& directoryPath) {
	std::filesystem::path fullPath = std::filesystem::absolute(directoryPath);
	std::string command = "explorer \"" + fullPath.string() + "\"";
	std::system(command.c_str());
}