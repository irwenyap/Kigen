/*********************************************************************
 * \file	Renderer.hpp
 * \brief	Defines the base Renderer component.
  *
  * \author	t.yongchin, 2301359
  * \email	t.yongchin@digipen.edu
  * \date	3 October 2024
  *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
  *********************************************************************/
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include "Vec.hpp"
#include "../Layers/SortingLayer.hpp"

/*********************************************************************
* \struct	Renderer
* \brief	Defines the Renderer component for rendering meshes.
*      The Renderer struct holds information related to the current
*      mesh being rendered, including its mesh ID, debug ID, current
*      texture ID, animation ID, and a flag indicating if the mesh
*      is animated. It provides constructors for initializing the
*      Renderer with default and specified values.
*********************************************************************/
struct Renderer {
	Renderer() = default;

	/*********************************************************************
	 * \brief	Parameterized constructor for the Renderer struct.
	 *      Initializes a Renderer instance with specified mesh IDs,
	 *      texture ID, animation ID, and animated flag.
	 *
	 * \param	meshID			ID of the mesh to be rendered.
	 * \param	meshDebugID		Debug ID for the mesh.
	 * \param	currentTextureID	Texture ID for the current texture (-1 if none).
	 * \param	animationID		ID for the current animation (-1 if none).
	 * \param	animated		Boolean flag indicating if the mesh is animated.
	 *********************************************************************/
	Renderer(size_t meshID, size_t meshDebugID, std::string uuid = "", size_t animationID = -1, bool isAnimated = false, SortingLayer sortingLayer = 0);
	
	bool isInitialized = false;
	bool isDirty = false;
	size_t currentMeshID;
	size_t currentMeshDebugID;
	//size_t currentTextureID;
	size_t currentAnimationID;
	bool isAnimated = false;

	SortingLayer prevSortingLayer = 0;
	SortingLayer sortingLayer = 0;
	bool sortingLayerChanged = false;

	std::string uuid;

	int mesh;

	void SetMeshID(size_t id) { currentMeshID = id; }
	void SetMeshDebugID(size_t id) { currentMeshDebugID = id; }
};

inline Renderer::Renderer(size_t meshID, size_t meshDebugID, std::string uuid, size_t animationID, bool isAnimated, SortingLayer sortingLayer)
	: currentMeshID(meshID), currentMeshDebugID(meshDebugID), uuid(uuid), currentAnimationID(animationID), isAnimated(isAnimated), sortingLayer(sortingLayer)
{
	mesh = 3;
}

#endif 
