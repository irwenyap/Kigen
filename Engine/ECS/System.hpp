/*********************************************************************
 * \file		System.hpp
 * \brief		System that stores a set of entities containing 
 *              comnponents related to the system
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

#ifndef SYSTEM_H
#define	SYSTEM_H

#include "Entity.hpp"
#include <set>

/**
 * \class System
 * \brief Represents a system that operates on a set of entities with specific components.
 *
 * The System class stores a set of entities that this system operates on. Each system is responsible
 * for processing entities that match certain component signatures.
 */
class System {
public:
    std::set<Entity> m_entities; /**< A set of entities that this system processes. */

};


#endif // !SYSTEM_H