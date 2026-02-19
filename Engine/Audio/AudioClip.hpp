/*********************************************************************
 * \file	    AudioClip.hpp
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

#ifndef AUDIO_CLIP_HPP
#define AUDIO_CLIP_HPP

#include "../Asset.hpp"

#include <fmod.hpp>

#include "../Utility/MetadataHandler.hpp"
#include "AudioManager.hpp"

class AudioClip : public IAsset {
public:
    FMOD::Sound* sound = nullptr;

    AudioClip() = default;
    ~AudioClip() override;

    bool LoadFromFile(const std::string& filePath) override;
};



#endif // !AUDIO_CLIP_HPP