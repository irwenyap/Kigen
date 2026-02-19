/*********************************************************************
 * \file	ProcessTime.cpp
 * \brief	Defines a ProcessTime class that process all time related functionality like fps and process time info.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#include "ProcessTime.hpp"
#include "../Input/InputManager.hpp"
#include <iostream>
//#include "Logger.hpp"


double ProcessTime::fps{};
Timer ProcessTime::deltaTime{};
ProcessTime::TimePoint ProcessTime::startTime{ProcessTime::Clock::now()};
std::map<ProcessTime::SystemType, double> ProcessTime::deltaMap{};
std::map<ProcessTime::SystemType, double> ProcessTime::percentMap{};

void ProcessTime::UpdateTime(double fps_calc_interval) {
    // get elapsed time (in seconds) between previous and current frames
    TimePoint ct = Clock::now();

    // fps calculations
    static double count = 0.0; // number of game loop iterations
    // get elapsed time since very beginning (in seconds) ...
    Duration et = ct - startTime;
    double elapsed_time = et.count();

    ++count;

    // update fps at least every 10 seconds ...
    fps_calc_interval = (fps_calc_interval < 0.0) ? 0.0 : fps_calc_interval;
    fps_calc_interval = (fps_calc_interval > 10.0) ? 10.0 : fps_calc_interval;
    if (elapsed_time > fps_calc_interval) {
        ProcessTime::fps = count / elapsed_time;
        startTime = ct;
        count = 0.0;
    }
}

void ProcessTime::CreateDelta() {

    deltaMap[MISC] = 0;
    deltaMap[PHYSICS] = 0;
    deltaMap[GRAPHICS] = 0;
    deltaMap[AUDIO] = 0;
}

void ProcessTime::StartDelta() {
    //time.Start();
    start = std::chrono::high_resolution_clock::now();
}

void ProcessTime::EndDelta(SystemType type) {
    auto it = deltaMap.find(type);
    if (it == deltaMap.end()) return;

    //time.Update();
    end = std::chrono::high_resolution_clock::now();

    it->second = std::chrono::duration<double>{ end - start}.count();
}

void ProcessTime::CalculatePercentageTime() {

    double dt = std::chrono::duration<double>{ deltaTime.GetCurrent() - deltaTime.GetBegin() }.count();
    
    percentMap[PHYSICS] = deltaMap.find(PHYSICS)->second / dt * 100.0;
    percentMap[GRAPHICS] = deltaMap.find(GRAPHICS)->second / dt * 100.0;
    percentMap[AUDIO] = deltaMap.find(AUDIO)->second / dt * 100.0;

    deltaMap[MISC] = dt - deltaMap.find(PHYSICS)->second - deltaMap.find(GRAPHICS)->second - deltaMap.find(AUDIO)->second;
    percentMap[MISC] = deltaMap.find(MISC)->second / dt * 100.0;
}