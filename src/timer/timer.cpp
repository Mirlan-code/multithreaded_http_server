#include "timer.h"

#include <algorithm>
#include <utility>

namespace timer
{
    interval_timer::interval_timer(std::chrono::seconds interval, std::function<void()> action)
        : m_interval(interval)
        , m_action(std::move(action))
        , m_stop(true)
    {
    }

    interval_timer::~interval_timer()
    {
        stop();
    }

    void interval_timer::start()
    {
        {
            const std::scoped_lock lock(m_mtx);
            m_stop = false;
        }

        m_timer_thread = std::thread([this] {
            this->thread_func();
        });
    }

    void interval_timer::thread_func()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            m_cond_var.wait_for(lock, m_interval, [this] { return m_stop == true; });
            if (m_stop) {
                return;
            }
            m_action();
        }
    }

    void interval_timer::stop()
    {
        {
            const std::scoped_lock lock(m_mtx);
            m_stop = true;
            m_cond_var.notify_one();
            if (m_timer_thread.joinable()) {
                m_timer_thread.join();
            }
        }
    }

    std::unique_ptr<i_timer> timer_factory::create_interval_timer(std::chrono::seconds interval,
                                                                  std::function<void()> action)
    {
        return std::make_unique<interval_timer>(interval, action);
    }
} // namespace timer
