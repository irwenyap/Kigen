/*********************************************************************
 * \file		Camera.hpp
 * \brief		Defines the base Camera component
 * 
 * \author		t.yongchin, 2301359 (t.yongchin@digipen.edu)
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		20 January 2025
 *
 * \copyright	Copyright(C) 2025 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/
#pragma once

#include "Vec3.hpp"
#include "Mat4.hpp"

struct Camera {

	Camera(bool isMainCamera = false);

	glm::mat4 viewMtx;
	glm::mat4 projectionMtx;

	float zoom;
	float width;
	float height;

	bool isMainCamera;
	bool isActive;
	bool hasUpdatedView;
	bool hasUpdatedProj;

	float bloomIntensity{ 7.0f };

	float vignetteStrength{ 0.8f };
	float vignetteSoftness{ 0.9f };
	Vec2 vignetteCenter{ 0.5f, 0.34f };
};

inline Camera::Camera(bool isMainCamera)
	: viewMtx(1.0f), projectionMtx(1.0f), width(1920.f), height(1080.f), isMainCamera(isMainCamera), 
	isActive(false), hasUpdatedView(false), hasUpdatedProj(false), zoom(1.0f)
{
}
