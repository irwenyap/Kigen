/*********************************************************************
 * \file		AnimationSystem.hpp
 * \brief		System to handle all the animation components
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		17 November 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef ANIMATION_SYSTEM_HPP
#define ANIMATION_SYSTEM_HPP

#include "../ECS/System.hpp"

struct Renderer;
struct Animation;

/*!*****************************************************************************
\class AnimationSystem
\brief
	A class for managing rendering operations in a graphics application.

	The `AnimationSystem` class is responsible for managing meshes, textures,
	animations, and background settings. It interacts with the `GraphicsManager`
	to handle rendering tasks and provides methods for manipulating and rendering
	graphics assets in the application.
*******************************************************************************/
class AnimationSystem : public System {
public:
	/*!
	 * \brief Default constructor for the `AnimationSystem` class.
	 * Initializes the rendering system.
	 */
	AnimationSystem();
	/*!
	 * \brief Destructor for the `AnimationSystem` class.
	 * Cleans up resources used by the render system.
	 */
	~AnimationSystem();

	/*!
	 * \brief Initializes the rendering system.
	 * Sets up necessary resources and state for rendering.
	 */
	void Init();
	/*!
	 * \brief Updates the render system.
	 * Performs any necessary updates for rendering.
	 */
	void Update(double);
	/*!
	 * \brief Exits the rendering system.
	 * Cleans up resources and prepares for shutdown.
	 */
	void Exit();

	/*!
	* \brief Sets the animation for a specified entity.
	* 
	* This function sets the animation for the specified entity based on the
	* provided parameters.
	* 
	* \param entity The entity to set the animation for.
	* \param sprPerRow The number of sprites per row in the sprite sheet.
	* \param sprPerCol The number of sprites per column in the sprite sheet.
	* \param numFrames The total number of frames in the animation.
	* \param startFrame The starting frame of the animation.
	* \param endFrame The ending frame of the animation.
	* \param timePerFrame The time in seconds per frame.
	* \param isLooping Whether the animation should loop.
	*/
	void SetAnimation(Entity entity, 
		unsigned sprPerRow, unsigned sprPerCol,
		unsigned numFrames, unsigned startFrame, unsigned endFrame,
		double timePerFrame, bool isLooping = false);
	/*!
	* \brief Sets the animation for a specified entity.
	*
	* This function sets the animation for the specified entity based on the
	* provided parameters.
	*
	* \param entity The entity to set the animation for.
	* \param a The animation to set.
	*/
	void SetAnimation(Entity entity, Animation const& a);

	/*!
	* \brief Plays an animation for a specified entity.
	*
	* This function plays the specified animation for the specified entity.
	*
	* \param entity The entity whose animation will be played.
	* \param a The animation to play.
	* \param reset Whether to reset the animation to the starting frame.
	*/
	void PlayAnimation(Renderer& entity, Animation& a, bool reset = false, bool refresh = false, bool playOnce = true, bool isLooping = false);
	/*!
	* \brief Plays an animation for a specified entity.
	*
	* This overload of the `PlayAnimation` function plays the specified animation
	* for the specified entity.
	*
	* \param entity The entity whose animation will be played.
	* \param reset Whether to reset the animation to the starting frame.
	*/
	void PlayAnimation(Entity entity, bool reset = false, bool refresh = false, bool playOnce = true, bool isLooping = false);

	/*!
	 * \brief Updates the animation for a specified entity based on time.
	 *
	 * This function updates the animation state for the specified entity based
	 * on the elapsed time since the last update.
	 *
	 * \param r The renderer component containing the animation to update.
	 * \param a The animation component to update.
	 * \param dt The elapsed time since the last update.
	 */
	void UpdateAnimation(Renderer& r, Animation& a, double dt);

	/*!
	* \brief Stops the animation for a specified entity.
	*
	* This function stops the animation for the specified entity.
	*
	* \param r The renderer component containing the animation to stop.
	* \param a The animation component to stop.
	* \param reset Whether to reset the animation to the starting frame.
	*/
	void PauseAnimation(Renderer& r, Animation& a, bool reset = false);
	/*!
	* \brief Stops the animation for a specified entity.
	* 
	* This overload of the `StopAnimation` function stops the animation for the
	* specified entity.
	* 
	* \param entity The entity whose animation will be stopped.
	* \param reset Whether to reset the animation to the starting frame.
	*/
	void PauseAnimation(Entity entity, bool reset = false);

	/*!
	* \brief Resets the animation to the starting frame.
	* 
	* This function resets the animation for the specified entity.
	* 
	* \param r The renderer component containing the animation to reset.
	* \param a The animation component to reset.
	*/
	void ResetAnimation(Renderer& r, Animation& a);

	/*!
	* \brief Assigns texture coordinates to a mesh based on the current animation frame.
	*
	* This function assigns texture coordinates to the vertices of a mesh based on the
	* current frame of the animation.
	*
	* \param r The renderer component containing the mesh to update.
	* \param a The animation component containing the animation data.
	*/
	void AssignTexCoordsToMesh(Renderer& r, Animation& a);
}; 

#endif // !ANIMATION_SYSTEM_HPP
