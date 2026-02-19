/*********************************************************************
 * \file	vignette.vert
 * \brief	
 *      This vertex shader processes vertex data to prepare for
 *      rasterization. It handles position, color, normals, texture 
 *      coordinates, texture IDs, and a visibility flag to determine 
 *      if a vertex should be rendered or ignored.
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

layout(location = 0) in vec3 aPos;        // Fullscreen quad positions (2D)
layout(location = 1) in vec2 aTexCoords;  // Texture coordinates

out vec2 vTexCoords;  // Pass texture coordinates to fragment shader

void main()
{
	gl_Position = vec4(aPos, 1.0);
    vTexCoords = aTexCoords;
}