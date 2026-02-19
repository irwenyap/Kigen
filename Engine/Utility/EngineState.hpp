/*********************************************************************
 * \file		EngineState.hpp
 * \brief		Stores engine states
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef ENGINE_STATE_HPP
#define ENGINE_STATE_HPP

enum class EngineState {
	STOPPED = 0,
	PLAYING,
	PAUSED
};

enum class GameWindowMode {
	ENGINE = 0,
	WINDOWED,
	FULLSCREEN
};

extern EngineState engineState;
extern GameWindowMode gameWindowMode;

#endif !ENGINE_STATE_HPP
