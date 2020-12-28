#include <boost/asio/thread_pool.hpp>
#include <boost/log/trivial.hpp>
#include <rocksdb/c.h>
#include <atomic>
#include <condition_variable>
#include <mutex>

class CheckSummer {
private:
    std::mutex _mutex;
    boost::asio::thread_pool _pool;
    std::string _input;
    std::string _output;

public:
    CheckSummer(const uint32_t& amountOfThreads, const std::string& input,
            const std::string& output);
    auto write_test_db() -> void;
    auto read_db() -> void;
};
