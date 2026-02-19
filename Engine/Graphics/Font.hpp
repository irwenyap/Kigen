/*********************************************************************
 * \file	Font.hpp
 * \brief
 *      This file declares the `Font` class, which manages
 *      fonts
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

#include "FontCharacter.hpp"
#include <vector>
#include "../Asset.hpp"

class Font : public IAsset
{

public:

	Font();
	~Font();

	/*
	* \brief Load the font from a file
	* \param path The file path
	*/
	bool LoadFromFile(const std::string& path) override;

	/*
	* \brief Find an empty texture array for the font to use
	* 
	* \return The index of the empty texture array
	*/
	size_t FindEmptyTextureArray();

public:

	// The initial size of the font to load
	static int const FONT_SIZE = 400;

	size_t id;
	size_t texArrayIndex;
	std::vector<FontCharacter> characters;
	unsigned int maxGlyphWidth;
	unsigned int maxGlyphHeight;
};