/*********************************************************************
 * \file	FrameBuffer.cpp
 * \brief
 *      This file defines the `FrameBuffer` class, which manages
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
#include "FrameBuffer.hpp"

#include "../Application.hpp"
#include "Texture.hpp"
#include "Logger.hpp"

FrameBuffer::FrameBuffer()
	: fbo(0), rbo(0), frameTexture(nullptr), width(0), height(0), quadVAO(0), quadVBO(0), quadEBO(0)
{

}

FrameBuffer::~FrameBuffer()
{
}

void FrameBuffer::Init()
{
    // Note: frameTexture is created and assigned in the init of GraphicsManager

    // Create the initial texture for the framebuffer based on the window size
    std::tie(width, height) = Application::GetInstance().GetWindowSize();

    // Finds a free texture unit to use
    frameTexture->Load(width, height);
    auto& textureArray = Texture::GetTextureArray();
    size_t arrayIndex = frameTexture->texArrayIndex;
    size_t layerIndex = frameTexture->texLayerIndex;

    if (arrayIndex >= textureArray.size()) {
		Logger::Instance().Log(Logger::Level::ERR, "[FrameBuffer] Init: No available texture array found");
		return;
	}

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    // Attach the specific layer in the texture array to the framebuffer
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureArray[arrayIndex].id_gl, 0, static_cast<GLint>(layerIndex));

    // Create renderbuffer object for depth and stencil attachment
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::Instance().Log(Logger::Level::ERR, "[FrameBuffer] Init: Framebuffer is not complete");
    }

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    InitFullScreenQuad();
}

void FrameBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void FrameBuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Exit()
{
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);

	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadEBO);
}

void FrameBuffer::Attach() 
{
    Bind();
    auto& textureArray = Texture::GetTextureArray();
    size_t arrayIndex = frameTexture->texArrayIndex;
    size_t layerIndex = frameTexture->texLayerIndex;
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
        textureArray[arrayIndex].id_gl, 0, static_cast<GLint>(layerIndex));
    Unbind();
}

void FrameBuffer::InitFullScreenQuad()
{
    // Simplified quad vertices: position (x, y, z), texCoord (u, v)
    float quadVertices[] = {
        // Positions        // TexCoords
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  // Bottom-left
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  // Top-right
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f   // Top-left
    };

    unsigned int quadIndices[] = {
        0, 1, 2,   // First triangle
        0, 2, 3    // Second triangle
    };

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    // Load the quad vertex data into the VBO
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Load the quad indices into the EBO
    glGenBuffers(1, &quadEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // Set position attribute (layout(location = 0))
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Set texture coordinate attribute (layout(location = 1))
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
