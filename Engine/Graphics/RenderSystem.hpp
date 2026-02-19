/*********************************************************************
 * \file	RenderSystem.hpp
 * \brief
 *      This file declares the `RenderSystem` class, which is responsible
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
#pragma once

#include <glad/glad.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <utility>
#include <string>

#include "../ECS/System.hpp"

#include "GraphicsManager.hpp"

class Mesh;
struct Vec2;
struct Vertex;
struct Mat4;
struct Renderer;
struct Animation;

/*!*****************************************************************************
\class RenderSystem
\brief
	A class for managing rendering operations in a graphics application.

	The `RenderSystem` class is responsible for managing meshes, textures,
	animations, and background settings. It interacts with the `GraphicsManager`
	to handle rendering tasks and provides methods for manipulating and rendering
	graphics assets in the application.
*******************************************************************************/
class RenderSystem : public System {
public:
	/*!
	 * \enum MeshType
	 * \brief Enumeration of different mesh types supported by the render system.
	 */
	enum MeshType {
		LOAD_FROM_FILE = 0,
		TRIANGLE,
		LINE,
		QUAD,
		SPHERE,
		TEXTCHAR,
		QUAD_UI,
		VIDEO_UI
	};

	/*!
	 * \brief Default constructor for the `RenderSystem` class.
	 * Initializes the rendering system.
	 */
	RenderSystem();
	/*!
	 * \brief Destructor for the `RenderSystem` class.
	 * Cleans up resources used by the render system.
	 */
	~RenderSystem();

	/*!
	 * \brief Initializes the rendering system.
	 * Sets up necessary resources and state for rendering.
	 */
	void Init();
	/*!
	 * \brief Updates each renderer's visibility flag.
	 * Updates based on whether its entity is currently active or not.
	 */
	void UpdateEntitiesVisibility();
	/*!
	 * \brief Updates the render system.
	 * Performs any necessary updates for rendering.
	 */
	void Update();
	/*!
	 * \brief Exits the rendering system.
	 * Cleans up resources and prepares for shutdown.
	 */
	void Exit();

	// Mesh management
	/*!
	 * \brief Adds a mesh to the render system.
	 *
	 * This function creates a mesh of the specified type, optionally loading
	 * it from a file or using provided vertex data.
	 *
	 * \param mtype Type of the mesh to create (default is `LOAD_FROM_FILE`).
	 * \param path Path to the mesh file (used if mtype is `LOAD_FROM_FILE`).
	 * \param vertices Vertex data for the mesh (used if mtype is not `LOAD_FROM_FILE`).
	 * \return A pair containing the mesh ID and debug ID.
	 */
	std::pair<size_t, size_t> AddMesh(MeshType mtype = MeshType::LOAD_FROM_FILE,
		std::string const& path = "", std::vector<Vertex> const& vertices = std::vector<Vertex>());
	std::pair<size_t, size_t> AddMesh(int mtype = 0, size_t batchID = 0, std::string const& path = "", std::vector<Vertex> const& vertices = std::vector<Vertex>());
	/*!
	 * \brief Retrieves a mesh by ID.
	 *
	 * This function returns a reference to the mesh associated with the given ID.
	 *
	 * \param id The ID of the mesh to retrieve.
	 * \return A reference to the requested `Mesh` object.
	 */
	Mesh& GetMesh(size_t id);
	/*!
	 * \brief Retrieves the ID of a given mesh.
	 *
	 * This function returns the ID associated with the specified mesh.
	 *
	 * \param mesh The mesh for which to retrieve the ID.
	 * \return The ID of the specified mesh.
	 */
	size_t GetID(Mesh& mesh);

	// Texture management
	/*!
	 * \brief Associates a texture with a specified mesh.
	 *
	 * This function sets the specified texture ID to the given mesh ID,
	 * allowing the mesh to use that texture during rendering.
	 *
	 * \param meshID The ID of the mesh to which the texture will be assigned.
	 * \param texID The ID of the texture to assign to the mesh.
	 */
	void SetTextureToMesh(size_t meshID, std::string texID);

	/*!
	* \brief Associates a texture with a specified entity.
	*
	* This function sets the specified texture ID to the given entity,
	* allowing the entity's mesh to use that texture during rendering.
	*
	* \param entity The entity to which the texture will be assigned.
	* \param texID The ID of the texture to assign to the entity.
	*/
	void SetTextureToEntity(Entity entity, std::string texID);

	/*!
	* \brief Sets the color of a mesh.
	* 
	* This function sets the color of the specified mesh.
	* 
	* \param meshID The ID of the mesh to modify.
	* \param color The new color for the mesh.
	*/
	void SetColorToMesh(size_t const& meshID, Vec4 color);

	/*!
	* \brief Sets the color of an entity.
	* 
	* This function sets the color of the specified entity.
	* 
	* \param entity The entity whose color will be modified.
	* \param color The new color for the entity.
	*/
	void SetColorToEntity(Entity entity, Vec4 color);

	// Debug mode
	/*!
	 * \brief Sets the debug mode for the render system.
	 *
	 * This function enables or disables debug mode, allowing for additional
	 * information to be printed during rendering operations.
	 *
	 * \param val True to enable debug mode; false to disable it.
	 */
	void SetDebugMode(bool val);	

	// Visibility
	/*!
	 * \brief Sets the visibility of a mesh.
	 *
	 * This function controls whether a specified mesh is visible during rendering.
	 *
	 * \param meshID The ID of the mesh to modify.
	 * \param val True to make the mesh visible; false to hide it.
	 */
	void SetVisibility(size_t const& meshID, bool val);
	/*!
	 * \brief Sets the visibility of a mesh.
	 *
	 * This overload of the `SetVisibility` function controls whether a specified
	 * entity's mesh is visible during rendering.
	 *
	 * \param entity The entity whose mesh visibility will be modified.
	 * \param val True to make the mesh visible; false to hide it.
	 */
	void SetVisibility(Entity entity, bool val);

	/*!
	 * \brief Sets the visibility of all meshes in a specified layer.
	 * 
	 * \param layer The layer to modify.
	 * \param visible Whether the layer should be visible.
	 */
	void UpdateVisibilityLayer(uint8_t layer, bool visible);

	/*!
	* \brief Loads a font for rendering text by calling the AssetManager.
	* 
	* This function loads a font for rendering text by calling the AssetManager.
	* 
	* \param path The path to the font file.
	*/
	void LoadFont(std::string const& path);

	// Object picking
	/*!
	* \brief Encodes the entity ID into a color.
	* 
	* This function encodes the entity ID into a color for object picking.
	* 
	* \param entity The entity to encode.
	* \return The color encoded from the entity ID.
	*/
	Vec4 EncodeColor(Entity entity);

	/*!
	* \brief Decodes the entity ID from a color.
	* 
	* This function decodes the entity ID from a color for object picking.
	* 
	* \param color The color to decode.
	* \return The entity ID decoded from the color.
	*/
	Entity DecodeColor(Vec4 color);

	/*!
	* \brief Retrieves the entity the mouse is hovering over.
	* 
	* \param fbo The framebuffer object to read from. (See FrameBufferIndex for options)
	*/
	Entity GetClickedEntity(int fbo = 1);
	
private:
	bool paused = false; // Tracks if the system is paused
	bool isGMInitialized = false; // Tracks if the Graphics Manager has been initialized.
};