/*********************************************************************
 * \file	TextureArray.hpp
 * \brief
 *      This file declares the `TextureArray` class, which is a
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
#pragma once

typedef unsigned int GLuint;

/*!*****************************************************************************
\class TextureArray
\brief
	A class that holds basic information about a texture array.
*******************************************************************************/
class TextureArray {
public:

	/*
	* \brief Constructor
	* \param id_gl The OpenGL ID of the texture array
	* \param width The width of the texture array
	* \param height The height of the texture array
	* \param initialAllocatedLayers The initial number of layers allocated for the texture array
	*/
	TextureArray(GLuint id_gl = 0, int width = 0, int height = 0, int initialAllocatedLayers = 8);

	/*
	* \brief Bind the texture array to a texture unit
	* 
	* \param arrayIndex The array index to bind the texture unit to
	*/
	void Bind(int arrayIndex);

	/*
	* \brief Unbind the texture array from a texture unit
	*/
	void Unbind();

	GLuint id_gl;
	int currentLayers;
	int allocatedLayers;
	int width, height;
};
