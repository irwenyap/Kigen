/*********************************************************************
 * \file		GraphicsManager.hpp
 * \brief
 *				This file declares the `GraphicsManager` class, which manages
 *				rendering operations, including loading meshes, shaders, textures,
 *				and handling batch rendering in a graphics application.
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
#pragma once

#include <glad/glad.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>
#include <utility>
#include <functional>

#include "Vertex.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "BatchData.hpp"
//#include "Animation.hpp"
#include "Font.hpp"
#include "EngineCamera.hpp"
#include "Vec.hpp"
#include "../ECS/Entity.hpp"
#include "../Layers/SortingLayer.hpp"

 /*!*****************************************************************************
 \class GraphicsManager
 \brief
	 A singleton class responsible for managing graphics resources and rendering
	 operations in a graphics application.

	 The `GraphicsManager` class provides methods for loading and managing meshes,
	 shaders, textures, and animations, as well as handling batch rendering and
	 framebuffer operations. It ensures efficient rendering by organizing graphics
	 resources and facilitating rendering calls.
 *******************************************************************************/
class GraphicsManager
{
public:

	/*!
	* \enum ShaderIndex
	* \brief Enum for shader indices.
	 */
	enum ShaderIndex : size_t
	{
		SHDR_DEFAULT = 0,
		SHDR_FRAMEBUFFER,
		SHDR_FONT,
		SHDR_OBJ_PICKING_WORLD,
		SHDR_TEXTURE_UI,
		SHDR_BRIGHT,
		SHDR_HORIBLUR,
		SHDR_VERTBLUR,
		SHDR_COMBINE,
		SHDR_VIGNETTE,
		SHDR_GLITCH,
		SHDR_FINAL,
		SHDR_VIDEOPLAYER,
		SHDR_OBJ_PICKING_UI,

		SHDR_MAX // This represents the total number of shaders, not an actual shader
	};

	/*!
	* \enum BatchIndex
	 * \brief Enum for batch indices.
	 */
	enum BatchIndex : size_t
	{
		FIRST_SRTG_LAYER = 0,
		LAST_SRTG_LAYER = MAX_SORTING_LAYERS - 1,
		DEBUG_BATCH,
		UI_TEXT_BATCH,
		UI_TEXTURE_BATCH,
		UI_VIDEO_TEXTURE_BATCH,

		MAX_BATCHES, // This represents the total number of batches, not an actual batch
		NO_BATCH = static_cast<size_t>(-1) // This represents an invalid batch
	};

	/*!
	* \enum FrameBufferIndex
	 * \brief Enum for framebuffer indices.
	 */
	enum FrameBufferIndex : size_t
	{
		GAME = 0,
		OBJ_PICKING_ENGINE,
		BRIGHT,
		HORIBLUR,
		VERTBLUR,
		COMBINE,
		VIGNETTE,
		GLITCH,
		UI,
		ENGINE,
		OBJ_PICKING_GAME,
		GAME_FINAL,
		OBJ_PICKING_UI,

		MAX_FRAMEBUFFERS // This represents the total number of framebuffers, not an actual framebuffer
	};

	/*!
	 * \brief Retrieves the singleton instance of the `GraphicsManager`.
	 *
	 * \return A reference to the `GraphicsManager` instance.
	 */
	static GraphicsManager& GetInstance();

	/*!
	 * \brief Default constructor for the `GraphicsManager` class.
	 * Initializes a new instance of the `GraphicsManager`.
	 */
	GraphicsManager();
	/*!
	 * \brief Destructor for the `GraphicsManager` class.
	 * Cleans up resources used by the graphics manager.
	 */
	~GraphicsManager();

	/*!
	 * \brief Initializes the graphics manager.
	 * Sets up necessary resources and states for rendering.
	 */
	void Init();
	/*!
	 * \brief Renders the current frame.
	 * Executes the rendering process for all loaded graphics resources.
	 */
	void Render();
	/*!
	 * \brief Exits and cleans up the graphics manager.
	 * Releases resources and performs necessary shutdown operations.
	 */
	void Exit();

	// Batch rendering
	/*!
	 * \brief Loads batch rendering settings.
	 *
	 * This function configures the rendering mode and polygon mode for batch
	 * rendering operations.
	 *
	 * \param renderMode The rendering mode (e.g., GL_TRIANGLES).
	 * \param polygonMode The polygon mode (e.g., GL_FILL).
	 */
	void LoadBatch(GLuint renderMode, GLuint polygonMode);

	// Mesh
	/*!
	 * \brief Loads a mesh from a file.
	 *
	 * This function loads a mesh from the specified file path and returns its ID.
	 *
	 * \param meshPath The path to the mesh file.
	 * \return The ID of the loaded mesh.
	 */
	size_t LoadMesh(std::string const& meshPath);
	/*!
	 * \brief Loads a mesh from vertex data.
	 *
	 * This function creates a mesh using the provided vertices and indices,
	 * returning its ID.
	 *
	 * \param vertices A vector of vertices to create the mesh.
	 * \param indices A vector of indices for indexing the vertices.
	 * \return The ID of the created mesh.
	 */
	size_t LoadMesh(std::vector<Vertex> const& vertices, std::vector<unsigned int> const& indices);
	/*!
	 * \brief Loads a collision mesh for the given mesh ID.
	 *
	 * This function generates a collision mesh based on the specified mesh ID.
	 *
	 * \param meshID The ID of the mesh for which to load the collision mesh.
	 * \return The ID of the loaded collision mesh.
	 */
	size_t LoadMeshCollision(size_t meshID);
	/*!
	 * \brief Refreshes the collision mesh for the specified mesh.
	 *
	 * This function updates the collision mesh based on the specified mesh ID
	 * and debug mesh ID.
	 *
	 * \param meshID The ID of the original mesh.
	 * \param meshDebugID The ID of the debug mesh to refresh.
	 * \param entity The entity to refresh the collision mesh for.
	 */
	void RefreshMeshCollision(size_t meshID, size_t meshDebugID, Entity entity);

	/*!
	 * \brief Loads a quad mesh.
	 *
	 * This function creates a quad mesh using optional provided vertices and
	 * returns its ID.
	 *
	 * \param batchID The ID of the batch to place the mesh in.
	 * \param vertices Optional vertices to define the quad.
	 * \return The ID of the created quad mesh.
	 */
	size_t LoadQuadMesh(size_t batchID = BatchIndex::FIRST_SRTG_LAYER, std::vector<Vertex> const& vertices = std::vector<Vertex>());
	/*!
	 * \brief Loads a triangle mesh.
	 *
	 * This function creates a triangle mesh using optional provided vertices and
	 * returns its ID.
	 *
	 * \param batchID The ID of the batch to place the mesh in.
	 * \param vertices Optional vertices to define the triangle.
	 * \return The ID of the created triangle mesh.
	 */
	size_t LoadTriangleMesh(size_t batchID = BatchIndex::FIRST_SRTG_LAYER, std::vector<Vertex> const& vertices = std::vector<Vertex>());
	/*!
	* \brief Loads a line mesh.
	*
	* This function creates a line mesh using optional provided vertices and
	* returns its ID.
	*
	* \param batchID The ID of the batch to place the mesh in.
	* \param vertices Optional vertices to define the line.
	* \return The ID of the created line mesh.
	*/
	size_t LoadLineMesh(size_t batchID = BatchIndex::FIRST_SRTG_LAYER, std::vector<Vertex> const& vertices = std::vector<Vertex>());
	/*!
	 * \brief Loads a sphere mesh.
	 *
	 * This function creates a sphere mesh using optional provided vertices and
	 * returns its ID.
	 * 
	 * \param batchID The ID of the batch to place the mesh in.
	 * \param vertices Optional vertices to define the sphere
	 * \return The ID of the created sphere mesh.
	 */
	size_t LoadSphereMesh(size_t batchID = BatchIndex::FIRST_SRTG_LAYER, std::vector<Vertex> const& vertices = std::vector<Vertex>());

	/*!
	 * \brief Loads a text character mesh.
	 * 
	 * This function creates a text character mesh using optional provided vertices
	 * and returns its ID.
	 * 
	 * \param batchID The ID of the batch to place the mesh in.
	 * \param vertices Optional vertices to define the text character.
	 * \return The ID of the created text character mesh.
	 */
	size_t LoadTextCharacterMesh(size_t batchID = BatchIndex::UI_TEXT_BATCH, std::vector<Vertex> const& vertices = std::vector<Vertex>());

	// Shader
	/*!
	 * \brief Loads a shader program from specified file paths by calling the AssetManager.
	 *
	 * This function compiles and links a shader program using the provided vertex
	 * and fragment shader file paths.
	 *
	 * \param path The path to the shader vertex and fragment files.
	 * 
	 * \return The ID of the loaded shader program.
	 */
	size_t LoadShader(std::string const& path);

	// Frame buffer
	/*!
	 * \brief Renders to the screen using the specified shader and framebuffer.
	 *
	 * \param shaderIndex The index of the shader to use for rendering.
	 * \param frameBuffIndex The index of the framebuffer to render to.
	 */
	void RenderToScreen(size_t shaderIndex, size_t frameBuffIndex);

	/*!
	 * \brief Free all texture arrays.
	 * 
	 */
	void FreeTextureArrays();

	// Fonts
	/*!
	 * \brief Loads a font from a file by calling the AssetManager.
	 * 
	 * \param fontPath The path to the font file.
	 * \return True if the font was successfully loaded, false otherwise.
	 */
	bool LoadFont(std::string const& fontPath);

	// For IMGUI
	/*!
	 * \brief Generates a temporary texture for IMGUI with the specified texture UUID.
	 * 
	 * \param textureUUID The UUID of the texture to generate.
	 * \return The ID_GL of the generated temporary texture.
	 */
	GLuint GenerateTempTexture(const std::string& textureUUID);

	/*!
	 * \brief Updates the temporary texture for IMGUI with the specified texture UUID.
	 * 
	 * \param textureUUID The UUID of the texture to update.
	 * \param tempTex The temporary texture to update.
	 */
	void UpdateTempTexture(const std::string& textureUUID, GLuint tempTex);

	/*!
	 * \brief Binds the temporary texture for IMGUI with the specified texture UUID.
	 * 
	 * \param textureUUID The UUID of the texture to bind.
	 * \param tempTex The temporary texture to bind.
	 */
	void BindTempTexture(const std::string& textureUUID, GLuint tempTex);

	/*!
	 * \brief Unbinds the temporary texture for IMGUI.
	 * 
	 * This function unbinds the temporary texture for IMGUI.
	 */
	void UnbindTempTexture();

	// Getters
	/*!
	 * \brief Getter function that returns the texture's assigned index in the texture array.
	 * 
	 * \param uid The UUID of the texture.
	 * \return The index of the texture in the texture array.
	 */
	size_t GetTextureArrayIndex(const std::string& uid) const;
	/*!
	 * \brief Getter function that returns the texture's assigned layer index in the texture array.
	 * 
	 * \param uid The UUID of the texture.
	 * \return The layer index of the texture.
	 */
	size_t GetTextureLayerIndex(const std::string& uid) const;
	/*!
	 * \brief Getter function that returns the texture's assigned ID_GL in the texture array.
	 * 
	 * \param uid The UUID of the texture.
	 * \return The ID_GL of the texture.
	 */
	GLuint GetTextureArrayIDGL(const std::string& uid) const;
	/*!
	 * \brief Getter function that returns the texture's width.
	 * 
	 * \param uid The UUID of the texture.
	 * \return The width of the texture.
	 */
	int GetTextureWidth(const std::string& uid) const;
	/*!
	 * \brief Getter function that returns the texture's height.
	 * 
	 * \param uid The UUID of the texture.
	 * \return The height of the texture.
	 */
	int GetTextureHeight(const std::string& uid) const;
	/*!
	 * \brief Getter function that returns the shader's ID_GL.
	 * 
	 * \param index The index of the shader.
	 * \return The ID_GL of the shader.
	 */
	GLuint GetShaderIDGL(size_t index) const;
	
	// Object picking
	/*!
	 * \brief Reads the pixel color at the specified mouse position from the framebuffer.
	 * 
	 * \param framebuffer The framebuffer to read from.
	 * \param mouseX The x-coordinate of the mouse position.
	 * \param mouseY The y-coordinate of the mouse position.
	 */
	Vec4 GetPixelColor(FrameBuffer& framebuffer, int mouseX, int mouseY);

	/*!
	 * \brief Copies the texture layer from the source texture to the destination texture.
	 * 
	 * This function copies the texture layer from the source texture to the destination texture.
	 * It is meant to be used in place of glCopyImageSubData as some hardware does not support it.
	 * 
	 * \param srcTex The source texture to copy from.
	 * \param srcLayer The source layer to copy from.
	 * \param destTex The destination texture to copy to.
	 * \param width The width of the texture.
	 * \param height The height of the texture.
	 */
	void CopyTextureLayer(GLuint srcTex, GLuint srcLayer, GLuint destTex, int width, int height);

	/*!
	 * \brief Creates a texture view for the specified texture array and layer.
	 * 
	 * \param textureArray The texture array to create the view from.
	 * \param layer The layer to create the view from.
	 * 
	 * \return The ID_GL of the created texture view.
	 */
	GLuint CreateTextureView(GLuint textureArray, int layer);

	/*!
	 * \brief Adds a mesh to the specified batch.
	 * 
	 * \param batchID The ID of the batch to add the mesh to.
	 * \param meshID The ID of the mesh to add.
	 * 
	 * \return True if the mesh was successfully added to the batch, false otherwise.
	 */
	bool AddToBatch(BatchIndex batchID, size_t meshID);

	/*!
	* \brief Removes a mesh from the specified batch.
	* 
	* \param batchID The ID of the batch to remove the mesh from.
	* \param meshID The ID of the mesh to remove.
	* 
	* \return True if the mesh was successfully removed from the batch, false otherwise.
	*/
	bool RemoveFromBatch(BatchIndex batchID, size_t meshID);

	/*!
	* \brief Sorts the specified batch.
	* 
	* \param batch The batch to sort.
	*/
	void SortBatch(BatchData& batch);

	/*!
	* \brief Updates the specified batch by refilling the vertex buffer with the mesh data.
	*
	* \param batch The batch to update.
	*/
	void UpdateBatch(BatchData& batch);

	/*!
	* \brief Sets the batch update flag.
	* 
	* The batch update flag is used to determine if batch needs to be updated.
	* Overloaded to accept meshID or batchID.
	* 
	* \param meshID Sets the update flag of the batch containing the mesh.
	* \param flag The flag to set.
	*/
	void SetBatchUpdateFlag(size_t meshID, bool flag = false);

	/*!
	* \brief Sets the batch update flag.
	*
	* The batch update flag is used to determine if the batch needs to be updated.
	* Overloaded to accept meshID or batchID.
	*
	* \param batchID Sets the update flag of the batch.
	* \param flag The flag to set.
	*/
	void SetBatchUpdateFlag(BatchIndex batchID, bool flag = false);

	/*!
	* \brief Sets the batch sort flag.
	*
	* The batch sort flag is used to determine if the batch needs to be sorted.
	* Overloaded to accept meshID or batchID.
	*
	* \param meshID Sets the sort flag of the batch containing the mesh.
	* \param flag The flag to set.
	*/
	void SetBatchSortFlag(size_t meshID, bool flag = false);

	/*!
	* \brief Sets the batch sort flag.
	*
	* The batch sort flag is used to determine if the batch needs to be sorted.
	* Overloaded to accept meshID or batchID.
	*
	* \param batchID Sets the sort flag of the batch.
	* \param flag The flag to set.
	*/
	void SetBatchSortFlag(BatchIndex batchID, bool flag = false);

	// Migrated functions from Mesh
	/*!
	 * \brief Sets the texture for the mesh.
	 *
	 * This function assigns a texture ID to the mesh, which will be used during
	 * rendering to apply the specified texture.
	 *
	 * \param textureID The ID of the texture to apply to the mesh.
	 */
	void SetTextureToMesh(size_t meshID, int texArrayIndex, int texLayerIndex);

	/*!
	 * \brief Sets the color for the mesh.
	 *
	 * This function updates the color of the mesh, allowing it to be rendered
	 * with the specified color.
	 *
	 * \param color The color to set for the mesh, represented as a Vec4.
	 */
	void SetColorToMesh(size_t meshID, Vec4 color);

	/*!
	 * \brief Sets the visibility of the mesh.
	 *
	 * This function enables or disables the rendering of the mesh based on the
	 * specified visibility state.
	 *
	 * \param visibility True to make the mesh visible; false to hide it.
	 */
	void SetVisibilityToMesh(size_t meshID, bool visibility);

	/*!
	 * \brief Returns the view matrix of the engine camera.
	 *
	 * \return The view matrix of the active engine camera.
	 */
	glm::mat4 GetViewMatrixEngine();

	/*!
	* \brief Returns the projection matrix of the engine camera.
	*
	* \return The projection matrix of the active engine camera.
	*/
	glm::mat4 GetProjectionMatrixEngine();

	/*!
	* \brief Returns the view matrix of the game camera.
	*
	* \return The view matrix of the active game camera.
	*/
	glm::mat4 GetViewMatrixGame();

	/*!
	* \brief Returns the projection matrix of the game camera.
	*
	* \return The projection matrix of the active game camera.
	*/
	glm::mat4 GetProjectionMatrixGame();

	/*!
	* \brief Loads all textures from the specified directory.
	*/
	void LoadAllTextures(const std::string&);

	/*!
	* \brief Sets the internal format for texture views.
	*/
	void SetInternalFormat(std::string internalFormat);

	GLenum GetInternalFormat() const;
	
public:
	std::vector<Shader> shaders;						// Shaders used for rendering
	std::vector<Mesh> meshes;							// Meshes used for rendering

	std::vector<GLuint> tempTextures;					// Temporary textures used for IMGUI

	// Batch rendering
	std::vector<BatchData> batches;						// Batch data for rendering

	// Frame buffer rendering
	std::vector<FrameBuffer> frameBuffers;				// Framebuffers for rendering

	bool debugMode;										// Debug mode flag

	// Camera
	EngineCamera camera;								// Camera used for rendering
	Entity activeCamera;								// Active camera entity

private:
	// These framebuffers are only meant to by used by CopyTextureLayer()
	GLuint readFramebuffer;
	GLuint drawFramebuffer;

	// Internal format for texture views
	GLenum internalFormat;
};