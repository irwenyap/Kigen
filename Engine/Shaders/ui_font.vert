/*********************************************************************
 * \file	ui_font.vert
 * \brief	
 *      This vertex shader processes vertex data to prepare for
 *      rasterization. It handles position, color, normals, texture 
 *      coordinates, texture IDs, and a visibility flag to determine 
 *      if a vertex should be rendered or ignored.
 *      This file is specifically for fonts.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	5 November 2024
 * 
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#version 460 core

layout(location = 0) in vec3 aPos;				// Position
layout(location = 1) in vec4 aColor;			// Color
layout(location = 2) in vec3 aNormal;			// Normal
layout(location = 3) in vec2 aTexCoord;			// Texture coordinates
layout(location = 4) in int aTextureArrayID;	// Texture Array ID
layout(location = 5) in int aTextureLayerID;	// Texture Layer ID
layout(location = 6) in int aVisible;  			// Visibility flag

out vec3 vColor;				// Pass the color to the fragment shader
out vec2 vTexCoords;			// Pass the texture coordinates to the fragment shader
flat out int vTexArrayID;		// Use 'flat' to prevent interpolation of texture ID
flat out int vTexLayerID;		// Use 'flat' to prevent interpolation of texture layer ID

void main()
{
    if (aVisible == 0) {
		gl_Position = vec4(0.0);
		return;
	}
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);	// Set the vertex position
	
    vTexCoords = aTexCoord;			// Pass texture coordinates
    vColor = vec3(aColor);			// Pass the color to the fragment shader
    vTexArrayID = aTextureArrayID;	// Pass texture unit ID (flat shading, no interpolation)
	vTexLayerID = aTextureLayerID;	// Pass texture layer ID (flat shading, no interpolation)
}  