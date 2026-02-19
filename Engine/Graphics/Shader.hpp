/*********************************************************************
 * \file	Shader.hpp
 * \brief
 *      This file declares the `Shader` class, which is responsible
 *      for loading and managing OpenGL shaders. It provides functionalities
 *      to load vertex and fragment shaders from specified file paths,
 *      compile them, and link them into a shader program that can be
 *      used for rendering graphics in OpenGL.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	3 October 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#pragma once

#include "../Asset.hpp"

 // Forward declaration
 typedef unsigned int GLuint;

 /*!*****************************************************************************
 \class Shader
 \brief
	 A class for managing OpenGL shaders.

	 The `Shader` class is designed to load, compile, and link vertex and fragment
	 shaders, providing an interface to use the shader program in rendering operations.
 *******************************************************************************/
class Shader : public IAsset
{
public:

    Shader();

    ~Shader();

    /*!
     * \brief Load shaders from specified file paths and compile them.
     *
     * This function reads the shader code from the given vertex and fragment
     * shader file paths, compiles the shaders, and links them into a single
     * shader program for use in OpenGL rendering.
     *
     * \param path The file path to the vertex shader.
     */
    bool LoadFromFile(const std::string& path) override;

    /*!
    * \brief Use the shader program for rendering.
    *
    * This function sets the current shader program to the one managed by this
    * `Shader` instance, allowing it to be used for rendering operations.
    */
	void Use();

    /*!
	 * \brief Delete the shader program.
	 */
    void DeleteProgram();
	
public:
    GLuint id_gl; //!< The ID of the shader program in OpenGL.
    size_t id;
};