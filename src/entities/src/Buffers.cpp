#include "Buffers.h"

namespace eng::ent{

void PixelArray::fill(pixel value)
{
    std::fill(pixels.begin(), pixels.end(), value);
}

[[nodiscard]] uint8_t *PixelArray::data() { return &pixels[0][0]; }

[[nodiscard]] const uint8_t *PixelArray::data() const noexcept{ return &pixels[0][0]; }

bool PixelArray::trySetPixel(uint64_t index, pixel value)
{
    if (index < pixels.size()) [[likely]] {
        pixels[index] = value;
        return true;
    }
    return false;
}

PixelArray::pixel &PixelArray::operator[](uint64_t index) { return pixels[index]; }

void IndexBuffer::operator()(uint32_t x, uint32_t y, floating u, floating v, floating w,
                             Triangle triangle) noexcept
{
    uint64_t index = y * _xSize + x;
    if (index < _gBuffer.size()) [[likely]] {
        _gBuffer[index] = Fragment{u, v, w, triangle};
    }
}

void IndexBuffer::clean(){
    std::fill(_gBuffer.begin(), _gBuffer.end(), Fragment{});
}

}

