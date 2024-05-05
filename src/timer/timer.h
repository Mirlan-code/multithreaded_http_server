#pragma once

#include "i_timer.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace timer {
    class interval_timer : public i_timer
    {
    public:
        interval_timer(std::chrono::seconds interval, std::function<void()> action);
        ~interval_timer();

        void start() override;
        void stop() override;

    private:
        void thread_func();

        const std::chrono::seconds m_interval;
        const std::function<void()> m_action;
        bool m_stop;
        std::thread m_timer_thread;
        std::mutex m_mtx;
        std::condition_variable m_cond_var;
    };

    struct timer_factory : public i_timer_factory
    {
    public:
        std::unique_ptr<i_timer> create_interval_timer(std::chrono::seconds interval, std::function<void()> action) override;
    };
} // namespace timer
