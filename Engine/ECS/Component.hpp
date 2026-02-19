/*********************************************************************
 * \file		Component.hpp
 * \brief		Base component definition
 *
 * \author		Yap Zi Yang Irwen, y.ziyangirwen, 2301345
 * \email		y.ziyangirwen@digipen.edu
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <stdint.h>

/**
 * \typedef ComponentType
 * \brief An alias for the component type identifier.
 *
 * Each component type is assigned a unique ID, which is represented by an 8-bit unsigned integer.
 */
using ComponentType = uint8_t;

/**
 * \brief The maximum number of component types that can be registered in the system.
 */
const ComponentType MAX_COMPONENTS = 32;

#endif // !COMPONENT_HPP
