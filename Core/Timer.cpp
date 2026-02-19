/*********************************************************************
 * \file		Timer.cpp
 * \brief		Handles delta time as well as elapsed time
 *
 * \author		Yap Zi Yang Irwen, y.ziyangirwen, 2301345
 * \email       y.ziyangirwen@digipen.edu
 * \author		Wong Woon Li, woonli.wong, 2301308
 * \email       woonli.wong@digipen.edu
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its contents without the
 *				prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#include "Timer.hpp"
#include <iostream>

int Timer::fps = 0;

void Timer::Start() {
    begin = previous = Clock::now();
}

//void Timer::Update() {
//    current = Clock::now();
//    Duration elapsed = current - begin;
//    Duration delta = current - previous;
//
//    previous = current;
//
//    elapsedTime = DurationInSeconds(elapsed);
//    deltaTime = DurationInSeconds(delta);
//
//    static int frame_counter = 0;
//    ++frame_counter;
//    if (elapsedTime >= 1.0) {
//		fps = frame_counter;
//        frame_counter = 0;
//		begin = current;
//	}
//
//    CalculateNumOfSteps(deltaTime);
//}

static int frame_counter = 0;
static double fpsTimer = 0.0;  // Add this to accumulate time
void Timer::Update() {
    current = Clock::now();
    Duration elapsed = current - begin;
    Duration delta = current - previous;

    previous = current;

    elapsedTime = DurationInSeconds(elapsed);
    deltaTime = DurationInSeconds(delta);

    ++frame_counter;
    fpsTimer += deltaTime;         // Accumulate delta time

    if (fpsTimer >= 1.0) {         // Update FPS every second
        fps = frame_counter;
        frame_counter = 0;
        fpsTimer = 0.0;           // Subtract 1.0 to avoid time drift
    }

    CalculateNumOfSteps(deltaTime);
}

void Timer::CalculateNumOfSteps(double dt) {
    // Calculate the number of steps to update time-based systems based on fixedDeltaTime.
    numOfSteps = 0;
    dtAccumulator += dt;
    while (dtAccumulator >= fixedDeltaTime) {
        dtAccumulator -= fixedDeltaTime;
        ++numOfSteps;
    }
}
