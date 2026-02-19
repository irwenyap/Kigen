/*********************************************************************
 * \file		ComponentIDGenerator.cpp
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
#include "ComponentIDGenerator.hpp"

#include <random>
#include <stdexcept>


uint32_t ComponentIDGenerator::GenerateID(char prefix) {
    // Validate the prefix (optional, ensure valid component type prefixes)
    //if (!IsValidPrefix(prefix)) {
    //    throw std::invalid_argument("Invalid prefix character for component type.");
    //}

    // Convert the prefix to its ASCII value
    uint32_t prefixValue = static_cast<uint32_t>(prefix);
    uint32_t randomNumber = GenerateRandomNumber(10000, 99999);
    return (prefixValue * 100000) + randomNumber;
}

bool ComponentIDGenerator::IsValidPrefix(char prefix) {
    return prefix == 't' || prefix == 'r' || prefix == 'p' || prefix == 'c';
}

uint32_t ComponentIDGenerator::GenerateRandomNumber(uint32_t min, uint32_t max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(rng);
}