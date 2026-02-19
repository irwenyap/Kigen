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

uniform float vignetteStrength = 0.8;       // Strength of the vignette effect
uniform float vignetteSoftness = 0.9;
uniform vec2 vignetteCenter = vec2(0.5, 0.34);

uniform sampler2DArray screenTexture;
uniform int layerIndex;

void main()
{
    vec2 uv = vTexCoords - vignetteCenter;
    float dist = length(uv);

    float vignette = smoothstep(1.0 - vignetteStrength, 1.0, dist / vignetteSoftness);
    vignette = 1.0 - vignette; // Invert to darken edges

    vec3 color = texture(screenTexture, vec3(vTexCoords, layerIndex)).rgb;
    FragColor = vec4(color * vignette, 1.0);
}