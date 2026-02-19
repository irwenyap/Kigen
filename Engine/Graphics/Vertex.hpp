/*********************************************************************
 * \file	Vertex.hpp
 * \brief	Declaration of the `Vertex` struct used for rendering.
 *
 * This struct contains the attributes required to define a vertex,
 * including its position, color, normal vector, texture coordinates,
 * texture ID, and visibility state.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	3 October 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#pragma once

#include "Vec.hpp"

 /*!*****************************************************************************
 \struct Vertex
 \brief
	 A struct representing a single vertex in 3D space, with properties
	 for position, color, normal vector, texture coordinates, texture ID,
	 and visibility state.

 *******************************************************************************/
struct Vertex
{
	Vec3 position;	// Position in 2D space
	Vec4 color;		// Color in RGBA format
	Vec3 normal;	// Normal vector
	Vec2 texCoord;	// Texture coordinates
	int texArray;	// Texture array ID
	int texLayer;	// Texture layer ID
	int visible;	// Visibility state

	/*!*****************************************************************************
	\brief
		Constructs a vertex using individual float values for its attributes.
	\param posx
		The x-coordinate of the vertex position.
	\param posy
		The y-coordinate of the vertex position.
	\param posz
		The z-coordinate of the vertex position.
	\param colr
		The red component of the vertex color.
	\param colg
		The green component of the vertex color.
	\param colb
		The blue component of the vertex color.
	\param cola
		The alpha component of the vertex color.
	\param normx
		The x-component of the vertex normal.
	\param normy
		The y-component of the vertex normal.
	\param normz
		The z-component of the vertex normal.
	\param texu
		The u-coordinate of the texture mapping.
	\param texv
		The v-coordinate of the texture mapping.
	\param texID
		The texture ID (default is -1, indicating no texture).
	\param visible
		The visibility state of the vertex (default is 1, visible).
	*******************************************************************************/
	Vertex(
		float posx = 0, float posy = 0, float posz = 0,
		float colr = 1, float colg = 0, float colb = 0, float cola = 1,
		float normx = 0, float normy = 0, float normz = 0,
		float texu = 0, float texv = 0, 
		int texArrayID = -1,
		int texLayerID = -1,
		int visible = 1
	);

	/*!*****************************************************************************
	\brief
		Constructs a vertex using `Vec3`, `Vec4`, and `Vec2` types for its
		attributes.
	\param position
		The position of the vertex in 3D space as a `Vec3`.
	\param color
		The color of the vertex as a `Vec4`.
	\param normal
		The normal vector of the vertex as a `Vec3`.
	\param texCoord
		The texture coordinates for mapping as a `Vec2`.
	\param texID
		The texture ID (default is -1, indicating no texture).
	\param visible
		The visibility state of the vertex (default is 1, visible).
	*******************************************************************************/
	Vertex(
		Vec3 position = Vec3(0, 0, 0),
		Vec4 color = Vec4(1, 0, 0, 1),
		Vec3 normal = Vec3(0, 0, 0),
		Vec2 texCoord = Vec2(0, 0),
		int texArrayID = -1,
		int texLayerID = -1,
		int visible = 1
	);

};

inline Vertex::Vertex(
	float posx, float posy, float posz,
	float colr, float colg, float colb, float cola,
	float normx, float normy, float normz,
	float texu, float texv, 
	int texArrayID,
	int texLayerID,
	int visible) :
	position(posx, posy, posz),
	color(colr, colg, colb, cola),
	normal(normx, normy, normz),
	texCoord(texu, texv), 
	texArray(texArrayID),
	texLayer(texLayerID),
	visible(visible)
{}

inline Vertex::Vertex(
	Vec3 position,
	Vec4 color,
	Vec3 normal,
	Vec2 texCoord,
	int texArrayID,
	int texLayerID,
	int visible) :
	position(position),
	color(color),
	normal(normal),
	texCoord(texCoord),
	texArray(texArrayID),
	texLayer(texLayerID),
	visible(visible)
{}