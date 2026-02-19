/*********************************************************************
 * \file		SortingLayerManager.hpp
 * \brief		Handles management of sorting layers
 *
 * \author		woonli.wong, 2301308 (woonli.wong@digipen.edu)
 * \date		15 January 2025
 *
 * \copyright	Copyright(C) 2025 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef SORTINGLAYER_MANAGER_HPP
#define SORTINGLAYER_MANAGER_HPP

#include "SortingLayer.hpp"
#include <array>
#include <functional>
#include "../Graphics/BatchData.hpp"

class SortingLayerManager {
public:
    /**
     * \brief Retrieves the singleton instance of the SortingLayerManager.
     * \return A reference to the SortingLayerManager instance.
     */
    static SortingLayerManager& GetInstance();

    std::array<std::string, MAX_SORTING_LAYERS> srtgLayerNames;

private:
    SortingLayerManager();
};

#endif !SORTINGLAYER_MANAGER_HPP
