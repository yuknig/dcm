#include <Dicom/TagStruct/ParseHelpers.h>
#include <Util/StreamMem.h>
#include <gtest/gtest.h>

#include <initializer_list>
#include <memory>
#include <vector>
#include <type_traits>
#include <utility>

namespace
{

namespace detail
{

template <typename ByteT, typename ValueT>
void PushToVector(std::vector<ByteT>& a_vector, ValueT&& a_value)
{
    static_assert(std::is_arithmetic<std::decay<ValueT>::type>::value, "Wrong type");

    const ByteT* src_ptr = reinterpret_cast<const ByteT*>(&a_value);
    for (size_t byte_num = 0; byte_num < sizeof(ValueT); ++byte_num)
        a_vector.push_back(src_ptr[byte_num]); // assuming Little Endian byte order
}

template <typename ByteT, typename FirstT, typename... RestT>
void PushToVector(std::vector<ByteT>& a_vector, FirstT&& a_first, RestT&&... a_rest)
{
    PushToVector(a_vector, a_first);
    PushToVector(a_vector, std::forward<RestT>(a_rest)...);
}

} // namespace detail


template <typename ByteT, typename... ArgsT>
std::vector<ByteT> PushToVector(ArgsT&&... a_args)
{
    std::vector<ByteT> vec;
    detail::PushToVector(vec, std::forward<ArgsT>(a_args)...);
    return vec;
}

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
using ByteTypes = testing::Types<char, signed char, unsigned char>;
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


struct GetTagAndVr_Case
{
public:
    template <typename ByteT>
    GetTagAndVr_Case(std::vector<ByteT> a_stream_data, bool a_explicit, std::optional<std::pair<dcm::Tag, dcm::VRType>> a_expectedResult)
        : m_expected_result(a_expectedResult)
    {
        auto vec = std::make_shared<std::vector<ByteT>>(std::move(a_stream_data));
        MemStreamRead<ByteT> stream(vec);
        m_actual_result = dcm::GetTagAndVr(stream, a_explicit);
    }

    std::optional<std::pair<dcm::Tag, dcm::VRType>> m_actual_result;
    std::optional<std::pair<dcm::Tag, dcm::VRType>> m_expected_result;
};

class ParseHelpers_GetTagAndVr_tests : public testing::TestWithParam<GetTagAndVr_Case>
{};

TEST_P(ParseHelpers_GetTagAndVr_tests, RequireThat_GetTagAndVr_ReturnsValueThatAgreesWithReference_WhenCalled)
{
    const auto& param = GetParam();

    ASSERT_EQ(param.m_expected_result, param.m_actual_result);
}

INSTANTIATE_TEST_CASE_P(ParseHelpers, ParseHelpers_GetTagAndVr_tests,
    testing::ValuesIn(std::initializer_list<GetTagAndVr_Case>
{
    // return nullopt on empty stream
    { std::vector<char>{}, true, std::nullopt },
    { std::vector<char>{}, false, std::nullopt },

    // return nullptr on tag nummer in stream when VR is explicit
    { PushToVector<char>(0x12345678), true, std::nullopt },

    // return tag nummer and VR on tag number in stream when VR is implicit
    { PushToVector<char>(0x00200008)/*StudyDate tag*/, false, std::nullopt },
}
));
