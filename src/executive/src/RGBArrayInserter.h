#pragma once

#include "RGBArray.h"
#include <cstdint>
#include <iterator>

class RGBArrayInserter final {
public:
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using _Container = RGBArray;

    RGBArrayInserter(RGBArray &arr, RGB inserted, uint64_t xSize)
        : arr_(arr), pixel_(inserted), xSize_(xSize)
    {}
    constexpr inline RGBArrayInserter &operator++() { return *this; }
    constexpr inline RGBArrayInserter &operator++(int) { return *this; }
    constexpr inline RGBArrayInserter &operator*() { return *this; }

    inline RGBArrayInserter &operator=(std::pair<uint32_t, uint32_t> xy)
    {
        arr_[xy.second * xSize_ + xy.first] = pixel_;
        return *this;
    }

private:
    _Container &arr_;
    RGB pixel_;
    uint64_t xSize_;
};
