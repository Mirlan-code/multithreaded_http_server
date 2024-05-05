#pragma once

#include "i_stat_manager.h"
#include "timer/i_timer.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>

namespace http {

    class total_stat_manager : public i_stat_manager {
    public:
        total_stat_manager(const std::shared_ptr<timer::i_timer_factory>& timer_factory);
        ~total_stat_manager() {};

        void increase_reads(const std::string& key);
        void increase_writes(const std::string& key);

        std::optional<statistics> get_statistics(const std::string& key) const override;

    private:
        mutable std::mutex m_mtx;
        std::unique_ptr<timer::i_timer> m_timer;
        std::unordered_map<std::string, std::pair<int, int>> m_stats;
        std::atomic<int> total_reads = 0;
        std::atomic<int> total_writes = 0;
    };
} // namespace http