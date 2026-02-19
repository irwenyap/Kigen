/*********************************************************************
 * \file	bright.frag
 * \brief	
 * 			Does a bunch of random stuff, currently used to extract bright spots
 *
 * \author	y.ziyangirwen, 2301359
 * \date	3 October 2024
 * 
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#version 460 core

in vec2 vTexCoords;       // Texture coordinates from the vertex shader

out vec4 FragColor;       // Final output color

uniform float vignetteStrength;       // Strength of the vignette effect
uniform sampler2DArray screenTexture;  // The texture array from the framebuffer
uniform int layerIndex;                // Layer index to sample
uniform float brightnessThreshold = 0.85;  // Threshold for bright areas

void main()
{
	//Bright pass and blur
	vec3 color = texture(screenTexture, vec3(vTexCoords, layerIndex)).rgb;
    vec3 bright = max(color - brightnessThreshold, vec3(0.0));
    FragColor = vec4(bright, 1.0);
}