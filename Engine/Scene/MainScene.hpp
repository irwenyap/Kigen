/*********************************************************************
 * \file		MainScene.hpp
 * \brief		The main scene
 *
 * \author		Yap Zi Yang Irwen, y.ziyangirwen, 2301345
 * \email		y.ziyangirwen@digipen.edu
 * \date		1 September 2024
 * 
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef MAIN_SCENE_HPP
#define MAIN_SCENE_HPP

#include "Scene.hpp"

class MainScene : public IScene {
public:
	void Initialize();
	void Update(double dt, double fixedDt, int numOfSteps);
	void Exit();
};

#endif