/*********************************************************************
 * \file		LayerManager.hpp
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

#ifndef LAYER_MANAGER_HPP
#define LAYER_MANAGER_HPP
#include <bitset>
#include <array>
#include <string>

#include "Layer.hpp"

class LayerManager {
public:
    /**
     * \brief Retrieves the singleton instance of the LayerManager.
     * \return A reference to the LayerManager instance.
     */
    static LayerManager& GetInstance();

    /**
     * \brief Retrieves the name of the layer.
     */
    const std::string& GetLayerName(Layer layer) const { return layerNames[layer]; }

    /**
     * \brief Sets the name of the layer.
     */
    void SetLayerName(Layer layer, const std::string& name) { layerNames[layer] = name; }

    /**
     * \brief Checks if layer is visible.
     */
    bool IsLayerVisible(Layer layer) const { return layerVisibility[layer]; }

    /**
     * \brief Sets the layer visibility.
     */
    void SetLayerVisibility(Layer layer, bool visible) { layerVisibility[layer] = visible; }

    // Collision rules
    bool CanLayersCollide(Layer layerA, Layer layerB) const { 
        if (layerA < MAX_LAYERS && layerB < MAX_LAYERS)
            return collisionMatrix[MAX_LAYERS * layerA + layerB];
        else return true; // Handle when the layer is 'None' (16).
    }

    //void SetCollision(uint8_t layerA, uint8_t layerB, bool canCollide) {
    //    collisionMatrix[layerA][layerB] = canCollide;
    //    collisionMatrix[layerB][layerA] = canCollide; // Symmetric matrix
    //}

    std::array<std::string, MAX_LAYERS> layerNames;
    std::array<bool, MAX_LAYERS> layerVisibility;
    std::array<bool, MAX_LAYERS * MAX_LAYERS> collisionMatrix;

private:
    LayerManager();
};

#endif !LAYER_MANAGER_HPP
