#include <boost/asio/thread_pool.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>
#include <rocksdb/c.h>
#include <atomic>
#include <condition_variable>
#include <unordered_map>

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

    auto _write_db() -> void;
public:
    CheckSummer(const uint32_t& amountOfThreads, const std::string& input,
            const std::string& output);
    auto write_test_db() -> void;
    auto read_db() -> void;
    auto start() -> void;
    auto calculate_hash(const uint32_t i,
            const std::string key,
            const std::string value) -> void;
};
