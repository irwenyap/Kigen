/*********************************************************************
 * \file		VideoPlayerSystem.hpp
 * \brief		System to handle all the VideoPlayer components
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		30 January 2025
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef VIDEO_PLAYER_SYSTEM_HPP
#define	VIDEO_PLAYER_SYSTEM_HPP

#include "../ECS/System.hpp"

class VideoPlayerSystem : public System {
public:

	void Init();
	void Update(double);
	void Exit();

};

#endif !VIDEO_PLAYER_SYSTEM_HPP