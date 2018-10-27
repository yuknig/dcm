#include <gtest/gtest.h>
#include <Util/CastValue.h>

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastIntToInt)
{
    int from = 100;
    int to = 0;
    EXPECT_EQ(CastResult::Ok_NoCast, castValue(from, to));
    EXPECT_EQ(100, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastCharToInt)
{
    char from = 100;
    int to = 0;
    EXPECT_EQ(CastResult::Ok_NoCast, castValue(from, to));
    EXPECT_EQ(100, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastIntToChar)
{
    int from = 100;
    char to = 0;
    EXPECT_EQ(CastResult::Ok_WithCast, castValue(from, to));
    EXPECT_EQ(100, to);
}

//TODO: check signed char, sint8_t etc

