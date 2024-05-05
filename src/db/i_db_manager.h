#pragma once

#include <string>
#include <vector>
#include <optional>

namespace db {

    class i_db_manager {
    public:
        virtual void set(std::string&& key, std::string&& value) = 0;
        virtual std::optional<std::string> get(const std::string& key) const = 0;

        virtual ~i_db_manager() = default;
    };
} // namespace db