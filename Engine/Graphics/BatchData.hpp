/*********************************************************************
 * \file	BatchData.hpp
 * \brief
 *      This file declares the `BatchData` class, which manages
 *      the batching of vertex data and rendering operations for efficient
 *      graphics rendering using OpenGL.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	5 November 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#pragma once

#include <vector>
#include <glad/glad.h>

#include "../Application.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "Shader.hpp"
#include "FrameBuffer.hpp"
#include "Mesh.hpp"

// for glm::mat4
#include <glm/glm.hpp>

class EngineCamera;

 /*!*****************************************************************************
 \class BatchData
 \brief
	 A class that manages batch rendering data, including vertices and
	 indices, and handles the rendering of the batched data to the
	 screen or to a framebuffer.

	 The `BatchData` class is responsible for organizing vertex data and
	 indices, updating OpenGL buffers for efficient rendering, and
	 providing functionality for rendering operations.
 *******************************************************************************/
class BatchData {
public:

	/*!
	 * \brief Constructs a new `BatchData` instance.
	 *
	 * \param id The unique identifier for this batch.
	 * \param renderMode The rendering mode (e.g., GL_TRIANGLES).
	 * \param polygonMode The polygon mode (e.g., GL_FILL, GL_LINE).
	 */
	BatchData(size_t id, GLuint renderMode, GLuint polygonMode);

	/*!
	 * \brief Initializes the batch data.
	 *
	 * This function sets up the necessary OpenGL buffers (VAO, VBO, EBO)
	 * for rendering the batched data.
	 */
	void Init();

	/*!
	 * \brief Renders the batched data to a framebuffer
	 *
	 * \param shader The shader to use for rendering the batched data.
	 * \param textures The textures to bind for the rendering.
	 */
	void RenderToBuffer(Shader& shader, FrameBuffer& framebuffer, glm::mat4 view = glm::mat4(1.f), glm::mat4 projection = glm::mat4(1.f));

	/*!
	 * \brief Cleans up resources used by the batch data.
	 *
	 * This function releases the OpenGL resources associated with the
	 * batch data (VAO, VBO, EBO).
	 */
	void Exit();

	/*!
	 * \brief Updates the OpenGL buffers with the current vertex data.
	 *
	 * This function uploads the current vertex data and indices to the
	 * GPU, ensuring that rendering uses the latest data.
	 */
	void UpdateBuffers();

	bool IsEmpty();

public:
	size_t id; 							//!< The unique identifier for the batch. (Index within the batch list)
	GLuint renderMode; 					//!< The rendering mode for the batch (e.g., GL_TRIANGLES).
	GLuint polygonMode;					//!< The polygon mode for the batch (e.g., GL_FILL, GL_LINE).
		
	GLuint vao;							//!< The vertex array object for the batch.
	GLuint vbo;							//!< The vertex buffer object for the batch.
	GLuint ebo;							//!< The element buffer object for the batch.

	std::vector<size_t> meshIDs;		//!< The meshes to be batched, stored with their IDs.

	std::vector<Vertex> vertices;		//!< The combined vertices for the batch.
	std::vector<unsigned int> indices;	//!< The combined indices for the batch.

	bool isSorted;						//!< Flag to indicate if the batch data is sorted.
	bool isUpdated;						//!< Flag to indicate if the batch data has been updated.

private:
	static const size_t BATCH_SIZE = 65536; // !< The size to reserve for the batch data.
};
