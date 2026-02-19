/*********************************************************************
 * \file		ComponentIDGenerator.hpp
 * \brief		Generate id for each component for future
 *              referencing
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/
#ifndef COMPONENT_ID_GENERATOR_HPP
#define COMPONENT_ID_GENERATOR_HPP

#include <cstdint>

class ComponentIDGenerator {
public:
    /**
     * \brief Generates a unique ID for a component type.
     * 
     * \param prefix The prefix character indicating the component type.
     * 
     * \return A unique uint32_t ID for the component.
     */
    static uint32_t GenerateID(char prefix);

private:
    /**
     * \brief Validates whether the prefix character is allowed.
     * 
     * \param prefix The prefix character.
     * 
     * \return True if valid, false otherwise.
     */
    static bool IsValidPrefix(char prefix);

    /**
     * \brief Generates a random number within the given range.
     * 
     * \param min The minimum value (inclusive).
     * \param max The maximum value (inclusive).
     * 
     * \return A random number within the range.
     */
    static uint32_t GenerateRandomNumber(uint32_t min, uint32_t max);
};

#endif // !COMPONENT_ID_GENERATOR_HPP