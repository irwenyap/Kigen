/*********************************************************************
 * \file	ui_font.frag
 * \brief	
 *      This fragment shader processes fragment data to produce the final
 *      pixel color on the screen. It uses texture mapping and handles 
 *      transparency based on texture coordinates and texture IDs.
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

in vec3 vColor;        // Color passed from vertex shader
in vec2 vTexCoords;    // Texture coordinates passed from vertex shader
flat in int vTexArrayID;    // Texture ID passed from vertex shader (flat, no interpolation)
flat in int vTexLayerID;        // Texture ID passed from vertex shader (flat, no interpolation)

out vec4 FragColor;

uniform sampler2DArray textureArrays[32];  // Array of texture samplers (up to 32)

void main()
{
    // If texture ID is invalid (less than 0), use the vertex color
    if (vTexArrayID < 0 || vTexLayerID < 0)
    {
        FragColor = vec4(vColor, 1.0f);
    }
    // If texture ID is valid, sample the corresponding texture
    else
    {
        vec4 sampledColor = vec4(1.0f, 1.0f, 1.0f, texture(textureArrays[vTexArrayID], vec3(vTexCoords, vTexLayerID)).r);
        FragColor = vec4(vColor, 1.0f) * sampledColor;
    }
	
	if (FragColor.a == 0) {
        discard;
    }
}