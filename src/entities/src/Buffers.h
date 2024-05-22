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

    void fill(pixel value);

    [[nodiscard]] uint8_t *data();

    [[nodiscard]] const uint8_t *data() const noexcept;

    bool trySetPixel(uint64_t index, pixel value);

    pixel &operator[](uint64_t index);

    [[nodiscard]] inline auto begin() { return pixels.begin(); }
    [[nodiscard]] inline auto end() { return pixels.end(); }

    inline size_t size() const noexcept { return pixels.size(); }

private:
    std::vector<pixel> pixels;
};
struct Fragment {
    floating u{}, v{}, w{};
    Triangle triangle{};
};


// it can be used as z-buffer, but effectiveness should be tested
class IndexBuffer{
public:
    IndexBuffer(std::size_t gBufferSize, uint32_t xSize)
        : _gBuffer(gBufferSize),_xSize(xSize)
    {}
    IndexBuffer(const IndexBuffer &) = delete;
    IndexBuffer&operator=(const IndexBuffer&) = delete;
    [[nodiscard]] inline auto cBegin()const noexcept{return _gBuffer.cbegin();}
    [[nodiscard]] inline auto cEnd()const noexcept{return _gBuffer.cend();}
    void clean();

    void operator()(uint32_t x, uint32_t y, floating u, floating v, floating w,
                    Triangle triangle) noexcept;

private:
    std::vector<Fragment> _gBuffer;
    uint32_t _xSize;
};

} // namespace eng::ent
