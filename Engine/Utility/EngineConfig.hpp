/*********************************************************************
 * \file		EngineConfig.hpp
 * \brief		Store engine configurations
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef ENGINE_CONFIG_HPP
#define ENGINE_CONFIG_HPP

#include <string>

struct EngineConfig {
	std::string windowTitle;
	int windowWidth;
	int windowHeight;
	std::string sceneName;
	bool isFullscreen;
	std::string graphicsQuality;
};

#endif // !ENGINE_SETTINGS_HPP
