/*********************************************************************
 * \file		Entity.hpp
 * \brief		Base entity definition
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <stdint.h>

#define NO_ENTITY INT_MAX

using Entity = uint32_t;
const Entity MAX_ENTITIES = 2000;

#endif // !ENTITY_HPP
