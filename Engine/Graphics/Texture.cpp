/*********************************************************************
 * \file	Texture.cpp
 * \brief
 *      This file defines the `Texture` class, which represents
 *      a texture resource used in rendering applications. It provides
 *      functionalities for loading textures from files, binding and
 *      unbinding textures in OpenGL context, and managing texture
 *      identifiers for both application-level and OpenGL-level usage.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	5 November 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#include "Texture.hpp"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "../Application.hpp"
#include "Logger.hpp"

// Initialize all texture identifiers to 0
std::array<TextureArray, 32> Texture::textureArrays = {};

Texture::Texture() :
    type(type), id(0), texArrayIndex(0), texLayerIndex(0)
{
    static size_t idCounter = 0;
    id = idCounter++;
}

Texture::~Texture() 
{
}

bool Texture::LoadFromFile(const std::string& filePath)
{
    path = filePath;

	// Early exit if the file path is empty
	if (path == "") return false;

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip the image if needed
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (!data) {
		Logger::Instance().Log(Logger::Level::ERR, "[Texture] LoadFromFile: Failed to load texture from file: " + filePath);
        stbi_image_free(data);
		return false;
	}

    // This sets texArrayIndex to the index of the texture array to use
    SetTextureArrayToUse(width, height);

    if (texArrayIndex == 5) {
		//std::cout << "testing" << std::endl;
    }

    // Error checking
    if (!(texArrayIndex >= 0 && texArrayIndex < textureArrays.size())) {
		Logger::Instance().Log(Logger::Level::ERR, "[Texture] LoadFromFile: No valid texture array index");
        stbi_image_free(data);
        return false; 
    }

	GLenum internalFormat = Application::GetInstance().GetInternalFormat();

    // If the texture array is not initialized, initialize it
    if (textureArrays[texArrayIndex].id_gl == 0) {
		std::cout << "Initializing texture array: " << texArrayIndex << std::endl;

		glGenTextures(1, &textureArrays[texArrayIndex].id_gl);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[texArrayIndex].id_gl);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalFormat, width, height, textureArrays[texArrayIndex].allocatedLayers);
	}
    else if (textureArrays[texArrayIndex].currentLayers > textureArrays[texArrayIndex].allocatedLayers) {
		GLuint newID;
        glGenTextures(1, &newID);
        glBindTexture(GL_TEXTURE_2D_ARRAY, newID);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        textureArrays[texArrayIndex].allocatedLayers *= 2;
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalFormat, width, height, textureArrays[texArrayIndex].allocatedLayers);

        // Copy the old texture data to the new texture
        /*
        glCopyImageSubData(textureArrays[texArrayIndex].id_gl, GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0,
            			newID, GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, textureArrays[texArrayIndex].currentLayers - 1);
        */
        CopyAllTextureLayers(textureArrays[texArrayIndex].id_gl, newID, width, height, textureArrays[texArrayIndex].currentLayers - 1);

        // Delete the old texture
        glDeleteTextures(1, &textureArrays[texArrayIndex].id_gl);

        // Reassign the new texture ID
        textureArrays[texArrayIndex].id_gl = newID;
	}
    // Else, just bind the existing texture array
    else {
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[texArrayIndex].id_gl);
    }
	
    // Load the texture data into the texture array
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        // Load texture data into OpenGL
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, static_cast<GLint>(texLayerIndex), width, height, 1, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

        stbi_image_free(data);
        return true;
    }

    stbi_image_free(data);
    return false;
}

bool Texture::Load(int width, int height)
{
    // This sets texArrayIndex to the index of the texture array to use
	SetTextureArrayToUse(width, height);

    if (texArrayIndex == 5) {
        Logger::Instance().Log(Logger::Level::ERR, "[Texture] Load: Invalid texture width or height");
    }

	if (!(texArrayIndex >= 0 && texArrayIndex < textureArrays.size())) return false; // Error checking

	GLenum internalFormat = Application::GetInstance().GetInternalFormat();

	// If the texture array is not initialized, initialize it
	if (textureArrays[texArrayIndex].id_gl == 0) {
		glGenTextures(1, &textureArrays[texArrayIndex].id_gl);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[texArrayIndex].id_gl);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalFormat, width, height, textureArrays[texArrayIndex].allocatedLayers);
	}
    // Else if the texture array is full, copy the data to a new larger texture array
    else if (textureArrays[texArrayIndex].currentLayers > textureArrays[texArrayIndex].allocatedLayers) {
        GLuint newID;
        glGenTextures(1, &newID);
        glBindTexture(GL_TEXTURE_2D_ARRAY, newID);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        textureArrays[texArrayIndex].allocatedLayers *= 2;
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalFormat, width, height, textureArrays[texArrayIndex].allocatedLayers);

        // Copy the old texture data to the new texture
        CopyAllTextureLayers(textureArrays[texArrayIndex].id_gl, newID, width, height, textureArrays[texArrayIndex].currentLayers - 1);
        
        // Using this OpenGL function to copy would be easier, but I am worried about driver compatibility issues
        /*glCopyImageSubData(textureArrays[texArrayIndex].id_gl, GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0,
                        newID, GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, textureArrays[texArrayIndex].currentLayers - 1);*/

        // Delete the old texture
        glDeleteTextures(1, &textureArrays[texArrayIndex].id_gl);

        // Reassign the new texture ID
        textureArrays[texArrayIndex].id_gl = newID;
    }
	// Else, bind the existing texture array
	else {
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[texArrayIndex].id_gl);
	}

	return true;
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

size_t const& Texture::GetID() const
{
	return id;
}

size_t Texture::SetTextureArrayToUse(int widthImage, int heightImage)
{
    bool newArrayFlag{ false };
    texArrayIndex = static_cast<size_t>(-1); // Set the initial index to -1
	for (size_t i = 0; i < textureArrays.size(); ++i)
	{
        // Assigns the first available texture array
        if (!newArrayFlag 
            && textureArrays[i].width == 0 
            && textureArrays[i].height == 0
            && textureArrays[i].id_gl == 0)
		{
            texArrayIndex = i;
            newArrayFlag = true;
		}

        // Reassigns the texture array if it finds an array with the same width and height
		if (textureArrays[i].width == widthImage 
            && textureArrays[i].height == heightImage
            && textureArrays[i].id_gl != 0)
		{
            texArrayIndex = i;
            newArrayFlag = false;
			break;
		}
	}

    if (texArrayIndex == 5) {
        std::cout << "";
    }

    if (texArrayIndex < 32) {
        texLayerIndex = static_cast<size_t>(textureArrays[texArrayIndex].currentLayers++);
    }
    else {
        Logger::Instance().Log(Logger::Level::ERR, "[Texture] SetTextureArrayToUse: There are no available texture units");
        return static_cast<size_t>(-1);
    }
    
    // If a new texture array is used, set the width and height
    if (newArrayFlag) {
        textureArrays[texArrayIndex].width = widthImage;
		textureArrays[texArrayIndex].height = heightImage;
    }

    return texArrayIndex;
}

void Texture::CopyAllTextureLayers(GLuint srcTex, GLuint destTex, int width, int height, int numLayers)
{
    GLuint readFramebuffer, drawFramebuffer;
    glGenFramebuffers(1, &readFramebuffer);
    glGenFramebuffers(1, &drawFramebuffer);

    for (GLuint layer = 0; layer < static_cast<GLuint>(numLayers); ++layer) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);
		glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, srcTex, 0, layer);

        if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::Instance().Log(Logger::Level::ERR, "[Texture] CopyAllTextureLayers(): Read framebuffer is not complete");
            continue; // Skip to the next layer
        }

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);
		glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, destTex, 0, layer);

        if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			Logger::Instance().Log(Logger::Level::ERR, "[Texture] CopyAllTextureLayers(): Draw framebuffer is not complete");
			continue; // Skip to the next layer
		}

		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &readFramebuffer);
    glDeleteFramebuffers(1, &drawFramebuffer);
}