#pragma once

#include "RGBArray.h"
#include <cstdint>

class RGBArrayInserter final {
public:
    using valueType = std::pair<uint32_t, uint32_t>;
    RGBArrayInserter(RGBArray &arr, RGB inserted, uint64_t xSize)
        : arr_(arr), pixel_(inserted), xSize_(xSize)
    {}
    constexpr inline RGBArrayInserter &operator++() { return *this; }
    constexpr inline RGBArrayInserter &operator++(int) { return *this; }
    constexpr inline RGBArrayInserter &operator*() { return *this; }
    inline RGBArrayInserter &operator=(valueType xy)
    {
        arr_[xy.second * xSize_ + xy.first] = pixel_;
        return *this;
    }

private:
    RGBArray &arr_;
    RGB pixel_;
    uint64_t xSize_;
};
