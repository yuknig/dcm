#include <gtest/gtest.h>
#include <Util/CastValue.h>
#include <cstdint>
#include <typeinfo>

using namespace testing;

namespace
{

template <typename T>
bool AreEqual(const T& a_lhs, const T& a_rhs) {
    return a_lhs == a_rhs;
}

} // namespace anonymous

struct Case {
    template <typename FromT, typename ToT>
    explicit Case(const FromT& a_from, CastResult a_expected_result, ToT a_expected_value = {})
        : m_type_from(typeid(FromT).name())
        , m_type_to(typeid(ToT).name())
        , m_expected_result(a_expected_result)
    {
        ToT value = {};
        m_actual_result = CastValue<FromT, ToT>(a_from, value);
        if (Succeeded(m_actual_result))
            m_values_equal = AreEqual(a_expected_value, value);
    }

    const char* m_type_from;
    const char* m_type_to;
    CastResult m_expected_result = CastResult::FailedCast;
    CastResult m_actual_result = CastResult::FailedCast;
    bool m_values_equal = false;
};

std::ostream& operator<<(std::ostream& a_os, const Case& a_case) {
    a_os << "<From=" << a_case.m_type_from << ", To=" << a_case.m_type_to << ">";
    return a_os;
}

class CastValue_Tests : public TestWithParam<Case>
{};

TEST_P(CastValue_Tests,
       RequireThat_CastResult_ReturnsExpectedValue_WhenCalled) {
    const auto& param = GetParam();
    ASSERT_EQ(param.m_expected_result, param.m_actual_result);
    if (Succeeded(param.m_actual_result))
        ASSERT_TRUE(param.m_values_equal);
}

INSTANTIATE_TEST_CASE_P(CastValue_Tests, CastValue_Tests,
    ValuesIn(std::initializer_list<Case> {
//// to same types
/// 8 bits types
// we don't know whether char signed or not
    Case(static_cast<char>(0), CastResult::Ok_NoCast, static_cast<char>(0)),
    Case(static_cast<char>(SCHAR_MAX), CastResult::Ok_NoCast, static_cast<char>(SCHAR_MAX)),
// signed char
    Case(static_cast<signed char>(0), CastResult::Ok_NoCast, static_cast<signed char>(0)),
    Case(static_cast<signed char>(SCHAR_MAX), CastResult::Ok_NoCast, static_cast<signed char>(SCHAR_MAX)),
    Case(static_cast<signed char>(SCHAR_MIN), CastResult::Ok_NoCast, static_cast<signed char>(SCHAR_MIN)),
// unsigned char
    Case(static_cast<unsigned char>(0), CastResult::Ok_NoCast, static_cast<unsigned char>(0)),
    Case(static_cast<unsigned char>(UCHAR_MAX), CastResult::Ok_NoCast, static_cast<unsigned char>(UCHAR_MAX)),
/// 16 bits types
// signed short
    Case(static_cast<signed short>(0), CastResult::Ok_NoCast, static_cast<signed short>(0)),
    Case(static_cast<signed short>(SHRT_MAX), CastResult::Ok_NoCast, static_cast<signed short>(SHRT_MAX)),
    Case(static_cast<signed short>(SHRT_MIN), CastResult::Ok_NoCast, static_cast<signed short>(SHRT_MIN)),
// unsigned short
    Case(static_cast<unsigned short>(0), CastResult::Ok_NoCast, static_cast<unsigned short>(0)),
    Case(static_cast<unsigned short>(USHRT_MAX), CastResult::Ok_NoCast, static_cast<unsigned short>(USHRT_MAX)),
// 32 bits types
// signed int
    Case(static_cast<int>(0), CastResult::Ok_NoCast, static_cast<int>(0)),
    Case(static_cast<int>(INT_MAX), CastResult::Ok_NoCast, static_cast<int>(INT_MAX)),
    Case(static_cast<int>(INT_MIN), CastResult::Ok_NoCast, static_cast<int>(INT_MIN)),
// unsigned int
    Case(static_cast<unsigned int>(0), CastResult::Ok_NoCast, static_cast<unsigned int>(0)),
    Case(static_cast<unsigned int>(UINT_MAX), CastResult::Ok_NoCast, static_cast<unsigned int>(UINT_MAX)),
/// float point types
// float
    Case(0.0f, CastResult::Ok_NoCast, 0.0f),
    Case(FLT_MAX, CastResult::Ok_NoCast, FLT_MAX),
    Case(-FLT_MAX, CastResult::Ok_NoCast, -FLT_MAX),
// double
    Case(0.0, CastResult::Ok_NoCast, 0.0),
    Case(DBL_MAX, CastResult::Ok_NoCast, DBL_MAX),
    Case(-DBL_MAX, CastResult::Ok_NoCast, -DBL_MAX),
// long double
    Case(static_cast<long double>(0), CastResult::Ok_NoCast, static_cast<long double>(0)),
    Case(static_cast<long double>(LDBL_MAX), CastResult::Ok_NoCast, static_cast<long double>(LDBL_MAX)),
    Case(static_cast<long double>(-LDBL_MAX), CastResult::Ok_NoCast, static_cast<long double>(-LDBL_MAX)),
}));
