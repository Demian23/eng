#include "RGBArray.h"
#include <cstdint>
#include <iterator>
#include "../../base/src/eng.h"

class RGBArrayInterpolator final {
public:
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using _Container = RGBArray;

    RGBArrayInterpolator(RGBArray &arr, RGB a, RGB b, RGB c, uint32_t xSize)
        : arr_(arr), _a(a), _b(b), _c(c), xSize_(xSize)
    {}
    constexpr inline RGBArrayInterpolator &operator++() { return *this; }
    constexpr inline RGBArrayInterpolator &operator++(int) { return *this; }
    constexpr inline RGBArrayInterpolator &operator*() { return *this; }

    inline RGBArrayInterpolator &operator=(std::tuple<uint32_t, uint32_t, eng::floating, eng::floating, eng::floating> xyuvw)
    {
        auto [x, y, u, v, w] = xyuvw;
        RGB color{};
        color.r = static_cast<uint8_t>(u * static_cast<eng::floating>(_a.r) + v * static_cast<eng::floating>(_b.r) + w * static_cast<eng::floating>(_c.r));
        color.b = static_cast<uint8_t>(u * static_cast<eng::floating>(_a.b) + v * static_cast<eng::floating>(_b.b) + w * static_cast<eng::floating>(_c.b));
        color.g = static_cast<uint8_t>(u * static_cast<eng::floating>(_a.g) + v * static_cast<eng::floating>(_b.g) + w * static_cast<eng::floating>(_c.g));
        arr_[y * xSize_ + x] = color;
        return *this;
    }

private:
    _Container &arr_;
    RGB _a, _b, _c;
    uint32_t xSize_;
};
