#include "Timer.hpp"

void Timer::Start() {
    begin = previous = Clock::now();
}

void Timer::Update() {
    current = Clock::now();
    Duration elapsed = current - begin;
    Duration delta = current - previous;

    previous = current;

    elapsedTime = DurationInSeconds(elapsed);
    deltaTime = DurationInSeconds(delta);
}