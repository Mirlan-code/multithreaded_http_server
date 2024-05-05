#pragma once

#include "i_db_manager.h"
#include "timer/i_timer.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace db {

    class file_db_manager : public i_db_manager {
    public:
        file_db_manager(
                std::string file,
                const std::shared_ptr<timer::i_timer_factory>& timer_factory);

        ~file_db_manager();

        void set(std::string&& key, std::string&& value) override;
        std::optional<std::string> get(const std::string& key) const override;

    private:
        const std::string m_file;
        std::unordered_map<std::string, std::string> m_state;
        mutable std::mutex m_mtx;
        std::unique_ptr<timer::i_timer> m_timer;
        std::function<void()> m_do_save;
    };
} // namespace http