#include <gtest/gtest.h>
#include <Util/CastValue.h>
#include <cstdint>

// same types - Ok_NoCast

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastInt64ToInt64)
{
    int64_t from = 100, to = 0;
    EXPECT_EQ(CastResult::Ok_NoCast, castValue(from, to));
    EXPECT_EQ(100, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastUInt64ToUInt64)
{
    uint64_t from = 0x1234567890AB, to = 0;
    EXPECT_EQ(CastResult::Ok_NoCast, castValue(from, to));
    EXPECT_EQ(0x1234567890AB, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastIntToInt)
{
    int from = -2147483647, to = 0;
    EXPECT_EQ(CastResult::Ok_NoCast, castValue(from, to));
    EXPECT_EQ(-2147483647, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastUIntToUInt)
{
    unsigned int from = 0x8890abcd, to = 0;
    EXPECT_EQ(CastResult::Ok_NoCast, castValue(from, to));
    EXPECT_EQ(0x8890abcd, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastShortToShort)
{
    short from = 21345, to = 0;
    EXPECT_EQ(CastResult::Ok_NoCast, castValue(from, to));
    EXPECT_EQ(21345, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastUShortToUShort)
{
    unsigned short from = 35678, to = 0;
    EXPECT_EQ(CastResult::Ok_NoCast, castValue(from, to));
    EXPECT_EQ(35678, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastCharToChar)
{
    char from = 100, to = 0;
    EXPECT_EQ(CastResult::Ok_NoCast, castValue(from, to));
    EXPECT_EQ(100, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastSCharToSChar)
{
    signed char from = -100, to = 0;
    EXPECT_EQ(CastResult::Ok_NoCast, castValue(from, to));
    EXPECT_EQ(-100, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastUCharToUChar)
{
    unsigned char from = 100, to = 0;
    EXPECT_EQ(CastResult::Ok_NoCast, castValue(from, to));
    EXPECT_EQ(100, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastFloatToFloat)
{
    float from = 500.0f, to = 0;
    EXPECT_EQ(CastResult::Ok_NoCast, castValue(from, to));
    EXPECT_EQ(500.0f, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastDoubleToDouble)
{
    double from = 500.0, to = 0;
    EXPECT_EQ(CastResult::Ok_NoCast, castValue(from, to));
    EXPECT_EQ(500.0, to);
}

// smaller to bigger

// 8 bits to short
TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastCharToShort)
{
    char from = 40;
    short to = 0;
    EXPECT_EQ(CastResult::Ok_WithCast, castValue(from, to));
    EXPECT_EQ(40, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastSCharToShort)
{
    signed char from = 91;
    short to = 0;
    EXPECT_EQ(CastResult::Ok_WithCast, castValue(from, to));
    EXPECT_EQ(91, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastUCharToShort)
{
    unsigned char from = 50;
    short to = 0;
    EXPECT_EQ(CastResult::Ok_WithCast, castValue(from, to));
    EXPECT_EQ(50, to);
}

// 8 bits to ushort

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastCharToUShort)
{
    char from = 40;
    unsigned short to = 0;
    EXPECT_EQ(CastResult::Ok_WithCast, castValue(from, to));
    EXPECT_EQ(40, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsFailedCast_WhenCastNegativeSCharToUShort)
{
    signed char from = -1;
    unsigned short to = 0;
    EXPECT_EQ(CastResult::FailedCast, castValue(from, to));
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastUCharToUShort)
{
    unsigned char from = 59;
    unsigned short to = 0;
    EXPECT_EQ(CastResult::Ok_WithCast, castValue(from, to));
    EXPECT_EQ(59, to);
}

// 8 bits to int

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastCharToInt)
{
    char from = 127;
    int to = 0;
    EXPECT_EQ(CastResult::Ok_WithCast, castValue(from, to));
    EXPECT_EQ(127, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastSCharToInt)
{
    signed char from = 72;
    int to = 0;
    EXPECT_EQ(CastResult::Ok_WithCast, castValue(from, to));
    EXPECT_EQ(72, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastUCharToInt)
{
    uint8_t from = 126;
    int to = 0;
    EXPECT_EQ(CastResult::Ok_WithCast, castValue(from, to));
    EXPECT_EQ(126, to);
}

// 8 bits to uint

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastCharToUInt)
{
    char from = 17;
    unsigned int to = 0;
    EXPECT_EQ(CastResult::Ok_WithCast, castValue(from, to));
    EXPECT_EQ(17, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastSCharToUInt)
{
    signed char from = 22;
    unsigned int to = 0;
    EXPECT_EQ(CastResult::Ok_WithCast, castValue(from, to));
    EXPECT_EQ(22, to);
}

TEST(CastValue, RequireThat_CastValue_ReturnsFailedCast_WhenCastNegativeSCharToUInt)
{
    int8_t from = -127;
    unsigned int to = 0;
    EXPECT_EQ(CastResult::FailedCast, castValue(from, to));
}

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastUCharToUInt)
{
    unsigned char from = 116;
    int to = 0;
    EXPECT_EQ(CastResult::Ok_WithCast, castValue(from, to));
    EXPECT_EQ(116, to);
}


// bigger to smaller

TEST(CastValue, RequireThat_CastValue_ReturnsOkNoCast_WhenCastIntToChar)
{
    int from = 100;
    char to = 0;
    EXPECT_EQ(CastResult::Ok_WithCast, castValue(from, to));
    EXPECT_EQ(100, to);
}

//TODO: check signed char, sint8_t etc

