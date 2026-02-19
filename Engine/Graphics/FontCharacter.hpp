/*********************************************************************
 * \file	FontCharacter.hpp
 * \brief
 *      This file declares the `FontCharacter` class, which manages
 *      individual font characters
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

// Forward declaration
typedef unsigned int GLuint;

#include "Math.hpp"
#include "TextureArray.hpp"

// Textures for font characters will be managed separately from other textures
class FontCharacter
{
public:

	FontCharacter() = default;

	/*
	* \brief Constructor
	* \param textureLayerIndex The index of the texture layer that contains the font character
	* \param size The size of the font character
	* \param bearing The bearing of the font character
	* \param advance The advance of the font character
	*/
	FontCharacter(size_t textureLayerIndex, Vec2 size, Vec2 bearing, GLuint advance);

public:
	size_t texLayerIndex;
	Vec2 size;
	Vec2 bearing;
	GLuint advance;
};
