/*********************************************************************
 * \file		Signature.hpp
 * \brief		Definition of signature representing the components
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

#ifndef SIGNATURE_HPP
#define SIGNATURE_HPP

#include <bitset>
#include "Component.hpp"

/**
 * \typedef Signature
 * \brief A bitset used to represent the combination of components an entity has.
 *
 * Each bit in the Signature corresponds to a different component type. A system uses signatures
 * to determine whether an entity has the necessary components for it to be processed.
 */
using Signature = std::bitset<MAX_COMPONENTS>;

#endif