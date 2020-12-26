#include <boost/asio/thread_pool.hpp>
#include <boost/log/trivial.hpp>
#include <rocksdb/c.h>
#include <atomic>
#include <condition_variable>
#include <mutex>

class CheckSummer {
};
