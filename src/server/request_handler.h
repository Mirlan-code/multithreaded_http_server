//
// request_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once
#include "i_stat_manager.h"
#include "db/i_db_manager.h"
#include <string>
#include <memory>
#include <functional>
#include <boost/noncopyable.hpp>

namespace http {
    struct reply;
    struct request;

/// The common handler for all incoming requests.
    class request_handler
            : private boost::noncopyable
    {
    public:
        /// Construct with a directory containing files to be served.
        explicit request_handler(const std::shared_ptr<i_stat_manager>& stat_manager,
                                 const std::shared_ptr<db::i_db_manager>& db_manager);

        /// Handle a request and produce a reply.
        void handle_request(const request& req, reply& rep);

    private:
        void get_key(const request& req, reply& rep);
        void set_key(const request& req, reply& rep);
        /// Perform URL-decoding on a string. Returns false if the encoding was invalid.
        static bool url_decode(const std::string& in, std::string& out);

    private:
        const std::shared_ptr<i_stat_manager> m_stat_manager;
        const std::shared_ptr<db::i_db_manager> m_db_manager;

        std::unordered_map<std::string, std::function<void(const request&, reply&)>> m_handler_map;
    };
} // namespace http