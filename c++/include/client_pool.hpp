#include <iostream>
#include <memory>
#include <string>
#include <mutex>
#include <future>
#include <unordered_map>
#include <boost/compute/detail/lru_cache.hpp>

const int CLIENT_CACHE_SIZE = 20;

template<typename T>
class Connect {
public:
    virtual std::future<T> connect(const std::string& endpoint) = 0;
    virtual ~Connect() = default;
};

template<typename T>
class ClientPool {
private:
    mutable std::mutex mutex_;
    boost::compute::detail::lru_cache<std::string, T> client_cache_;

public:
    ClientPool() : client_cache_(CLIENT_CACHE_SIZE) {}

    std::future<T> connect(const std::string& address) {
        return std::async(std::launch::async, [this, address]() {
            auto client = T::connect(address).get();
            std::lock_guard<std::mutex> lock(mutex_);
            client_cache_.insert(address, client);
            return client;
        });
    }

    std::future<T> get_client(const std::string& address) {
        return std::async(std::launch::async, [this, address]() {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                auto it = client_cache_.find(address);
                if (it != client_cache_.end()) {
                    return it->second;
                }
            }
            return connect(address).get();
        });
    }
};