#include "file_db_manager.h"
#include "iostream"
#include "fstream"

namespace {
    int SAVING_TO_FILE_INTERVAL_SEC = 30;
    char DELIMETER = '\n';
}

namespace db {
    file_db_manager::file_db_manager(
            std::string file,
            const std::shared_ptr<timer::i_timer_factory>& timer_factory)
        : m_file(file)
    {

        {
            // restoring data from file
            const std::scoped_lock lock(m_mtx);
            std::ifstream file(m_file, std::ios::in);
            if (file.is_open()) {
                std::string key;
                std::string value;
                while (getline(file, key, DELIMETER))
                {
                    getline(file, value, DELIMETER);
                    m_state[key] = value;
                }
            }
        }

        m_do_save = [this]()
        {
            const std::scoped_lock lock(m_mtx);

            std::ofstream file(m_file, std::ios::out);
            if (!file.is_open())
            {
                std::cerr << "Error opening file: " << m_file << std::endl;
                return;
            }

            for (const auto& [key, value] : m_state)
            {
                file << key << DELIMETER << value << DELIMETER;
            }
            file.close(); // Close the file
        };

        m_timer = timer_factory->create_interval_timer(std::chrono::seconds(SAVING_TO_FILE_INTERVAL_SEC), m_do_save);
        m_timer->start();
    }

    file_db_manager::~file_db_manager() {
        m_do_save();
    }

    void file_db_manager::set(std::string&& key, std::string&& value) {
        const std::scoped_lock(m_mtx);
        m_state[std::move(key)] = std::move(value);
    }

    std::optional<std::string> file_db_manager::get(const std::string& key) const {
        const std::scoped_lock(m_mtx);
        const auto it = m_state.find(key);
        if (it != m_state.end()) {
            return it->second;
        }

        return std::nullopt;
    }

} // namespace db