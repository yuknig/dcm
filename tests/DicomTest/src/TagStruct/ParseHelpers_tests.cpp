#include <Dicom/TagStruct/ParseHelpers.h>
#include <Util/StreamMem.h>
#include <gtest/gtest.h>

#include <initializer_list>
#include <memory>
#include <vector>

namespace
{

template <typename ByteT>
MemStreamRead<ByteT> CreateStream(const std::initializer_list<ByteT>& bytes)
{
    auto vec = std::make_shared<std::vector<ByteT>>(bytes);
    size_t offset = 50;
    vec->insert(vec->begin(), offset, 42);
    MemStreamRead<ByteT> stream(vec, offset);
    return stream;
}

} // namespace anonymous


template <typename T>
class ParseHelpers_tests : public testing::Test
{
protected:
    using ByteT = T;
};

// test on possible byte types
using ByteTypes = ::testing::Types<char, signed char, unsigned char>;
TYPED_TEST_SUITE(ParseHelpers_tests, ByteTypes);

TYPED_TEST(ParseHelpers_tests, RequireThat_GetTag_ReturnsNullopt_WhenStreamHas0BytesLeft)
{
    auto stream = CreateStream<ByteT>({});
    ASSERT_EQ(std::nullopt, dcm::GetTag(stream));
}

TYPED_TEST(ParseHelpers_tests, RequireThat_GetTag_ReturnsNullopt_WhenStreamHas1ByteLeft)
{
    auto stream = CreateStream<ByteT>({1});
    ASSERT_EQ(std::nullopt, dcm::GetTag(stream));
}

TYPED_TEST(ParseHelpers_tests, RequireThat_GetTag_ReturnsNullopt_WhenStreamHas2BytesLeft)
{
    auto stream = CreateStream<ByteT>({ 1, 2 });
    ASSERT_EQ(std::nullopt, dcm::GetTag(stream));
}

TYPED_TEST(ParseHelpers_tests, RequireThat_GetTag_ReturnsNullopt_WhenStreamHas3BytesLeft)
{
    auto stream = CreateStream<ByteT>({ 1, 2, 3 });
    ASSERT_EQ(std::nullopt, dcm::GetTag(stream));
}

TYPED_TEST(ParseHelpers_tests, RequireThat_GetTag_ReturnsCorrectTag_WhenStreamHas4BytesLeft)
{
    auto stream = CreateStream<ByteT>({ 1, 2, 3, 4 });
    auto res = dcm::GetTag(stream);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(dcm::Tag(0x0201, 0x0403), *res);
}
