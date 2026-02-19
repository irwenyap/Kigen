/*********************************************************************
 * \file	EngineCamera.hpp
 * \brief
 *      This file declares the `EngineCamera` class.
 *
 * \author	Wong Woon Li, 2301308, t.yongchin, 2301359
 * \email	woonli.wong@digipen.edu, t.yongchin@digipen.edu
 * \date	5 Nov 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // For value_ptr

class EngineCamera
{
public:
    EngineCamera();
    ~EngineCamera();
    
    /*
    * \brief Initialize the camera with the given width and height
    * 
    * \param width The width of the camera
    * \param height The height of the camera
    */
    void Init(int width, int height);

    /*
    * \brief Update the camera
    */
    void Update();
    
    /*
    * \brief Return the view matrix of the camera
    * 
    * \return The view matrix of the camera
    */
    glm::mat4 GetViewMatrix();

    /*
    * \brief Return the projection matrix of the camera
    * 
    * \return The projection matrix of the camera
    */
    glm::mat4 GetProjectionMatrix();

    /*
    * \brief Set the position of the camera
    * 
    * \param pos The new position of the camera
    */
    void SetPosition(glm::vec3 pos);
    /*
    * \brief Set the position of the camera
    * Overloaded to accept individual x and y coordinates
    *
    * \param x The new x-coordinate of the camera
    * \param y The new y-coordinate of the camera
    */
    void SetPosition(float x, float y);

    /*
    * \brief Set the projection matrix of the camera
   	*/
    void SetProjectionMatrix();

    void SetFullScreenProjectionMatrix();

    /*
    * \brief Move the camera up by the given distance
    * 
    * \param distance The distance to move the camera up
    */
    void MoveUp(float distance);
    /*
    * \brief Move the camera down by the given distance
    *
    * \param distance The distance to move the camera up
    */
    void MoveDown(float distance);
    /*
    * \brief Move the camera left by the given distance
    *
    * \param distance The distance to move the camera up
    */
    void MoveLeft(float distance);
    /*
    * \brief Move the camera right by the given distance
    *
    * \param distance The distance to move the camera up
    */
    void MoveRight(float distance);

    /*
    * \brief Set the zoom factor of the camera
    * 
    * \param zoomFactor The new zoom factor of the camera
	*/
    void SetZoom(float zoomFactor);

public:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    glm::mat4 projection;

    float screenWidth;
    float screenHeight;

    float zoom = 1.0f;

    // test
    glm::mat4 transform;
};