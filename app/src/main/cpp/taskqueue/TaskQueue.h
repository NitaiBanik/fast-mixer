//
// Created by asalehin on 7/18/20.
//

#ifndef FAST_MIXER_TASKQUEUE_H
#define FAST_MIXER_TASKQUEUE_H

#ifndef MODULE_NAME
#define MODULE_NAME "ThreadPool"
#endif

#include <queue>
#include <atomic>
#include <thread>
#include <functional>
#include <chrono>

using namespace std;

class TaskQueue {

public:
    void start_queue() {
        is_running.exchange(true);
        t = std::move(std::thread([=]() { this->executor_loop(); }));
    }

    void stop_queue() {
        is_running.exchange(false);
    }

    void enqueue(std::function<void()> f) {
        q.push(f);
    }
    thread t;

private:
    const char* TAG = "TaskQueue:: %d";
    std::queue<std::function<void()>> q;
    atomic<bool> is_running;

    void executor_loop() {
        while (is_running) {
            if (!q.empty()) {
                auto f = q.front();
                if (f != nullptr) {
                    f();
                }
                q.pop();
            }
            std::this_thread::sleep_for(std::chrono::microseconds (200));
        }
    }
};

#endif //FAST_MIXER_TASKQUEUE_H