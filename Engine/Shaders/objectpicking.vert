/*********************************************************************
 * \file	objectpicking.vert
 * \brief	
 *      This vertex shader processes vertex data to prepare for
 *      rasterization. It handles position, color, normals, texture 
 *      coordinates, texture IDs, and a visibility flag to determine 
 *      if a vertex should be rendered or ignored.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	18 November 2024
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

out vec4 vColor;				// Pass the color to the fragment shader
out vec2 vTexCoords;			// Pass the texture coordinates to the fragment shader
flat out int vTexArrayID;		// Use 'flat' to prevent interpolation of texture ID
flat out int vTexLayerID;		// Use 'flat' to prevent interpolation of texture layer ID

uniform mat4 view;
uniform mat4 projection;

void main()
{
	if (aVisible == 0) {
		gl_Position = vec4(0.0);
		return;
	}
    gl_Position =  projection * view * vec4(aPos, 1.0);	// Set the vertex position
    vColor = aColor;				// Pass color
	vTexCoords = aTexCoord;
	vTexArrayID = aTextureArrayID;
	vTexLayerID = aTextureLayerID;
}