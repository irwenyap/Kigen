/*********************************************************************
 * \file	CameraSystem.cpp
 * \brief
 *      This file defines the CameraSystem class.
 *
 *		The CameraSystem class is responsible for updating the
 *		view and projection matrices of the camera.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	29 January 2025
 *
 * Copyright(C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#include "CameraSystem.hpp"

#include "../ECS/ECSManager.hpp"
#include "../Components/Camera.hpp"
#include "../Components/Transform.hpp"

#include "../Graphics/GraphicsManager.hpp"

CameraSystem::CameraSystem() : mainCameraSet(false), mainCameraEntity(static_cast<uint32_t>(-1))
{
}

CameraSystem::~CameraSystem() 
{
}

void CameraSystem::Init() 
{
	for (auto& entity : m_entities) {
		auto& camera = ECSManager::GetInstance().GetComponent<Camera>(entity);
		// The main camera is only set once during initialization
		if (camera.isMainCamera) {
			mainCameraEntity = entity;
			mainCameraSet = true;
		}
		else if (camera.isActive) {
			SetActiveCamera(entity);
		}
	}
}

void CameraSystem::Update() 
{
	// Finds and sets the main camera & active camera
	// If there are multiple entities with the main camera flag set, 
	// the first one found will be set as the main camera
	// If there are multiple entities with the active camera flag set,
	// the last one found will be set as the active camera
	mainCameraSet = false;
	for (auto& entity : m_entities) {
		auto& camera = ECSManager::GetInstance().GetComponent<Camera>(entity);
		if (camera.isMainCamera) {
			if (mainCameraSet) {
				camera.isMainCamera = false;
			}
			else {
				mainCameraEntity = entity;
				mainCameraSet = true;
			}
		}
		if (camera.isActive) {
			SetActiveCamera(entity);
		}
	}

	{
		// Check for the case where the active camera's isActive is set to false
		auto activeCamera = ECSManager::GetInstance().TryGetComponent<Camera>(GraphicsManager::GetInstance().activeCamera);
		if (activeCamera.has_value()) {
			if (activeCamera->get().isActive == false) {
				DisableActiveCamera();
			}
		}
	}
	
	// We need to retrieve the active camera again in case it has been changed above
	// Update the projection and view matrices if the active camera has been updated
	auto activeCamera = ECSManager::GetInstance().TryGetComponent<Camera>(GraphicsManager::GetInstance().activeCamera);
	if (activeCamera.has_value()) {
		if (activeCamera->get().hasUpdatedView) {
			UpdateViewMatrix();
			activeCamera->get().hasUpdatedView = false;
		}
		if (activeCamera->get().hasUpdatedProj) {
			UpdateProjectionMatrix();
			activeCamera->get().hasUpdatedProj = false;
		}
	}
}

void CameraSystem::Exit() 
{
	
}

void CameraSystem::SetMainCamera(Entity entity)
{
	auto currentMainCamera = ECSManager::GetInstance().TryGetComponent<Camera>(mainCameraEntity);
	if (currentMainCamera.has_value()) {
		currentMainCamera->get().isMainCamera = false;
	}

	auto newMainCamera = ECSManager::GetInstance().TryGetComponent<Camera>(entity);
	if (newMainCamera.has_value()) {
		newMainCamera->get().isMainCamera = true;
		mainCameraEntity = entity;
		mainCameraSet = true;
	}
	else {
		if (currentMainCamera.has_value())
			currentMainCamera->get().isMainCamera = true;
	}
}

void CameraSystem::SetActiveCamera(Entity entity)
{
	// If the entity is already the active camera, return
	if (GraphicsManager::GetInstance().activeCamera == entity) return;

	// Deactivate the current active camera
	auto camera = ECSManager::GetInstance().TryGetComponent<Camera>(
		GraphicsManager::GetInstance().activeCamera
	);
	if (camera.has_value()) {
		camera->get().isActive = false;
	}

	// Set the new active camera
	GraphicsManager::GetInstance().activeCamera = entity;
	auto& newCamera = ECSManager::GetInstance().GetComponent<Camera>(entity);
	newCamera.isActive = true;
	newCamera.hasUpdatedView = true;
	newCamera.hasUpdatedProj = true;

	if (!mainCameraSet) {
		mainCameraEntity = entity;
		mainCameraSet = true;
		newCamera.isMainCamera = true;
	}
}

void CameraSystem::DisableActiveCamera()
{
	// If the active camera is the main camera
	if (GraphicsManager::GetInstance().activeCamera == mainCameraEntity) {
		return;
	}
	
	auto& camera = ECSManager::GetInstance().GetComponent<Camera>(GraphicsManager::GetInstance().activeCamera);
	camera.isActive = false;

	auto& mainCamera = ECSManager::GetInstance().GetComponent<Camera>(mainCameraEntity);
	mainCamera.isActive = true;

	GraphicsManager::GetInstance().activeCamera = mainCameraEntity;
}

void CameraSystem::SetZoom(float zoomFactor)
{
	auto& camera = ECSManager::GetInstance().GetComponent<Camera>(
		GraphicsManager::GetInstance().activeCamera
	);
	camera.zoom = zoomFactor;
	
	camera.hasUpdatedProj = true;
}

void CameraSystem::SetBloom(float bloomIntensity)
{
	auto& camera = ECSManager::GetInstance().GetComponent<Camera>(
		GraphicsManager::GetInstance().activeCamera
	);
	camera.bloomIntensity = bloomIntensity;
}

void CameraSystem::SetWidthHeight(float width, float height)
{
	auto& camera = ECSManager::GetInstance().GetComponent<Camera>(
		GraphicsManager::GetInstance().activeCamera
	);
	camera.width = width;
	camera.height = height;

	camera.hasUpdatedProj = true;
}

void CameraSystem::UpdateProjectionMatrix()
{
	auto& activeCamera = ECSManager::GetInstance().GetComponent<Camera>(GraphicsManager::GetInstance().activeCamera);
	float halfWidth = (activeCamera.width / 2.0f) / activeCamera.zoom;
	float halfHeight = (activeCamera.height / 2.0f) / activeCamera.zoom;

	float left = -halfWidth;
	float right = halfWidth;
	float bottom = -halfHeight;
	float top = halfHeight;

	float nearPlane = 0.1f;
	float farPlane = 100.0f;

	activeCamera.projectionMtx = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
}

void CameraSystem::UpdateViewMatrix()
{
	auto& activeCamera = ECSManager::GetInstance().GetComponent<Camera>(GraphicsManager::GetInstance().activeCamera);
	auto& transform = ECSManager::GetInstance().GetComponent<Transform>(GraphicsManager::GetInstance().activeCamera);

	glm::vec3 cameraPos{ transform.position.x, transform.position.y, 1.0f };
	glm::vec3 cameraTarget{ transform.position.x, transform.position.y, 0.0f };
	glm::vec3 cameraUp{ 0.0f, 1.0f, 0.0f };

	activeCamera.viewMtx = glm::lookAt(cameraPos, cameraTarget, cameraUp);
}

Entity CameraSystem::GetActiveCamera() const
{
	return GraphicsManager::GetInstance().activeCamera;
}