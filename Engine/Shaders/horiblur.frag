/*********************************************************************
 * \file	default.frag
 * \brief	
 *			Samples the bloom and blurs the bright spots horizontally
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
uniform int layerIndex;

const float intensity = 1.5;
const float stretch = 3.0;

void main()
{
    float texelWidth = 1.0 / 1920.0;
    vec3 result = vec3(0.0);
    float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    // Apply horizontal blur with anamorphic stretch
    for (int i = -4; i <= 4; ++i) {
        vec3 offset = vec3(float(i) * texelWidth * stretch, 0.0, 0.0);
        result += texture(screenTexture, vec3(vTexCoords, layerIndex) + offset).rgb * weights[abs(i)];
    }

    // Apply intensity to control bloom strength
    FragColor = vec4(result * intensity, 1.0);
}