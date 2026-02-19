/*********************************************************************
 * \file	Texture.hpp
 * \brief
 *      This file declares the `Texture` class, which represents
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
#pragma once

#include <string>
#include <array>

#include "../Asset.hpp"
#include "TextureArray.hpp"

// Forward declaration
typedef unsigned int GLuint;

/*!*****************************************************************************
\class Texture
\brief
	A class representing a texture resource used in rendering.

	This class provides functionality to load, bind, and unbind textures, as well as
	manage texture identifiers for both application-level and OpenGL-level usage.
*******************************************************************************/
class Texture : public IAsset
{
public:
	/*!*****************************************************************************
	\brief
		Default constructor for the Texture class.

		Initializes a new texture with default values.
	*******************************************************************************/
	Texture();

	/*!*****************************************************************************
	\brief
		Destructor for the Texture class.

		Cleans up any resources associated with the texture.
	*******************************************************************************/
	~Texture();

	/*!*****************************************************************************
   \brief
	   Loads texture data from a file.
   \param path
	   The file path to the texture image.
   \return
	   Returns true if the texture was successfully loaded; otherwise, false.
   *******************************************************************************/
	bool LoadFromFile(const std::string&) override;

	/*!*****************************************************************************
   \brief
	   Loads blank texture data given the width and height.
   *******************************************************************************/
	bool Load(int width, int height);

	/*!*****************************************************************************
   \brief
	   Unbinds the texture.

	   This makes the currently active texture inactive in the OpenGL context.
   *******************************************************************************/
	void Unbind();

	/*!*****************************************************************************
	\brief
		Gets the texture ID.
	\return
		A constant reference to the texture ID used in the application.
	*******************************************************************************/
	size_t const& GetID() const;

	
	/*!*****************************************************************************
	* \brief
	*	Finds the texture array to use based on the width and height of the image
	*	Either finds an existing texture array with the same dimensions or assigns it to a new one
	* 
	* \param widthImage
	*	The width of the image to be stored in the texture array
	* \param heightImage
	*	The height of the image to be stored in the texture array
	* \return
	*	The index of the texture array to use
	* ******************************************************************************/
	size_t SetTextureArrayToUse(int widthImage, int heightImage);

	/*!*****************************************************************************
	* \brief
	*	Copies all texture layers from one texture array to another
	*	Used in place of glCopyImageSubData as it is incompatible with certain hardware
	* 
	* \param srcTex
	*	The source texture array to copy from
	* \param destTex
	*	The destination texture array to copy to
	* \param width
	*	The width of the texture array
	* \param height
	*	The height of the texture array
	* \param numLayers
	*	The number of layers to copy
	* ******************************************************************************/
	void CopyAllTextureLayers(GLuint srcTex, GLuint destTex, int width, int height, int numLayers);

	/*!*****************************************************************************
	* \brief
	*	Static function to get the texture array vector
	* \return
	*	A reference to the texture array vector
	* ******************************************************************************/
	static std::array<TextureArray, 32>& GetTextureArray();

public:
	// Stores the OpenGL Texture Array ID
	// TextureArray is a simple class that only stores the id_gl of the texture array and its dimensions
	static std::array<TextureArray, 32> textureArrays;

	// This ID is used to identify the texture in the application (index in the vector)
	size_t id;

	size_t texArrayIndex;
	size_t texLayerIndex;

	std::string type;
	std::string name;
	std::string path;
};

inline std::array<TextureArray, 32>& Texture::GetTextureArray()
{
	return textureArrays;
}