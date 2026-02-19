/*********************************************************************
 * \file	CameraSystem.hpp
 * \brief
 *      This file declares the CameraSystem class.
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
#pragma once

#include "../ECS/System.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // For value_ptr

 /*!*****************************************************************************
 \class CameraSystem
 \brief
	 A class for managing the camera components for all entities.

	 The `CameraSystem` class is responsible selecting the active camera
	 and updating the view and projection matrices for rendering.
 *******************************************************************************/
class CameraSystem : public System {

public:

	/*
	* \brief Constructor for the CameraSystem
	*/
	CameraSystem();

	/*
	* \brief Destructor for the CameraSystem
	*/
	~CameraSystem();

	/*
	* \brief Initialize the CameraSystem
	*/
	void Init();

	/*
	* \brief Update the CameraSystem
	* \param dt the delta time
	*/
	void Update();

	/*
	* \brief Exit the CameraSystem
	*/
	void Exit();

	/*
	* \brief Set the main camera to the one attached to the entity
	*
	* \param entity The entity with the camera component
	*/
	void SetMainCamera(Entity entity);

	/*
	* \brief Set the active camera to the one attached to the entity
	* 
	* \param entity The entity with the camera component
	*/
	void SetActiveCamera(Entity entity);

	/*
	* \brief Disable the active camera
	* 
	* Disables the active camera, setting the main camera as the active camera
	*/
	void DisableActiveCamera();

	/*
	* \brief Set the zoom factor of the camera
	* 
	* \param zoomFactor The zoom factor to set the camera to
	*/
	void SetZoom(float zoomFactor);

	void SetBloom(float bloomIntensity);

	/*
	* \brief Set the width and height of the camera
	* 
	* \param width The width of the camera
	* \param height The height of the camera
	*/
	void SetWidthHeight(float width, float height);

	/*
	* \brief Update the projection matrix of the camera
	*/
	void UpdateProjectionMatrix();

	/*
	* \brief Update the view matrix of the camera
	*/
	void UpdateViewMatrix();

	/*
	* \brief Get the active camera entity
	*/
	Entity GetActiveCamera() const;
	
private:
	Entity mainCameraEntity;	//!< The main camera entity (will default to this if no active camera is set)

	bool mainCameraSet;			//!< Flag to check if the main camera has been set
};