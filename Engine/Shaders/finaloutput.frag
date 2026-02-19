/*********************************************************************
 * \file	vignette.frag
 * \brief	
 *      This fragment shader processes fragment data to produce the final
 *      pixel color on the screen. It uses texture mapping and handles 
 *      transparency based on texture coordinates and texture IDs.
 *
 * \author	y.ziyangirwen, 2301345
 * \email	y.ziyangirwen@digipen.edu
 * \date	3 October 2024
 * 
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#version 460 core

in vec2 vTexCoords;

out vec4 FragColor;

uniform sampler2DArray gameTexture;  // The framebuffer texture for the game
uniform sampler2DArray uiTexture;    // The framebuffer texture for the UI
uniform int gameLayerIndex;          // Layer index for the game framebuffer
uniform int uiLayerIndex;            // Layer index for the UI framebuffer

void main()
{
    vec4 gameColor = texture(gameTexture, vec3(vTexCoords, gameLayerIndex));
    vec4 uiColor = texture(uiTexture, vec3(vTexCoords, uiLayerIndex));

    FragColor = mix(gameColor, uiColor, uiColor.a);
}