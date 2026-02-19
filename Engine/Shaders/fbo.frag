/*********************************************************************
 * \file	fbo.frag
 * \brief	
 *      This fragment shader is used for rendering a texture 
 *      from a framebuffer to the screen. It samples the texture 
 *      coordinates provided by the vertex shader and outputs the 
 *      final color to the framebuffer.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	3 October 2024
 * 
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#version 460 core

in vec2 vTexCoords;       // Texture coordinates from the vertex shader

out vec4 FragColor;       // Final output color

uniform sampler2DArray screenTexture;  // The texture array from the framebuffer
uniform int layerIndex;                // Layer index to sample

void main()
{
	FragColor = texture(screenTexture, vec3(vTexCoords, layerIndex));
}