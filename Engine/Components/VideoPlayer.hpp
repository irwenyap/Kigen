/*********************************************************************
 * \file		VideoPlayer.hpp
 * \brief		Defines the video player component
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		30 January 2025
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef VIDEO_PLAYER_HPP
#define VIDEO_PLAYER_HPP

#include <string>

#include "../Video/VideoClip.hpp"

struct VideoPlayer {
	std::string videoClipUUID = "";
	
	VideoClip videoClip;

	bool isPlaying = false;
	bool playOnAwake = false;
	bool isLooping = false;
	double timer = 0;

	size_t currentFrame = 0;
	size_t meshID;
};

#endif // !VIDEO_PLAYER_HPP
