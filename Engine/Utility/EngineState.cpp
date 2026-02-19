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

#include "EngineState.hpp"

// Definition of the global variable
EngineState engineState = EngineState::STOPPED;
GameWindowMode gameWindowMode = GameWindowMode::ENGINE;
bool onStart = true;
bool onSwitch = false;