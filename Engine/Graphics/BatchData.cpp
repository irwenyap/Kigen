/*********************************************************************
 * \file	BatchData.cpp
 * \brief
 *      This file defines the `BatchData` class, which manages
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
#include "BatchData.hpp"
#include "EngineCamera.hpp"

BatchData::BatchData(size_t id, GLuint renderMode, GLuint polygonMode) :
	id(id), renderMode(renderMode), polygonMode(polygonMode), vao(0), vbo(0), ebo(0), vertices(), indices(), isSorted(false), isUpdated(false)
{
	vertices.reserve(BATCH_SIZE);
	indices.reserve(BATCH_SIZE);
}

void BatchData::Init()
{
    // Generate and bind VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate and bind VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Generate and bind EBO
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Define the vertex attributes layout
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    // Normal attribute (location = 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

    // Texture coordinate attribute (location = 3)
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(3);

    // Texture Unit ID attribute (location = 4)
    glVertexAttribIPointer(4, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, texArray));
    glEnableVertexAttribArray(4);

    // Texture Layer attribute (location = 5)
    glVertexAttribIPointer(5, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, texLayer));
    glEnableVertexAttribArray(5);

    // Visibility attribute (location = 6)
    glVertexAttribIPointer(6, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, visible));
    glEnableVertexAttribArray(6);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void BatchData::RenderToBuffer(Shader& shader, FrameBuffer& framebuffer, glm::mat4 view, glm::mat4 projection)
{
    // Use the shader
    shader.Use();

    // Bind the framebuffer
    framebuffer.Bind();
    // Set the viewport to the framebuffer size
    glViewport(0, 0, framebuffer.width, framebuffer.height);

    // Updates the VBO with the batch vertices
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // Texture binding
    auto& textureArray = Texture::GetTextureArray();
    for (unsigned int i = 0; i < textureArray.size(); ++i) {
        // Bind the texture using the unit
        textureArray[i].Bind(i);

        // Get uniform location only once
        GLint loc = glGetUniformLocation(shader.id_gl, ("textureArrays[" + std::to_string(i) + "]").c_str());
        if (loc != -1) {
            glUniform1i(loc, i);  // Set the uniform to the current texture unit
        }
        else {
            break;
        }
    }

    GLint viewLoc = glGetUniformLocation(shader.id_gl, "view");
    if (viewLoc != -1) {
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }
    GLint projLoc = glGetUniformLocation(shader.id_gl, "projection");
    if (projLoc != -1) {
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    }

    if (renderMode == GL_LINES) {
        glLineWidth(2.0f);
    }

    // Set the polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    // Bind the batch VAO
    glBindVertexArray(vao);

    // Draw all elements in the batch
    glDrawElements(renderMode, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    framebuffer.Unbind();
}

void BatchData::Exit() 
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void BatchData::UpdateBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool BatchData::IsEmpty()
{
    return vertices.empty();
}

