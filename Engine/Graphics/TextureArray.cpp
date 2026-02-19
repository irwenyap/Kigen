/*********************************************************************
 * \file	TextureArray.cpp
 * \brief
 *      This file defines the `TextureArray` class, which is a 
		simple class to hold information about a texture array.
		
		The reason this class exists is to allow for textures to be 
		automatically sorted into the correct texture array based on their size.
		Otherwise, the user would have to tell the program which texture array to use
		either via serialization or some other method.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	5 November 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#include "TextureArray.hpp"
#include <glad/glad.h>

TextureArray::TextureArray(GLuint id_gl, int width, int height, int initialAllocatedLayers) :
	id_gl(id_gl), width(width), height(height), currentLayers(0), allocatedLayers(initialAllocatedLayers)
{
}

void TextureArray::Bind(int arrayIndex)
{
	glActiveTexture(GL_TEXTURE0 + arrayIndex);
	glBindTexture(GL_TEXTURE_2D_ARRAY, id_gl);
}

void TextureArray::Unbind()
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}