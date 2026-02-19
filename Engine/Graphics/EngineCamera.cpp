/*********************************************************************
 * \file	EngineCamera.cpp
 * \brief
 *      This file defines the `EngineCamera` class.
 *
 * \author	Wong Woon Li, 2301308, t.yongchin, 2301359
 * \email	woonli.wong@digipen.edu, t.yongchin@digipen.edu
 * \date	5 Nov 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#include "EngineCamera.hpp"

EngineCamera::EngineCamera()
	: position(0.0f, 0.0f, 1.0f), target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f), zoom(1.0f), screenWidth(0.0f), screenHeight(0.0f), projection(1.0f), transform(1.0f)
{
}

EngineCamera::~EngineCamera()
{
}

void EngineCamera::Init(int width, int height)
{
	this->screenWidth = static_cast<float>(width);
	this->screenHeight = static_cast<float>(height);

	position = glm::vec3(0.0f, 0.0f, 1.0f);
	target = glm::vec3(0.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);

	SetProjectionMatrix();
}

void EngineCamera::Update()
{
	
}

glm::mat4 EngineCamera::GetViewMatrix()
{
	return glm::lookAt(position, target, up);
}

glm::mat4 EngineCamera::GetProjectionMatrix()
{
	return projection;
}

void EngineCamera::SetPosition(glm::vec3 pos)
{
	this->position = pos;
}

void EngineCamera::SetProjectionMatrix() 
{
	float halfWidth = (screenWidth / 2.0f) / zoom;
	float halfHeight = (screenHeight / 2.0f) / zoom;

	float left = -halfWidth;
	float right = halfWidth;
	float bottom = -halfHeight;
	float top = halfHeight;

	float near = 0.1f;                     // Near clipping plane
	float far = 100.0f;                    // Far clipping plane

	//left; right; bottom; top; near; far;

	// Orthographic projection matrix
	//projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
	projection = glm::ortho(left, right, bottom, top, near, far);
}

void EngineCamera::SetFullScreenProjectionMatrix()
{
	float halfWidth = (1920.f / 2.0f) / zoom;
	float halfHeight = (1080.f / 2.0f) / zoom;

	float left = -halfWidth;
	float right = halfWidth;
	float bottom = -halfHeight;
	float top = halfHeight;

	float near = 0.1f;                     // Near clipping plane
	float far = 100.0f;                    // Far clipping plane

	//left; right; bottom; top; near; far;

	// Orthographic projection matrix
	//projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
	projection = glm::ortho(left, right, bottom, top, near, far);
}

void EngineCamera::MoveUp(float distance)
{
	position.y += distance;
	target.y += distance;
}

void EngineCamera::MoveDown(float distance)
{
	position.y -= distance;
	target.y -= distance;
}

void EngineCamera::MoveRight(float distance)
{
	position.x += distance;
	target.x += distance;
}

void EngineCamera::MoveLeft(float distance)
{
	position.x -= distance;
	target.x -= distance;
}

void EngineCamera::SetPosition(float x, float y)
{
	position = glm::vec3(x, y, 1.f);
	target = glm::vec3(x, y, 0.f);
}

void EngineCamera::SetZoom(float zoomFactor)
{
	zoom = glm::max(0.1f, zoom + zoomFactor); // Prevent zooming out too far
	SetProjectionMatrix();
}
