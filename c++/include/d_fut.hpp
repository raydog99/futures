#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include "dstore_id.hpp"

template <typename T>
class DFut {
private:
    DStoreId d_store_id;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(d_store_id);
    }

public:
    DFut() = default;
    explicit DFut(const DStoreId& id) : d_store_id(id) {}

    DFut share() const {
        return DFut(d_store_id.clone());
    }

    static DFut from_d_store_id(const DStoreId& id) {
        return DFut(id);
    }
};