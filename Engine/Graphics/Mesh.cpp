/*********************************************************************
 * \file	Mesh.cpp
 * \brief
 *      This file defines the `Mesh` class, which represents a
 *      collection of vertices that can be rendered in a graphics
 *      application. It includes functionalities for managing vertex
 *      data, textures, colors, and visibility for rendering purposes.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	3 October 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#include "Mesh.hpp"
#include "TextureArray.hpp"

Mesh::Mesh(
	std::vector<Vertex> const& vertices, 
	std::vector<unsigned int> const& indices, 
	std::vector<Vec3> const& modelSpacePosition, 
	size_t batchID = static_cast<size_t>(-1))
	: id(static_cast<size_t>(-1)), 
	vertices(vertices), indices(indices), modelSpacePosition(modelSpacePosition), 
	batchID(batchID)
{
	static size_t idCounter = 0;
	this->id = idCounter++;
	cumulativeScale = Vec2(1.0f, 1.0f);
	cumulativeRotation = 0.0f;
}

Mesh::~Mesh()
{

}

//void Mesh::SetTexture(int texArrayIndex, int texLayerIndex)
//{
//	for (auto& vertex : vertices) {
//		vertex.texArray = texArrayIndex;
//		vertex.texLayer = texLayerIndex;
//	}
//}
//
//void Mesh::SetColor(Vec4 color)
//{
//	for (auto& vertex : vertices) {
//		vertex.color = color;
//	}
//}
//
//void Mesh::SetVisibility(bool visibility)
//{
//	for (auto& vertex : vertices) {
//		vertex.visible = static_cast<int>(visibility);
//	}
//}