/*********************************************************************
 * \file		AudioSource.hpp
 * \brief		Defines the base AudioSource component
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		01 November 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef AUDIO_SOURCE_HPP
#define	AUDIO_SOURCE_HPP

#include <string>

struct AudioSource {
	std::string audioClipUUID = "";

	bool isPlaying = false;
	bool isLooping = false;
};


#endif // !AUDIO_SOURCE_HPP
