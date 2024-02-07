#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include "../src/alg.h"

using namespace eng::alg;

struct valueType : public std::pair<long, long>{
    std::string stringRep;
    valueType(long first, long second) : std::pair<long, long>(first, second)
        {stringRep = std::to_string(this->first) + "," + std::to_string(this->second);}
    operator const char*()const{return stringRep.c_str();}
};
TEST_CASE("Line points generating"){
    std::vector<valueType> points;
    std::vector<valueType> expected{{0, 3}, {1, 3}, {2, 2}, {3,1}, {4, 0}};
    line<valueType>(0, 4, 3, 0, std::back_inserter(points));
    CHECK_EQ(expected, points);
}

TEST_CASE("Line points output in ostream"){
    std::vector<valueType> points;
    std::string expected = "0,3 1,3 2,2 3,1 4,0 ";
    std::stringstream res;
    line<valueType>(0, 4, 3, 0, std::ostream_iterator<valueType>(res, " "));
    REQUIRE_EQ(expected, res.str());
}
