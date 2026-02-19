/*********************************************************************
 * \file		LayerManager.cpp
 * \brief		Handles management of layers
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#include "LayerManager.hpp"

LayerManager& LayerManager::GetInstance()
{
    static LayerManager lm;
    return lm;
}

LayerManager::LayerManager()
{
    for (Layer i = 0; i < MAX_LAYERS; i++) {
        layerNames[i] = "Layer " + std::to_string(i);
        layerVisibility[i] = true;
    }

    for (Layer i = 0; i < MAX_LAYERS; i++) {
        for (Layer j = 0; j < MAX_LAYERS; j++) {
            collisionMatrix[i * MAX_LAYERS + j] = true;
        }
    }
}
