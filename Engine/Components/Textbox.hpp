/*********************************************************************
 * \file	Textbox.hpp
 * \brief	Defines the base Textbox component
  *
  * \author	t.yongchin, 2301359
  * \email	t.yongchin@digipen.edu
  * \date	15 November 2024
  *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
  *********************************************************************/
#pragma once

#include <string>
#include <vector>
#include "Math.hpp"

  /*********************************************************************
  * \struct	Textbox
  * \brief	Defines the Textbox component that is used to store
  *			the mesh IDs of the text, the font ID, the color of the text,
  *			and the text itself.
  *********************************************************************/
struct Textbox {

	/*
	* \brief	Constructor for Textbox component
	* \param	text		The text to be displayed
	* \param	fontUUID	The UUID of the font to be used
	* \param	color		The color of the text
	*/
	Textbox(
		std::string text = "", 
		std::string fontUUID = "19362e7adb5-f5d82ff0869c4802-662bdabeda82c652",
		Vec3 color = Vec3(1.f, 0.f, 0.f),
		bool centerAligned = false
		);

	std::string text;
	std::string fontUUID;
	Vec3 color;
	bool centerAligned;

	std::vector<size_t> meshIDs;
};

inline Textbox::Textbox(std::string text, std::string fontUUID, Vec3 color, bool centerAligned)
	: text(text), meshIDs(), color(color), fontUUID(fontUUID), centerAligned(centerAligned)
{
	meshIDs.reserve(text.size());
}