#pragma once

#include "../../base/src/Elements.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace eng::ent {
class PixelArray final {
public:
    using pixel = ScreenPixel;
    explicit PixelArray(std::size_t size) : pixels{size} {}

    inline void fill(pixel value)
    {
        std::fill(pixels.begin(), pixels.end(), value);
    }

    [[nodiscard]] inline uint8_t *data() { return &pixels[0][0]; }

    [[nodiscard]] inline const uint8_t *data() const { return &pixels[0][0]; }

    inline bool trySetPixel(uint64_t index, pixel value)
    {
        if (index < pixels.size()) [[likely]] {
            pixels[index] = value;
            return true;
        }
        return false;
    }

    inline pixel &operator[](uint64_t index) { return pixels[index]; }

    inline auto begin() { return pixels.begin(); }
    inline auto end() { return pixels.end(); }

    inline size_t size() const noexcept { return pixels.size(); }

private:
    std::vector<pixel> pixels;
};
} // namespace eng::ent
