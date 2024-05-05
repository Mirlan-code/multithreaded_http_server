//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_handler.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "mime_types.h"
#include "reply.h"
#include "request.h"
#include "rapidjson/document.h"

namespace {
    std::string parseKeyFromPath(const std::string& path) {
        std::string keyPrefix = "key=";
        size_t pos = path.find(keyPrefix);
        if (pos != std::string::npos) {
            pos += keyPrefix.length(); // Move to the start of the key
            size_t endPos = path.find('&', pos); // Find the end of the key
            if (endPos != std::string::npos) {
                return path.substr(pos, endPos - pos); // Extract the key
            } else {
                return path.substr(pos); // Key is at the end of the path
            }
        }
        return ""; // Key not found
    }
} // namespace

namespace http {
    request_handler::request_handler(const std::shared_ptr<i_stat_manager>& stat_manager,
                                     const std::shared_ptr<db::i_db_manager>& db_manager)
        : m_stat_manager(stat_manager)
        , m_db_manager(db_manager)
    {
        assert(m_stat_manager != nullptr);
        assert(m_db_manager != nullptr);
        m_handler_map["/get_key"] = [this](const request& req, reply& rep) { get_key(req, rep); };
        m_handler_map["/set_key"] = [this](const request& req, reply& rep) { set_key(req, rep); };
    }

    void request_handler::handle_request(const request& req, reply& rep)
    {
        // Decode url to path.
        std::string request_path;
        if (!url_decode(req.uri, request_path))
        {
            rep = reply::stock_reply(reply::bad_request);
            return;
        }

        for (const auto& [prefix, func] : m_handler_map)
        {
            if (request_path.starts_with(prefix)) {
                func(req, rep);
                return;
            }
        }

        rep = reply::stock_reply(reply::bad_request);
        return;
    }

    void request_handler::get_key(const request& req, reply& rep)
    {
        std::string request_path;
        if (!url_decode(req.uri, request_path))
        {
            rep = reply::stock_reply(reply::bad_request);
            return;
        }
        rep.status = reply::ok;
        const auto key = parseKeyFromPath(request_path);
        m_stat_manager->increase_reads(key);
        if (const auto valueOpt = m_db_manager->get(key); valueOpt.has_value())
        {
            auto stats = m_stat_manager->get_statistics(key);
            assert(stats.has_value());
            rep.content = key + "=" + *valueOpt + "\nreads=" + std::to_string(stats->total_reads) + "\nwrites=" + std::to_string(stats->total_writes) + "";
        } else {
            rep.content = "None";
        }
        rep.headers.resize(2);
        rep.headers[0].name = "Content-Length";
        rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
        rep.headers[1].name = "Content-Type";
        rep.headers[1].value = mime_types::extension_to_type("json");
    }

    void request_handler::set_key(const request& req, reply& rep)
    {
        rapidjson::Document doc;
        try
        {
            doc.Parse(req.body.c_str());
            if (!doc.IsObject()) {
                std::cerr << "Can not parse body" << std::endl;
                rep = reply::stock_reply(reply::bad_request);
                return;
            }
            const auto beginItr = doc.MemberBegin();
            std::string key = beginItr->name.GetString();
            const rapidjson::Value& value = beginItr->value;

            if (value.IsString()) {
                rep.status = reply::ok;
                std::string valueStr = value.GetString();
                rep.content = "set value=" + valueStr + " for key=" + key + "\n";
                rep.headers.resize(2);
                rep.headers[0].name = "Content-Length";
                rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
                rep.headers[1].name = "Content-Type";
                rep.headers[1].value = mime_types::extension_to_type("json");

                m_stat_manager->increase_writes(key);
                m_db_manager->set(std::move(key), std::move(valueStr));
            } else {
                std::cerr << "Unsupported type" << std::endl;
                rep = reply::stock_reply(reply::bad_request);
            }
        }
        catch (...)
        {
            rep = reply::stock_reply(reply::bad_request);
            return;
        }
    }

    bool request_handler::url_decode(const std::string& in, std::string& out)
    {
        out.clear();
        out.reserve(in.size());
        for (std::size_t i = 0; i < in.size(); ++i)
        {
            if (in[i] == '%')
            {
                if (i + 3 <= in.size())
                {
                    int value = 0;
                    std::istringstream is(in.substr(i + 1, 2));
                    if (is >> std::hex >> value)
                    {
                        out += static_cast<char>(value);
                        i += 2;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            else if (in[i] == '+')
            {
                out += ' ';
            }
            else
            {
                out += in[i];
            }
        }
        return true;
    }

} // namespace http
