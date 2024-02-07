#pragma once

#include <cstdint>
#include <vector>
#include <iostream>

struct RGB {
    uint8_t r, g, b;
};

class RGBArray final{
public:
    RGBArray(uint64_t size, RGB initValue) : rgbArrayRep(size, initValue){}
    inline void fill(RGB value){std::fill(rgbArrayRep.begin(), rgbArrayRep.end(), value);}
    [[nodiscard]]
    inline uint8_t* data(){return reinterpret_cast<uint8_t*>(rgbArrayRep.data());}
    [[nodiscard]]
    inline const uint8_t* data()const{return reinterpret_cast<const uint8_t*>(rgbArrayRep.data());}
    inline RGB & operator[](uint64_t index){
        if(index < rgbArrayRep.size())
            return rgbArrayRep[index];
        else
            std::cerr << "Out of bounds, index: " << index << '\n';
        return rgbArrayRep[0];
    }
private:
    std::vector<RGB> rgbArrayRep;
};
