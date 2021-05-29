#pragma once

#include <chrono>
#include <functional>
#include <mutex>
#include <thread>
#include <atomic>


class Timer final {
public:
    Timer(size_t sleepMilliseconds, const std::function<void()>& callback)
        : _stop(false)
        , _thread([&] {
            using namespace std::chrono;
            const auto start = system_clock::now();

            while (true) {
                const auto duration = duration_cast<milliseconds>(system_clock::now() - start).count();

                if (duration >= sleepMilliseconds) {
                    callback();
                    break;
                }
                std::this_thread::sleep_for(milliseconds(1));

                // Manual stop, callback is not called
                if (_stop) {
                    break;
                }
            }
        })
    {
    }

    void stop()
    {
        _stop = true;
    }

    void join()
    {
        _thread.join();
    }

private:
    std::atomic_bool _stop;
    std::thread _thread;
};