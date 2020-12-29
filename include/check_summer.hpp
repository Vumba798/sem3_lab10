// Copyright 2020 Vumba798 <alexandrov32649@gmail.com>
#ifndef INCLUDE_CHECK_SUMMER_HPP_
#define INCLUDE_CHECK_SUMMER_HPP_
#include <boost/asio/thread_pool.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>
#include <rocksdb/c.h>
#include <condition_variable>
#include <unordered_map>
#include <string>
#include <vector>

struct KeyValue {
    std::string key;
    std::string value;
    inline KeyValue(const std::string& k, const std::string& v) :
        key(k), value(v) {}
};

class CheckSummer {
private:
    boost::asio::thread_pool _pool;
    std::vector<std::unordered_map<std::string, std::string>> _data;
    std::vector<std::string> _columnNames;
    std::string _input;
    std::string _output;
    std::mutex _mutex;

    auto _calculate_hash(const uint32_t i,
            const KeyValue keyValue) -> void;
    auto _read_db() -> void;
    auto _write_db() -> void;
public:
    CheckSummer(const uint32_t& amountOfThreads, const std::string& input,
            const std::string& output);
    auto write_test_db() -> void;
    auto start() -> void;
};

#endif // INCLUDE_CHECK_SUMMER_HPP_
