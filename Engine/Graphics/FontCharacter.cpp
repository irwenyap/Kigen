/*********************************************************************
 * \file	FontCharacter.cpp
 * \brief
 *      This file defines the `FontCharacter` class, which manages
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
#include "FontCharacter.hpp"
#include "Math.hpp"
#include <iostream>

FontCharacter::FontCharacter(size_t textureLayerIndex, Vec2 size, Vec2 bearing, GLuint advance)
	: texLayerIndex(textureLayerIndex), size(size), bearing(bearing), advance(advance)
{

}

