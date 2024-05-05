#pragma once

#include <memory>
#include <functional>
#include <chrono>

namespace timer {

    class i_timer {
    public:
        virtual void start() = 0;
        virtual void stop() = 0;

        virtual ~i_timer() = default;
    };

    struct i_timer_factory {
        virtual std::unique_ptr<i_timer> create_interval_timer(std::chrono::seconds interval, std::function<void()> action) = 0;
        virtual ~i_timer_factory() = default;
    };
} // namespace timer