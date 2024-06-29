#pragma once
#include <memory>

class DStoreId {
private:
    std::shared_ptr<uint64_t> id;

public:
    explicit DStoreId(uint64_t value) : id(std::make_shared<uint64_t>(value)) {}

    DStoreId clone() const {
        return DStoreId(*id);
    }
};