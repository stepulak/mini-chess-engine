#pragma once

#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

#include <iostream>

class Timer final {
public:
    Timer(size_t sleepMilliseconds, const std::function<void()>& fn)
        : _thread([&] {
            using namespace std::chrono;
            const auto start = system_clock::now();
            while (true) {
                const auto duration = duration_cast<milliseconds>(system_clock::now() - start).count();
                _mutex.lock();
                if (_stop) {
                    std::cout << "HERE!" << std::endl;
                    break;
                }
                _mutex.unlock();
                if (sleepMilliseconds <= duration) {
                    fn();
                    break;
                }
                std::this_thread::sleep_for(milliseconds(1));
            }
            std::cout << "STOPPING!" << std::endl;
        })
    {
    }

    void stop()
    {
        _mutex.lock();
        _stop = true;
        _mutex.unlock();
        _thread.join();
    }

private:
    std::mutex _mutex;
    std::thread _thread;
    bool _stop = false;
};