/*********************************************************************
 * \file	combi.frag
 * \brief
 *			Creates the final texture after bloom and blur pass
 *
 * \author	y.ziyangirwen, 2301359
 * \date	3 October 2024
 * 
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#version 460 core

in vec2 vTexCoords;

out vec4 FragColor;

uniform sampler2DArray screenTexture;
uniform int screenLayerIndex;

uniform sampler2DArray blurTexture;
uniform int blurLayerIndex;

uniform float bloomIntensity; // Intensity of the bloom

void main()
{
    vec3 sceneColor = texture(screenTexture, vec3(vTexCoords, screenLayerIndex)).rgb;
    vec3 bloomColor = texture(blurTexture, vec3(vTexCoords, blurLayerIndex)).rgb;

    vec3 finalColor = sceneColor + bloomColor * bloomIntensity;

    FragColor = vec4(finalColor, 1.0);
}