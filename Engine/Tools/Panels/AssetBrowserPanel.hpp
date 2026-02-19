/*********************************************************************
 * \file	AssetsBrowserPanel.hpp
 * \brief	Defines a AssetsBrowserPanel class that manages everything related to the assets in the workspace.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	08 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#ifndef ASSET_BROWSER_PANEL
#define ASSET_BROWSER_PANEL

#include <filesystem>
#include "../EditorPanel.hpp"
#include "../../Graphics/GraphicsManager.hpp"

class AssetBrowserPanel : public EditorPanel {
public:

	/**
	* @brief Constructor for AssetBrowserPanel.
	*
	* Initializes the panel with default values:
	* - Sets the name of the panel.
	* - Sets the default directory and current directory to `../Assets`.
	* - Initializes icons for directories and files (IDs will be set later).
	*/
	AssetBrowserPanel();
	~AssetBrowserPanel() = default;

	/**
	 * @brief Recursively displays the folder tree for a given folder path.
	 *
	 * This function is used to create a tree view of the directories starting from the given folder path. It supports
	 * navigation through directories by allowing the user to click on folder names to expand or collapse their contents.
	 *
	 * @param folderPath The path of the directory to display in the tree view.
	 */
	void DisplayFolderTree(const std::filesystem::path& folderPath);

	/**
	 * @brief Displays files and folders in a grid layout within the right pane.
	 *
	 * This function displays the contents of the given folder in a grid-like structure, with each item showing its
	 * icon and filename. Directories and files are displayed differently, with directories using the folder icon and
	 * files using the file icon.
	 *
	 * It also allows for drag-and-drop interactions, where users can drag files and directories.
	 *
	 * @param folderPath The path of the folder to display its contents.
	 */
	void DisplayFilesInFolder(const std::filesystem::path& folderPath);
	//void DisplayBreadcrumbs(const std::filesystem::path& currentPath);

	 /**
	 * @brief Initializes the asset browser by loading the icons for folders and files.
	 *
	 * This function loads the texture files for directory and file icons using the `GraphicsManager`. These icons are
	 * later used to visually represent directories and files in the browser panel.
	 */
	void Init();

	/**
	 * @brief Updates the Asset Browser Panel UI.
	 *
	 * This method constructs and updates the Asset Browser panel in the GUI using ImGui. It creates a two-panel layout:
	 * - A left pane containing a folder tree for navigating directories.
	 * - A right pane displaying the files in the currently selected directory.
	 *
	 * The method also includes a draggable splitter between the two panes, allowing users to resize the left pane dynamically.
	 * The splitter adjusts the width of the left pane based on the user's mouse input, with constraints to prevent the left pane
	 * from being resized beyond a specified minimum or maximum width.
	 *
	 * @note The right pane will show the content of the currently selected folder, while the left pane lets the user navigate
	 *       through the folder structure.
	 */
	void Update() override;

	/**
	* @brief Registers the drop callback for handling file drag-and-drop events.
	*
	* This function binds a GLFW drop callback to the provided window. The callback
	* enables the Asset Browser Panel to handle files dragged from the operating
	* system's file explorer and dropped into the application window.
	*
	* @param window A pointer to the GLFW window where the drop callback is registered.
	*
	* @note The registered callback will handle copying dropped files to the
	*       Asset Browser's directory and refreshing the UI.
	*/
	void RegisterDropCallback(GLFWwindow* window);

	/**
	* @brief Handles files dropped into the GLFW window.
	*
	* This static callback function is invoked when files are dragged and dropped
	* into the GLFW window. It copies the dropped files to the Asset Browser's
	* directory and refreshes the displayed files and directories in the panel.
	*
	* @param window A pointer to the GLFW window where the files were dropped.
	* @param count The number of files dropped.
	* @param paths An array of file paths representing the dropped files.
	*
	* @note This function requires the Asset Browser Panel instance to be set
	*       as the user pointer for the GLFW window using `glfwSetWindowUserPointer`.
	*/
	static void DropCallback(GLFWwindow* window, int count, const char** paths);  // GLFW drop callback

private:
	std::filesystem::path baseDirectory;
	std::filesystem::path currentDirectory;
	std::filesystem::path selectedAssetPath;  // Currently selected asset in the file browser
	bool confirmDeletePopup = false;  // Flag to trigger the delete confirmation popup on the next UI frame

	//size_t directoryIcon;
	//size_t fileIcon;

	/**
	* @brief Refreshes the displayed files and directories in the Asset Browser Panel.
	*
	* This function updates the current directory content in the Asset Browser Panel.
	* If the current directory is valid, it reloads the assets from that directory.
	* If the current directory is invalid, it attempts to reset to the parent directory
	* or the base directory. Logs are generated to indicate the refresh status or
	* directory changes.
	*/
	void RefreshAssets();  // Refresh the displayed files and folders

	bool showErrorPopup = false;
	std::string errorMessage;

	GLuint folderTexViewID;
	GLuint fileTexViewID;
	GLuint prefabTexViewID;
	GLuint sceneTexViewID;

	bool showPopup = false;
	std::string selectedScenePath;

	std::unordered_map<std::string, GLuint> cachedViews;

	/**
	* @brief Creates a new folder in the current directory.
	*
	* This function creates a new folder in the currently displayed directory
	* of the Asset Browser Panel. It ensures that the new folder is named
	* appropriately to avoid conflicts and updates the displayed content
	* to reflect the changes.
	*/
	void CreateNewFolder();

	/**
	* @brief Creates a new file in the current directory.
	*
	* This function creates a new file in the currently displayed directory
	* of the Asset Browser Panel. It ensures the file is named appropriately
	* to avoid conflicts and refreshes the displayed content to reflect the
	* addition of the new file.
	*/
	void CreateNewFile();

	/**
	* @brief Displays textures for the asset browser.
	*
	* This function is responsible for rendering textures, such as file
	* and directory icons, in the Asset Browser Panel. It ensures that the
	* graphical representation of assets is consistent with the user interface.
	*/
	void DisplayTextures();

	/**
	* @brief Opens a directory in the system's file explorer.
	*
	* This function opens the specified directory in the operating system's
	* default file explorer. It is primarily used for convenience in accessing
	* files and folders outside the application.
	*
	* @param directoryPath The path of the directory to open in the file explorer.
	*/
	void OpenDirectoryInFileExplorer(const std::string&);

};

#endif // !ASSET_BROWSER_PANEL