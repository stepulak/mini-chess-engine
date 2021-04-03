#pragma once

#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

class Timer final {
public:
    Timer(size_t sleepMilliseconds, const std::function<void()>& fn)
        : _thread([&] {
            using namespace std::chrono;
            const auto start = system_clock::now();
            while (true) {
                const auto duration = duration_cast<milliseconds>(system_clock::now() - start).count();
                if (sleepMilliseconds <= duration) {
                    fn();
                    break;
                }
                std::this_thread::sleep_for(milliseconds(1));
                std::scoped_lock lock(_mutex);
                if (_stop) {
                    break;
                }
            }
        })
    {
    }

    void stop()
    {
        std::scoped_lock lock(_mutex);
        _stop = true;
    }

    void join()
    {
        _thread.join();
    }

private:
    std::mutex _mutex;
    std::thread _thread;
    bool _stop = false;
};