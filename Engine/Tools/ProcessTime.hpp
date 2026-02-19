/*********************************************************************
 * \file	ProcessTime.hpp
 * \brief	Defines a ProcessTime class that process all time related functionality like fps and process time info.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#ifndef _PROCESS_TIME_H
#define _PROCESS_TIME_H

#include <GLFW/glfw3.h>
#include <map>
#include <sstream>
#include <chrono>
#include "Timer.hpp"


class ProcessTime {
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using Duration = std::chrono::duration<double>;

public:

	enum SystemType{
		MISC = 0,
		PHYSICS,
		GRAPHICS,
		AUDIO
	};


	static double fps;
	static TimePoint startTime;

	static std::map<SystemType, double> deltaMap;
	static std::map<SystemType, double> percentMap;

	static Timer deltaTime; // time taken to complete most recent game loop
	TimePoint start;
	TimePoint end;

	/**
	 * @brief Initializes the delta time map for various system types.
	 *
	 * This method sets the initial delta time values for all system types
	 * (MISC, PHYSICS, GRAPHICS, AUDIO) to zero.
	 */
	static void CreateDelta();

	/**
	 * @brief Updates the frame rate and calculates elapsed time since the last update.
	 *
	 * This method computes the time elapsed since the last frame update and calculates the
	 * frames per second (FPS) based on the number of game loop iterations. The FPS is updated
	 * at least every specified interval, allowing for smooth and consistent performance metrics.
	 *
	 * @param fps_calc_interval The time interval (in seconds) for calculating the FPS.
	 *                          Values less than 0 are clamped to 0, and values greater than
	 *                          10 are clamped to 10.
	 */
	static void UpdateTime(double fps_calc_interval = 1.0);

	/**
	 * @brief Records the start time for delta calculations.
	 *
	 * This method captures the current time at a high resolution to
	 * mark the beginning of a processing delta. It is typically called
	 * before beginning a time-consuming process to later calculate
	 * how long that process took.
	 */
	void StartDelta();

	/**
	 * @brief Records the end time for a specific system type's processing delta.
	 *
	 * This method calculates the duration of the processing for the specified
	 * system type by comparing the current time to the recorded start time.
	 * The delta time is stored in the delta map for the corresponding system type.
	 *
	 * @param type The type of system (MISC, PHYSICS, GRAPHICS, AUDIO) for which
	 *             to record the processing time.
	 */
	void EndDelta(SystemType);

	/**
	 * @brief Calculates the percentage of total processing time for each system type.
	 *
	 * This method computes the percentage of the total elapsed time that each
	 * system type (PHYSICS, GRAPHICS, AUDIO, MISC) occupies. It uses the
	 * previously recorded delta times to calculate the percentages, which can
	 * be useful for performance analysis and optimization.
	 */
	static void CalculatePercentageTime();
};

#endif // !_PROCESS_TIME_H