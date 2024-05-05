#include "total_stat_manager.h"
#include "iostream"
#include "fstream"

namespace {
    int PRINTING_STATS_INTERVAL_SEC = 5;
}

namespace http {
    total_stat_manager::total_stat_manager(const std::shared_ptr<timer::i_timer_factory>& timer_factory)
    {
        auto print_stats = [this]()
        {
            std::cout << "total_reads = " << total_reads.load() << ", total_writes = " << total_writes.load() << std::endl;
        };

        m_timer = timer_factory->create_interval_timer(std::chrono::seconds(PRINTING_STATS_INTERVAL_SEC), print_stats);
        m_timer->start();
    }

    void total_stat_manager::increase_reads(const std::string& key)
    {
        ++total_reads;
        const std::scoped_lock lock(m_mtx);
        ++m_stats[key].first;
    }

    void total_stat_manager::increase_writes(const std::string& key)
    {
        ++total_writes;
        const std::scoped_lock lock(m_mtx);
        ++m_stats[key].second;
    }

    std::optional<i_stat_manager::statistics> total_stat_manager::get_statistics(const std::string& key) const
    {
        const std::scoped_lock lock(m_mtx);
        const auto it = m_stats.find(key);
        if (it != m_stats.end())
        {
            return i_stat_manager::statistics{.total_reads = it->second.first, .total_writes = it->second.second};
        }
        return std::nullopt;
    }

} // namespace db