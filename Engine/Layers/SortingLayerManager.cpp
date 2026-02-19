/*********************************************************************
 * \file		SortingLayerManager.cpp
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

#include "SortingLayerManager.hpp"

SortingLayerManager& SortingLayerManager::GetInstance() {
    static SortingLayerManager slm;
    return slm;
}

SortingLayerManager::SortingLayerManager() {
    for (SortingLayer i = 0; i < MAX_SORTING_LAYERS; i++) {
        srtgLayerNames[i] = "Layer " + std::to_string(i);
    }
}