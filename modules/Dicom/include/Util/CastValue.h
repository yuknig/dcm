#ifndef _CAST_VALUE_2B4B4879_93AE_47A7_881F_FD6A4A5F6544_
#define _CAST_VALUE_2B4B4879_93AE_47A7_881F_FD6A4A5F6544_

#include <type_traits>

enum class CastResult
{
    FailedCast = 0,
    Ok_WithCast,
    Ok_NoCast
};

template <typename FromT, typename ToT,
          bool Same = std::is_same<FromT, ToT>::value,
          typename std::enable_if<Same, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    static_assert(std::is_same<FromT, ToT>::value, "Isn't same types");

    a_to = a_from;
    return CastResult::Ok_NoCast;
}

template <typename FromT, typename ToT,
          bool NotSame = !std::is_same<FromT, ToT>::value,
          bool BothInt = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
          bool ContainsRange = (std::numeric_limits<FromT>::lowest() >= std::numeric_limits<ToT>::lowest() &&
                                std::numeric_limits<FromT>::max() <= std::numeric_limits<ToT>::max()),
          typename std::enable_if<NotSame && BothInt && ContainsRange, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    using CommonT = std::common_type<FromT, ToT>::type;
    CommonT ct = 0;
    (void)ct;

    static_assert(!std::is_same<FromT, ToT>::value, "Is same types");
    static_assert(std::is_integral<FromT>::value, "From isn't integer");
    static_assert(std::is_integral<ToT>::value, "To isn't integer");
    static_assert(static_cast<CommonT>(std::numeric_limits<FromT>::lowest()) >= static_cast<CommonT>(std::numeric_limits<ToT>::lowest()), "Wrong left range");
    static_assert(static_cast<CommonT>(std::numeric_limits<FromT>::max())    <= static_cast<CommonT>(std::numeric_limits<ToT>::max()), "Wrong right range");

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_WithCast;
}

template <typename FromT, typename ToT,
          bool BothInt = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
          bool FromMinLower = (std::numeric_limits<FromT>::lowest() < std::numeric_limits<ToT>::lowest() &&
                               std::numeric_limits<FromT>::max() <= std::numeric_limits<ToT>::max()),
          typename std::enable_if<BothInt && FromMinLower, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    static_assert(std::is_integral<FromT>::value, "From isn't integer");
    static_assert(std::is_integral<ToT>::value, "To isn't integer");
    static_assert(std::numeric_limits<FromT>::lowest() < std::numeric_limits<ToT>::lowest(), "Wrong left range");
    static_assert(std::numeric_limits<FromT>::max() <= std::numeric_limits<ToT>::max(), "Wrong right range");

    if (a_from < std::numeric_limits<ToT>::lowest())
        return CastResult::FailedCast;

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_WithCast;
}

template <typename FromT, typename ToT,
          typename From = FromT, // dummy
          bool BothInt = (std::is_integral<From>::value && std::is_integral<ToT>::value),
          bool FromeWider = (std::numeric_limits<FromT>::lowest() < std::numeric_limits<ToT>::lowest() &&
                             std::numeric_limits<FromT>::max() > std::numeric_limits<ToT>::max()),
          typename std::enable_if<BothInt && FromeWider, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    static_assert(std::is_integral<FromT>::value, "FromT isn't integer");
    static_assert(std::is_integral<ToT>::value, "ToT isn't integer");
    static_assert(std::numeric_limits<FromT>::lowest() < std::numeric_limits<ToT>::lowest(), "Unexpected left range");
    static_assert(std::numeric_limits<FromT>::max() > std::numeric_limits<ToT>::max(), "Unexpected right range");

    if (a_from < std::numeric_limits<ToT>::lowest() ||
        a_from > std::numeric_limits<ToT>::max())
    {
        return CastResult::FailedCast;
    }

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_WithCast;
}

template <typename FromT, typename ToT,
          bool FromFloat = std::is_floating_point<FromT>::value,
          bool ToInt = std::is_integral<ToT>::value,
          typename std::enable_if<FromFloat && ToInt, void>::type* = 0,
          typename Dummy1 = void, typename Dummy2 = void>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    static_assert(std::is_floating_point<FromT>::value, "FromT isn't float point");
    static_assert(std::is_integral<ToT>::value, "ToT isn't integer");

    const auto iFrom = std::lround(a_from);
    if (iFrom < std::numeric_limits<ToT>::lowest() ||
        iFrom > std::numeric_limits<ToT>::max())
    {
        return CastResult::FailedCast;
    }

    a_to = static_cast<ToT>(iFrom);
    return CastResult::Ok_WithCast;
}

template <typename FromT, typename ToT, 
          bool FromInt = std::is_integral<FromT>::value,
          bool ToFloat = std::is_floating_point<ToT>::value,
          typename std::enable_if<FromInt && ToFloat, void>::type* = 0,
          typename Dummy1 = void, typename Dummy2 = void, typename Dummy3 = void>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    static_assert(std::is_integral<FromT>::value, "FromT isn't integer");
    static_assert(std::is_floating_point<ToT>::value, "ToT isn't float point");

    a_to = static_cast<ToT>(iFrom);
    return CastResult::Ok_WithCast;
}

//#include "CastValue.inl"

#endif // _CAST_VALUE_2B4B4879_93AE_47A7_881F_FD6A4A5F6544_
