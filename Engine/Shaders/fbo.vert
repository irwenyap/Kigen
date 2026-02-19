/*********************************************************************
 * \file	fbo.vert
 * \brief	
 *      This vertex shader is used for rendering a fullscreen quad
 *      that displays the content of a framebuffer. It processes
 *      the vertex positions and texture coordinates for rendering.
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

layout(location = 0) in vec3 aPos;        // Fullscreen quad positions (2D)
layout(location = 1) in vec2 aTexCoords;  // Texture coordinates

out vec2 vTexCoords;  // Pass texture coordinates to fragment shader

void main()
{
	gl_Position = vec4(aPos, 1.0);
    vTexCoords = aTexCoords;
}