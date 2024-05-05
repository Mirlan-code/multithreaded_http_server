#pragma once

#include <string>
#include <vector>
#include <optional>

namespace http {

    class i_stat_manager {
    public:
        struct statistics {
            uint32_t total_reads = 0;
            uint32_t total_writes = 0;
        };

        virtual void increase_reads(const std::string& key) = 0;
        virtual void increase_writes(const std::string& key) = 0;

        virtual std::optional<statistics> get_statistics(const std::string& key) const = 0;

        virtual ~i_stat_manager() = default;
    };
} // namespace http