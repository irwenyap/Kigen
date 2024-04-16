#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class Timer {
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double>;

    TimePoint begin, previous, current;
    double elapsedTime = 0.0;
    double deltaTime = 0.0;

public:
    Timer() = default;

    void Start();
    void Update();

    double GetDeltaTime() const { return deltaTime; }
    double GetElapsedTime() const { return elapsedTime; }

private:
    static double DurationInSeconds(const Duration& duration) {
        return duration.count();
    }
};

#endif // !TIMER_HPP
