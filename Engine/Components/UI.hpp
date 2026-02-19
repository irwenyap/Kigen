/*********************************************************************
 * \file	UI.hpp
 * \brief	Defines the base UI component
  *
  * \author	t.yongchin, 2301359
  * \email	t.yongchin@digipen.edu
  * \date	15 November 2024
  *
  * Copyright(C) 2024 DigiPen Institute of Technology.
  * Reproduction or disclosure of this file or its contents without the
  * prior written consent of DigiPen Institute of Technology is prohibited.
  *********************************************************************/
#pragma once

#include "Math.hpp"

  /*********************************************************************
  * \struct	UI
  * \brief	Defines the UI component that is used to store the 
  *			position, size, scale and rotation of a UI element.
  *********************************************************************/
struct UI {

	/*
	* \brief	Constructor for the UI component
	* \param	position	Position of the UI element
	* \param	size		Size of the UI element
	* \param	scale		Scale of the UI element
	* \param	rotation	Rotation of the UI element
	*/
	UI(
		Vec3 position = Vec3(0.5f, 0.5f, -1.f),
		Vec2 size = Vec2(0.25f, 0.25f),
		Vec2 scale = Vec2(1.0f, 1.0f),
		float rotation = 0.0f
	);
		
	// Position is stored in normalised screen coordinates
	Vec3 position;
	// Size is stored in normalised screen coordinates
	Vec2 size;
	// Scale is stored as a multiplier that is applied to the size
	Vec2 scale;

	// Rotation currently not supported
	float rotation; 

	// Flag to indicate if the UI element needs to be updated
	bool isUpdated;
};

inline UI::UI(Vec3 position, Vec2 size, Vec2 scale, float rotation)
	: position(position), size(size), scale(scale), rotation(rotation), isUpdated(false)
{
}