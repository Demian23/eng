#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

struct RGB {
    uint8_t r, g, b;
};

class RGBArray final {
public:
    explicit RGBArray(std::size_t size) : rgbArrayRep{size} {}

    inline void fill(RGB value)
    {
        std::fill(rgbArrayRep.begin(), rgbArrayRep.end(), value);
    }

    [[nodiscard]] inline uint8_t *data()
    {
        return reinterpret_cast<uint8_t *>(rgbArrayRep.data());
    }

    [[nodiscard]] inline const uint8_t *data() const
    {
        return reinterpret_cast<const uint8_t *>(rgbArrayRep.data());
    }

    inline RGB &operator[](uint64_t index)
    {
        if (index < rgbArrayRep.size())
            return rgbArrayRep[index];
        return rgbArrayRep[0];
    }

    inline auto begin() { return rgbArrayRep.begin(); }
    inline auto end() { return rgbArrayRep.end(); }

    inline size_t size() const noexcept { return rgbArrayRep.size(); }

private:
    std::vector<RGB> rgbArrayRep;
};
