/*********************************************************************
 * \file	    AudioClip.cpp
 * \brief       Base AudioClip asset declaration
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/
#include "AudioClip.hpp"

AudioClip::~AudioClip() {
    if (sound) {
        sound->release();
    }
}

bool AudioClip::LoadFromFile(const std::string& filePath) {
    sound = AudioManager::GetInstance().LoadSound(filePath);
    return sound != nullptr;
}
