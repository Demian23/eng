#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/alg.h"
#include <doctest/doctest.h>
#include <iostream>

using namespace eng::alg;

struct valueType : public std::pair<long, long> {
    std::string stringRep;
    valueType(long first, long second) : std::pair<long, long>(first, second)
    {
        stringRep =
            std::to_string(this->first) + "," + std::to_string(this->second);
    }
    operator const char *() const { return stringRep.c_str(); }
};
TEST_CASE("Line points generating")
{
    std::vector<valueType> points;
    std::vector<valueType> expected{{0, 3}, {1, 2}, {2, 2}, {3, 1}, {4, 0}};
    line(0, 4, 3, 0, std::back_inserter(points));
    CHECK_EQ(expected, points);
}

TEST_CASE("Line points output in ostream")
{
    std::string expected = "0,3 1,2 2,2 3,1 4,0 ";
    std::stringstream res;
    line(0, 4, 3, 0, std::ostream_iterator<valueType>(res, " "));
    REQUIRE_EQ(expected, res.str());
}
