#ifndef _CAST_VALUE_2B4B4879_93AE_47A7_881F_FD6A4A5F6544_
#define _CAST_VALUE_2B4B4879_93AE_47A7_881F_FD6A4A5F6544_

#include <type_traits>

/*
1. signed -> unsigned
smaller->bigger
bigger->smaller
2. unsigned ->signed
smaller->bigger
bigger->smaller
3. signed-> signed
smaller->bigger
bigger->smaller
4. unsigned->unsigned
smaller->bigger
bigger->smaller
6. Same types
*/

enum class CastResult
{
    FailedCast = 0,
    Ok_CastLosy,
    Ok_CastLoseless,
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

//template <typename FromT, typename ToT,
//          bool BothInt = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
//          bool FromSigned = std::is_signed<FromT>::value,
//          bool ToUnsigned = std::is_unsigned<ToT>::value,
//          typename std::enable_if<BothInt && FromSigned && ToUnsigned, void>::type* = 0>
//CastResult castValue(const FromT& a_from, ToT& a_to)
//{
//    static_assert(std::is_integral<FromT>::value, "From is not integral");
//    static_assert(std::is_integral<ToT>::value,   "ToT is not unsigned");
//    static_assert(std::is_signed<FromT>::value,   "FromT is not signed");
//    static_assert(std::is_unsigned<ToT>::value,   "ToT is not unsigned");
//
//    if (a_from < 0)
//        return CastResult::FailedCast;
//
//    a_to = static_cast<ToT>(a_from);
//    return CastResult::Ok_CastLoseless;
//}

template <typename FromT, typename ToT,
          bool BothInt = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
          bool FromSigned = std::is_signed<FromT>::value,
          bool ToUnsigned = std::is_signed<ToT>::value,
          bool ToBigger   = sizeof(FromT) <= sizeof(ToT),
          typename std::enable_if<BothInt && FromSigned && ToUnsigned && ToBigger, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    static_assert(std::is_integral<FromT>::value, "From is not integral");
    static_assert(std::is_integral<ToT>::value,   "ToT is not unsigned");
    static_assert(std::is_unsigned<FromT>::value, "FromT is not unsigned");
    static_assert(std::is_signed<ToT>::value,     "ToT is not signed");
    static_assert(sizeof(FromT) <= sizeof(ToT),   "Wrong sizes");

    assert(static_cast<ToT>(a_from) <= std::numeric_limits<ToT>::max());

    a_to = static_cast(a_from);
    return CastResult::Ok_CastLoseless;
}

template <typename FromT, typename ToT,
          bool BothInt    = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
          bool FromSigned = std::is_signed<FromT>::value,
          bool ToUnsigned = std::is_unsigned<ToT>::value,
          bool ToSmaller  = (sizeof(FromT) > sizeof(ToT)),
          typename std::enable_if<BothInt && FromSigned && ToUnsigned && ToSmaller, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    static_assert(std::is_integral<FromT>::value, "From is not integral");
    static_assert(std::is_integral<ToT>::value,   "ToT is not integral");
    static_assert(std::is_signed<FromT>::value,   "FromT is not signed");
    static_assert(std::is_unsigned<ToT>::value,   "ToT is not unsigned");
    static_assert(sizeof(FromT) > sizeof(ToT),    "Wrong sizes");

    if (a_from < 0)
        return CastResult::FailedCast;

    if (static_cast<std::make_unsigned<FromT>::type>(a_from) > std::numeric_limits<ToT>::max())
        return CastResult::FailedCast;

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLoseLess;
}

template <typename FromT, typename ToT,
          bool BothInt      = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
          bool FromUnsigned = std::is_unsigned<FromT>::value,
          bool ToSigned     = std::is_signed<ToT>::value,
          bool ToBigger     = (sizeof(FromT) < sizeof(ToT)),
          typename std::enable_if<BothInt && FromUnsigned && ToSigned && ToBigger, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    static_assert(std::is_integral<FromT>::value, "From is not integral");
    static_assert(std::is_integral<ToT>::value,   "ToT is not integral");
    static_assert(std::is_unsigned<FromT>::value, "Wrong FromT type");
    static_assert(std::is_signed<ToT>::value,     "Wrong ToT type");
    static_assert(sizeof(FromT) < sizeof(ToT),    "Wrong sizes");

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLoseless;
}

template <typename FromT, typename ToT,
          bool BothInt      = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
          bool FromUnsigned = std::is_unsigned<FromT>::value,
          bool ToSigned     = std::is_signed<ToT>::value,
          bool ToSmaller    = (sizeof(FromT) >= sizeof(ToT)),
          typename std::enable_if<BothInt && FromUnsigned && ToSigned && ToSmaller, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    static_assert(std::is_integral<FromT>::value, "From is not integral");
    static_assert(std::is_integral<ToT>::value,   "ToT is not integral");
    static_assert(std::is_unsigned<FromT>::value, "Wrong FromT type");
    static_assert(std::is_signed<ToT>::value,     "Wrong ToT type");
    static_assert(sizeof(FromT) >= sizeof(ToT),    "Wrong sizes");

    if (static_cast<std::make_signed<FromT>::type>(a_from) > std::numeric_limits<ToT>::max())
        return CastResult::FailedCast;

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLoseless;
}

template <typename FromT, typename ToT,
          bool BothInt  = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
          bool SameSign = (std::is_unsigned<FromT>::value == std::is_unsigned<ToT>::value),
          bool ToBigger = (sizeof(FromT) > sizeof(ToT)),
          typename std::enable_if<BothInt && SameSign && ToBigger, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    static_assert(std::is_integral<FromT>::value, "From is not integral");
    static_assert(std::is_integral<ToT>::value,   "ToT is not integral");
    static_assert(std::is_unsigned<FromT>::value == std::is_unsigned<ToT>::value, "Diff sign types");
    static_assert(sizeof(FromT) < sizeof(ToT),    "Wrong sizes");

    a_to = a_from;
    return CastResult::Ok_CastLoseless;
}

template <typename FromT, typename ToT,
          bool BothInt    = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
          bool BothSigned = (std::is_signed<FromT>::value && std::is_signed<ToT>::value),
          bool ToSmaller  = (sizeof(FromT) > sizeof(ToT)),
          typename std::enable_if<BothInt && BothSigned && ToSmaller, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    static_assert(std::is_integral<FromT>::value, "From is not integral");
    static_assert(std::is_integral<ToT>::value,   "ToT is not integral");
    static_assert(std::is_signed<FromT>::value,   "Expected signed FromT");
    static_assert(std::is_signed<ToT>::value,     "Expected signed ToT");
    static_assert(sizeof(FromT) > sizeof(ToT),    "Wrong sizes");

    if (a_from <= std::numeric_limits<ToT>::min())
        return CastResult::FailedCast;

    if (a_from >= std::numeric_limits<ToT>::max())
        return CastResult::FailedCast;

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLoseless;
}

template <typename FromT, typename ToT,
          bool BothInt      = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
          bool BothUnsigned = (std::is_unsigned<FromT>::value && std::is_unsigned<ToT>::value),
          bool ToSmaller    = (sizeof(FromT) > sizeof(ToT)),
          typename std::enable_if<BothInt && BothUnsigned && ToSmaller, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    static_assert(std::is_integral<FromT>::value, "From is not integral");
    static_assert(std::is_integral<ToT>::value, "ToT is not integral");
    static_assert(std::is_unsigned<FromT>::value, "Expected unsigned FromT");
    static_assert(std::is_unsigned<ToT>::value, "Expected unsigned ToT");
    static_assert(sizeof(FromT) > sizeof(ToT), "Wrong sizes");

    if (a_from >= std::numeric_limits<ToT>::max())
        return CastResult::FailedCast;

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLoseless;
}

template <typename FromT, typename ToT,
          bool BothFloat = (std::is_floating_point<FromT>::value && std::is_floating_point<ToT>::value),
          bool ToBigger = (sizeof(FromT) < sizeof(ToT)),
          typename std::enable_if<BothFloat && ToBigger, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLoseless;
}

template <typename FromT, typename ToT,
          bool BothFloat = (std::is_floating_point<FromT>::value && std::is_floating_point<ToT>::value),
          bool ToSmaller = (sizeof(FromT) < sizeof(ToT)),
          typename std::enable_if<BothFloat && ToSmaller, void>::type* = 0>
CastResult castValue(const FromT& a_from, ToT& a_to)
{
    a_to = static_cast<ToT>(a_from);
    return (static_cast<FromT>(a_to) == a_from) ? CastResult::Ok_CastLoseless : CastResult::Ok_CastLosy;
}



//template <typename FromT, typename ToT,
//          bool Same = std::is_same<FromT, ToT>::value,
//          typename std::enable_if<Same, void>::type* = 0>
//CastResult castValue(const FromT& a_from, ToT& a_to)
//{
//    static_assert(std::is_same<FromT, ToT>::value, "Isn't same types");
//
//    a_to = a_from;
//    return CastResult::Ok_NoCast;
//}
//
//template <typename FromT, typename ToT,
//          bool BothInt = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
//          bool SameSign = std::is_signed<FromT>::value == std::is_signed<ToT>::value,
//          bool FromLower = sizeof(FromT) < sizeof(ToT),
//          typename std::enable_if<BothInt && SameSign && FromLower, void>::type* = 0>
//CastResult castValue(const FromT& a_from, ToT& a_to)
//{
//    using CommonT = std::common_type<FromT, ToT>::type;
//    CommonT ct = 0;
//    (void)ct;
//
//    static_assert(!std::is_same<FromT, ToT>::value, "Is same types");
//    static_assert(std::is_integral<FromT>::value, "From isn't integer");
//    static_assert(std::is_integral<ToT>::value, "To isn't integer");
//    static_assert(static_cast<CommonT>(std::numeric_limits<FromT>::lowest()) >= static_cast<CommonT>(std::numeric_limits<ToT>::lowest()), "Wrong left range");
//    static_assert(static_cast<CommonT>(std::numeric_limits<FromT>::max())    <= static_cast<CommonT>(std::numeric_limits<ToT>::max()), "Wrong right range");
//
//    a_to = static_cast<ToT>(a_from);
//    return CastResult::Ok_WithCast;
//}
//
//template <typename FromT, typename ToT,
//          bool BothInt = (std::is_integral<FromT>::value && std::is_integral<ToT>::value),
//          bool FromMinLower = (std::numeric_limits<FromT>::lowest() < std::numeric_limits<ToT>::lowest() &&
//                               std::numeric_limits<FromT>::max() <= std::numeric_limits<ToT>::max()),
//          typename std::enable_if<BothInt && FromMinLower, void>::type* = 0>
//CastResult castValue(const FromT& a_from, ToT& a_to)
//{
//    static_assert(std::is_integral<FromT>::value, "From isn't integer");
//    static_assert(std::is_integral<ToT>::value, "To isn't integer");
//    static_assert(std::numeric_limits<FromT>::lowest() < std::numeric_limits<ToT>::lowest(), "Wrong left range");
//    static_assert(std::numeric_limits<FromT>::max() <= std::numeric_limits<ToT>::max(), "Wrong right range");
//
//    if (a_from < std::numeric_limits<ToT>::lowest())
//        return CastResult::FailedCast;
//
//    a_to = static_cast<ToT>(a_from);
//    return CastResult::Ok_WithCast;
//}
//
//template <typename FromT, typename ToT,
//          typename From = FromT, // dummy
//          bool BothInt = (std::is_integral<From>::value && std::is_integral<ToT>::value),
//          bool FromeWider = (std::numeric_limits<FromT>::lowest() < std::numeric_limits<ToT>::lowest() &&
//                             std::numeric_limits<FromT>::max() > std::numeric_limits<ToT>::max()),
//          typename std::enable_if<BothInt && FromeWider, void>::type* = 0>
//CastResult castValue(const FromT& a_from, ToT& a_to)
//{
//    static_assert(std::is_integral<FromT>::value, "FromT isn't integer");
//    static_assert(std::is_integral<ToT>::value, "ToT isn't integer");
//    static_assert(std::numeric_limits<FromT>::lowest() < std::numeric_limits<ToT>::lowest(), "Unexpected left range");
//    static_assert(std::numeric_limits<FromT>::max() > std::numeric_limits<ToT>::max(), "Unexpected right range");
//
//    if (a_from < std::numeric_limits<ToT>::lowest() ||
//        a_from > std::numeric_limits<ToT>::max())
//    {
//        return CastResult::FailedCast;
//    }
//
//    a_to = static_cast<ToT>(a_from);
//    return CastResult::Ok_WithCast;
//}
//
//template <typename FromT, typename ToT,
//          bool FromFloat = std::is_floating_point<FromT>::value,
//          bool ToInt = std::is_integral<ToT>::value,
//          typename std::enable_if<FromFloat && ToInt, void>::type* = 0,
//          typename Dummy1 = void, typename Dummy2 = void>
//CastResult castValue(const FromT& a_from, ToT& a_to)
//{
//    static_assert(std::is_floating_point<FromT>::value, "FromT isn't float point");
//    static_assert(std::is_integral<ToT>::value, "ToT isn't integer");
//
//    const auto iFrom = std::lround(a_from);
//    if (iFrom < std::numeric_limits<ToT>::lowest() ||
//        iFrom > std::numeric_limits<ToT>::max())
//    {
//        return CastResult::FailedCast;
//    }
//
//    a_to = static_cast<ToT>(iFrom);
//    return CastResult::Ok_WithCast;
//}
//
//template <typename FromT, typename ToT, 
//          bool FromInt = std::is_integral<FromT>::value,
//          bool ToFloat = std::is_floating_point<ToT>::value,
//          typename std::enable_if<FromInt && ToFloat, void>::type* = 0,
//          typename Dummy1 = void, typename Dummy2 = void, typename Dummy3 = void>
//CastResult castValue(const FromT& a_from, ToT& a_to)
//{
//    static_assert(std::is_integral<FromT>::value, "FromT isn't integer");
//    static_assert(std::is_floating_point<ToT>::value, "ToT isn't float point");
//
//    a_to = static_cast<ToT>(iFrom);
//    return CastResult::Ok_WithCast;
//}

//#include "CastValue.inl"

#endif // _CAST_VALUE_2B4B4879_93AE_47A7_881F_FD6A4A5F6544_
