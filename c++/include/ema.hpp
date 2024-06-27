#include <boost/optional.hpp>

class EMA {
private:
    double b_;
    double n_;
    double avg_;

public:
    EMA() : b_(0.9), n_(0), avg_(0) {}

    double next(double v) {
        n_ += 1;
        avg_ = b_ * v + (1 - b_) * avg_;
        return avg_;
    }

    double avg() const {
        return avg_;
    }
};

boost::optional<EMA> create_ema() {
    return boost::optional<EMA>(EMA());
}