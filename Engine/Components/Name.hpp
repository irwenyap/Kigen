/*********************************************************************
 * \file		Name.hpp
 * \brief		Defines the base Name component
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		01 November 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef NAME_HPP
#define NAME_HPP

#include <string>

struct Name {
	std::string name{"Unnamed Entity"};
	std::string prefabID{ "" };

	// Hidden Vars
	std::string prefabPath;
};

#endif // !NAME_HPP
