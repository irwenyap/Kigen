/*********************************************************************
 * \file		AudioSystem.hpp
 * \brief		System to handle all the AudioSource components
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		17 November 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef AUDIO_SYSTEM_HPP
#define	AUDIO_SYSTEM_HPP

#include "../ECS/System.hpp"

class AudioSystem : public System {
public:

	void Init();
	void Update(double);
	void Exit();

};

#endif !AUDIO_SYSTEM_HPP
