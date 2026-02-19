/*********************************************************************
 * \file		Scene.hpp
 * \brief		Interface for all scenes
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		5 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef SCENE_HPP
#define SCENE_HPP

class IScene {
public:
	IScene() {}
	virtual ~IScene() {}

	virtual void Initialize() = 0;
	virtual void Update(double dt, double fixedDt, int numOfSteps) = 0;
	virtual void Exit() = 0;
};

#endif // !SCENE_HPP
