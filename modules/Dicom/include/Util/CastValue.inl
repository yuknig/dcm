//#include "CastValue.h"
#pragma once
#include <cstdint>
#include <type_traits>

//enum class CastResult
//{
//    FailedCast = 0,
//    Ok_WithCast,
//    Ok_NoCast
//};

template <typename FromT, typename ToT,
          bool Same = std::is_same<FromT, ToT>::value,
          typename std::enable_if<Same, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
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






//template <typename ToT, typename FromT>
//CastResult castValue(ToT& /*a_to*/, /*const FromT& a_from*/ ...)
//{
//    return GetValueResult::FailedCast;
//}
//
//// Same types. No, cast,just copy value
//template <typename ToT, typename FromT>
//          bool IsSameT = std::is_same<FromT, ToT>::value,
//          typename std::enable_if<IsSameT, int>::type = 0>
//CastResult castValue(ToT& a_to, const FromT& a_from)
//{
//    static_assert(std::is_same<FromT, ToT>::value, "Impl for same type");
//
//    a_to = m_from;
//    return CastResult::Ok_NoCast;
//}
//
//template <typename ToT, typename FromT>
//          bool BothInteger = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
//          bool ToBigger = (sizeof(ToT) > sizeof(FromT)),
//          typename std::enable_if<BothInteger && ToBigger, int>::type = 0>
//CastResult castValue(ToT& a_to, const FromT& a_from)
//{
//    static_assert(std::is_integral<FromT>::value, "FromT is integral");
//    static_assert(std::is_integral<ToT>::value,   "ToT is integral");
//    static_assert(sizeof(ToT) > sizeof(FromT));
//
//    a_to = static_cast<ToT>(m_from);
//    return CastResult::Ok_WithCast;7
//}
//
//template <typename ToT, typename FromT>
//          bool BothInteger = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
//          bool ToSamller = (sizeof(ToT) < sizeof(FromT)),
//          bool SameSigned = (std::is_signed(ToT)::value == std::is_signed(FromT)),
//          typename std::enable_if<BothInteger && ToBigger, int>::type = 0>
//CastResult castValue(ToT& a_to, const FromT& a_from)
//{
//    static_assert(std::is_integral(FromT), "FromT is integral");
//    static_assert(std::is_integral(ToT),   "ToT is integral");
//    static_assert(sizeof(ToT) > sizeof(FromT));
//
//    a_to = static_cast<ToT>(m_from);
//    return CastResult::Ok_WithCast;
//}


// TODO: call aux functions (castSignedToUnsigned, castDown, etc)
//template <typename ToT, typename FromT>
//CastResult castValue(ToT& a_to, const FromT& a_from)
//{
//    using CommonT = std::common_type<ToT, FromT>::type;
//
//    if (std::is_same<ToT, FromT>::value) // same types
//    {
//        a_to = a_a_from;
//        return CastResult::Ok_NoCast;
//    }
//    else if (std::is_integral<ToT>::value && std::is_integral<FromT>::value) // both integers
//    {
//        if (sizeof(ToT) > sizeof(FromT))
//        {
//            a_to = static_cast<ToT>(a_fromT);
//            return CastResult::Ok_WithCast;
//        }
//        else
//        {
//            if ((static_cast<CommonT>(std::numeric_limits<To>::lowest()) > static_cast<CommonT>(*a_from)) ||
//                (static_cast<CommonT>(std::numeric_limits<To>::max())    < static_cast<CommonT>(*a_from)))
//            {
//                return GetValueResult::FailedCast;
//            }
//            else
//            {
//                a_to = static_cast<ToT>(a_fromT);
//                return CastResult::Ok_WithCast;
//            }
//        }
//    }
//    else if (std::is_integral<ToT>::value && std::is_floating_point<FromT>::value)
//    {
//        const auto iFrom = std::lround(a_from);
//        using CommonIntT = std::common_type<ToT, decltype(iFrom)>::type;
//
//        if ((static_cast<CommonIntT>(std::numeric_limits<To>::lowest()) > static_cast<CommonIntT>(*iFrom)) ||
//            (static_cast<CommonIntT>(std::numeric_limits<To>::max())    < static_cast<CommonIntT>(*iFrom)))
//        {
//            return GetValueResult::FailedCast;
//        }
//        else
//        {
//            a_to = static_cast<ToT>(iFrom);
//            return CastResult::Ok_WithCast;
//        }
//    }
//    else if (std::is_floating_point<ToT>::value && std::is_integral<FromT>::value)
//    {
//        a_to = static_cast<ToT>(a_from);
//        return CastResult::Ok_WithCast;
//    }
//
//    assert(false);
//    return CastResult::FailedCast;
//}


// otherwise
