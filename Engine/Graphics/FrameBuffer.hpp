/*********************************************************************
 * \file	FrameBuffer.hpp
 * \brief
 *      This file declares the `FrameBuffer` class, which manages
 *      the framebuffer objects used for offscreen rendering, as well as
 *      rendering a full-screen quad for displaying the framebuffer's texture.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	20 November 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>

#include "Vertex.hpp"

// Forward Declaration
class Texture;

/*!*****************************************************************************
\class FrameBuffer
\brief
	A class that manages framebuffer objects for offscreen rendering
	and handles the rendering of a full-screen quad to display the
	framebuffer's texture.

	The `FrameBuffer` class encapsulates the functionality required to
	create and manage framebuffers, including binding, unbinding, and
	rendering a full-screen quad that uses the framebuffer's attached
	texture.
*******************************************************************************/
class FrameBuffer {
public:
	/*!
	 * \brief Default constructor for the `FrameBuffer` class.
	 * Initializes a new instance of the `FrameBuffer`.
	 */
	FrameBuffer();
	/*!
	 * \brief Destructor for the `FrameBuffer` class.
	 * Cleans up resources used by the framebuffer.
	 */
	~FrameBuffer();

	/*!
	 * \brief Initializes the framebuffer and its attachments.
	 *
	 * This function sets up the framebuffer, rendering buffer, and
	 * attaches textures as needed for offscreen rendering.
	 */
	void Init();
	/*!
	 * \brief Binds the framebuffer for rendering.
	 *
	 * This function binds the framebuffer to the current OpenGL context,
	 * enabling subsequent rendering operations to be directed to this
	 * framebuffer.
	 */
	void Bind();
	/*!
	 * \brief Unbinds the framebuffer.
	 *
	 * This function unbinds the framebuffer, reverting to the default
	 * framebuffer (usually the screen).
	 */
	void Unbind();
	/*!
	 * \brief Cleans up resources used by the framebuffer.
	 *
	 * This function releases the resources associated with the framebuffer
	 * and its attachments.
	 */
	void Exit();

	/*!
	 * \brief Binds the framebuffer then attaches the texture to it.
	 */
	void Attach();

	/*!
	 * \brief Initializes the full-screen quad for rendering.
	 * 
	 * This function initializes the vertex array object (VAO) and vertex
	 * buffer object (VBO) for rendering a full-screen quad.
	 */
	void InitFullScreenQuad();

public:
	Texture* frameTexture;	// Texture attached to the framebuffer

	GLuint fbo;				// Framebuffer object
	GLuint rbo;				// Renderbuffer object

	GLuint quadVAO;			// Vertex array object for the quad
	GLuint quadVBO;			// Vertex buffer object for the quad
	GLuint quadEBO;			// Element buffer object for the quad

	int width;				// Width of the framebuffer
	int height;				// Height of the framebuffer
};