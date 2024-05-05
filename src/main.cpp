//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "server/server.h"
#include "server/total_stat_manager.h"
#include "db/file_db_manager.h"
#include "timer/timer.h"

int main(int argc, char* argv[])
{
    try
    {
        // Initialise the server.
        std::size_t num_threads = boost::lexical_cast<std::size_t>(10);
        std::shared_ptr<timer::i_timer_factory> timer_factory = std::make_shared<timer::timer_factory>();
        std::shared_ptr<http::i_stat_manager> stat_manager = std::make_shared<http::total_stat_manager>(timer_factory);
        std::shared_ptr<db::i_db_manager> db_manager = std::make_shared<db::file_db_manager>("file_storage.txt", timer_factory);
        http::server s("0.0.0.0", "12345", num_threads, stat_manager, db_manager);

        // Run the server until stopped.
        s.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    return 0;
}
