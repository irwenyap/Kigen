/*********************************************************************
 * \file	Font.cpp
 * \brief
 *      This file defines the `Font` class, which manages
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
#include "Font.hpp"
#include <glad/glad.h>
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Logger.hpp"
#include "Texture.hpp"
#include <algorithm>


Font::Font() : maxGlyphWidth(0), maxGlyphHeight(0), texArrayIndex(0)
{
    static size_t idCounter = 0;
    id = idCounter++;
    characters.resize(128);
}

Font::~Font()
{
}

bool Font::LoadFromFile(const std::string& path)
{
    
    auto& textureArray = Texture::GetTextureArray();

    // Find an empty texture array, if there is none, return false
    if (FindEmptyTextureArray() >= textureArray.size()) {
        Logger::Instance().Log(Logger::Level::ERR, "[Font] LoadFromFile: No available texture array found");
        return false;
    }

    // Initialize FreeType
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        Logger::Instance().Log(Logger::Level::ERR, "[Font] LoadFromFile: FreeType library initialization failed");
        return false;
    }

    // Load font face
    FT_Face face;
    if (FT_New_Face(ft, path.c_str(), 0, &face)) {
        Logger::Instance().Log(Logger::Level::ERR, "[Font] LoadFromFile: FreeType failed to load font face");
        FT_Done_FreeType(ft);
        return false;
    }

    // Set the font size
    FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);

    // Load the first 128 characters of the ASCII set to retrieve the maximum glyph size
    for (unsigned char c = 0; c < 128; ++c) {
        // Load the glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            Logger::Instance().Log(Logger::Level::ERR, "[Font] LoadFromFile: FreeType failed to load glyph");
            continue;
        }
        maxGlyphWidth = std::max(maxGlyphWidth, face->glyph->bitmap.width);
        maxGlyphHeight = std::max(maxGlyphHeight, face->glyph->bitmap.rows);
    }

    // Generate the texture array
    glGenTextures(1, &textureArray[texArrayIndex].id_gl);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray[texArrayIndex].id_gl);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, maxGlyphWidth, maxGlyphHeight, 128, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    // Set the texture array information
    textureArray[texArrayIndex].width = maxGlyphWidth;
    textureArray[texArrayIndex].height = maxGlyphHeight;
    textureArray[texArrayIndex].allocatedLayers = 128;

    // Load each character glyph into a separate layer of the texture array
    for (unsigned char c = 0; c < 128; ++c) {
        // Load the glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            Logger::Instance().Log(Logger::Level::ERR, "[Font] LoadFromFile: FreeType failed to load glyph");
            continue;
        }

        // Flip the glyph bitmap
        std::vector<unsigned char> flippedBitmap(face->glyph->bitmap.width * face->glyph->bitmap.rows);
        for (unsigned int y = 0; y < face->glyph->bitmap.rows; ++y) {
            for (unsigned int x = 0; x < face->glyph->bitmap.width; ++x) {
				flippedBitmap[x + y * face->glyph->bitmap.width] = face->glyph->bitmap.buffer[x + (face->glyph->bitmap.rows - 1 - y) * face->glyph->bitmap.width];
			}
		}
        std::copy(flippedBitmap.begin(), flippedBitmap.end(), face->glyph->bitmap.buffer);


        // Copy glyph bitmap into texture array at layer 'c'
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, c, face->glyph->bitmap.width,
            face->glyph->bitmap.rows, 1, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        // Increment the number of layers in the texture array
        ++textureArray[texArrayIndex].currentLayers;

        // Store character information
        characters[c].texLayerIndex = c;
        characters[c].size = Vec2(static_cast<float>(face->glyph->bitmap.width), static_cast<float>(face->glyph->bitmap.rows));
        characters[c].bearing = Vec2(static_cast<float>(face->glyph->bitmap_left), static_cast<float>(face->glyph->bitmap_top));
        characters[c].advance = static_cast<GLuint>(face->glyph->advance.x);
    }

    // Restore the default unpack alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // Unbind the texture array
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    // Destroy FreeType resources
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return true;
}

size_t Font::FindEmptyTextureArray()
{
    texArrayIndex = static_cast<size_t>(-1);
    auto& textureArray = Texture::GetTextureArray();
    for (size_t i = 0; i < Texture::textureArrays.size(); ++i) {
        if (textureArray[i].id_gl == 0 && textureArray[i].currentLayers == 0) {
            texArrayIndex = i;
            break;
		}
	}
    return texArrayIndex;
}
