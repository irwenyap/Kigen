/*********************************************************************
 * \file	Mesh.hpp
 * \brief
 *      This file declares the `Mesh` class, which represents a
 *      collection of vertices that can be rendered in a graphics
 *      application. It includes functionalities for managing vertex
 *      data, textures, colors, and visibility for rendering purposes.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	3 October 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#pragma once

#include <glad/glad.h>
#include <vector>
#include <memory>
#include <functional>
#include "Vertex.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

 /*!*****************************************************************************
 \class Mesh
 \brief
	 A class representing a collection of vertices for rendering in a graphics application.

	 The `Mesh` class encapsulates vertex data and provides functionalities to set
	 textures, colors, and visibility for the mesh. It allows for efficient rendering
	 of geometric shapes by managing vertex attributes and rendering states.
 *******************************************************************************/
class Mesh 
{
public:

	// Never call this constructor
	Mesh() = delete;

	/*!
	 * \brief Parameterized constructor for the `Mesh` class.
	 *
	 * This constructor initializes the mesh with the specified vertex data,
	 * and defines the range of vertices to be used in rendering.
	 *
	 * \param vertices A reference to a vector of vertices.
	 * \param verticeStart The starting index of vertices to include in this mesh.
	 * \param verticeEnd The ending index of vertices to include in this mesh.
	 */
	Mesh(
		std::vector<Vertex> const& vertices, 
		std::vector<unsigned int> const& indices,
		std::vector<Vec3> const& modelSpacePosition,
		size_t batchID
	);

	/*!
	 * \brief Destructor for the `Mesh` class.
	 * Cleans up resources used by the mesh.
	 */
	~Mesh();
	
	/*!
	 * \brief Sets the texture for the mesh.
	 *
	 * This function assigns a texture ID to the mesh, which will be used during
	 * rendering to apply the specified texture.
	 *
	 * \param textureID The ID of the texture to apply to the mesh.
	 */
	//void SetTexture(int texArrayIndex, int texLayerIndex);

	/*!
	 * \brief Sets the color for the mesh.
	 *
	 * This function updates the color of the mesh, allowing it to be rendered
	 * with the specified color.
	 *
	 * \param color The color to set for the mesh, represented as a Vec4.
	 */
	//void SetColor(Vec4 color);

	/*!
	 * \brief Sets the visibility of the mesh.
	 *
	 * This function enables or disables the rendering of the mesh based on the
	 * specified visibility state.
	 *
	 * \param visibility True to make the mesh visible; false to hide it.
	 */
	//void SetVisibility(bool visibility);

public:
	size_t id;												//!< The unique identifier for the mesh.
	
	std::vector<Vec3> modelSpacePosition;					//!< The collection of model space positions for the mesh.

	//std::vector<std::reference_wrapper<Vertex>> vertices;	//!< Reference to the model's vertices in the batch to be sent to the GPU
	std::vector<Vertex> vertices;							//!< The collection of vertices for the mesh.
	std::vector<unsigned int> indices;						//!< The collection of indices for the mesh.

	size_t batchID;											//!< The ID of the batch to which the mesh belongs.

	Vec2 cumulativeScale;									//!< The cumulative scale of the mesh.
	float cumulativeRotation;
};