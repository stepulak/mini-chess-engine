#pragma once

#include <chrono>
#include <functional>
#include <thread>

class Timer {
public:
    Timer(size_t sleepTimeMilliseconds, const std::function<void()>& fn)
        : _thread([&] {
            using namespace std::chrono;
            const auto start = system_clock::now();
            while (sleepTimeMilliseconds >= duration_cast<milliseconds>(system_clock::now() - start).count()) {
                std::this_thread::sleep_for(milliseconds(1u));
            }
            fn();
        })
    {
    }

private:
    std::thread _thread;
};