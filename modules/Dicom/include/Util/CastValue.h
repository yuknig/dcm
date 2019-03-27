#ifndef _CAST_VALUE_2B4B4879_93AE_47A7_881F_FD6A4A5F6544_
#define _CAST_VALUE_2B4B4879_93AE_47A7_881F_FD6A4A5F6544_

#include <type_traits>

enum class CastResult
{
    FailedCast = 0,
    Ok_CastLossy,
    Ok_CastLoseless,
    Ok_NoCast
};

namespace detail
{

template <typename FromT, typename ToT>
inline CastResult CastValue_Same(const FromT& a_from, ToT& a_to) {
    assert((std::is_same<FromT, ToT>::value));
    a_to = static_cast<ToT>(a_from);

    return CastResult::Ok_NoCast;
}

template <typename FromT, typename ToT>
inline CastResult CastValue_FromInt_ToInt_SameSign_ToBigger(const FromT& a_from, ToT& a_to) {
    assert(std::is_integral<FromT>::value);
    assert(std::is_integral<ToT>::value);
    assert(std::is_signed<FromT>::value == std::is_signed<ToT>::value);
    assert(sizeof(FromT) <= sizeof(ToT));

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLoseless;
}

template <typename FromT, typename ToT>
inline CastResult CastValue_FromUnsignedInt_ToUnsignedInt_ToSmaller(const FromT& a_from, ToT& a_to) {
    assert(std::is_integral<FromT>::value);
    assert(std::is_unsigned<FromT>::value);
    assert(std::is_integral<ToT>::value);
    assert(std::is_unsigned<FromT>::value);
    assert(sizeof(FromT) > sizeof(ToT));

    if (a_from > static_cast<FromT>(std::numeric_limits<ToT>::max()))
        return CastResult::FailedCast;

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLoseless;
}

template <typename FromT, typename ToT>
inline CastResult CastValue_FromSignedInt_ToSignedInt_ToSmaller(const FromT& a_from, ToT& a_to) {
    assert(std::is_integral<FromT>::value);
    assert(std::is_signed<FromT>::value);
    assert(std::is_integral<ToT>::value);
    assert(std::is_signed<FromT>::value);
    assert(sizeof(FromT) > sizeof(ToT));

    if (a_from < static_cast<FromT>(std::numeric_limits<ToT>::lowest()))
        return CastResult::FailedCast;
    if (a_from > static_cast<FromT>(std::numeric_limits<ToT>::max()))
        return CastResult::FailedCast;

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLoseless;
}

template <typename FromT, typename ToT>
inline CastResult CastValue_FromSignedInt_ToUnsignedInt_ToBigger(const FromT& a_from, ToT& a_to) {
    assert(std::is_integral<FromT>::value);
    assert(std::is_signed<FromT>::value);
    assert(std::is_integral<ToT>::value);
    assert(std::is_unsigned<FromT>::value);
    assert(sizeof(FromT) <= sizeof(ToT));

    if (a_from < 0)
        return CastResult::FailedCast;

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLoseless;
}

template <typename FromT, typename ToT>
inline CastResult CastValue_FromSignedInt_ToUnsignedInt_ToSmaller(const FromT& a_from, ToT& a_to) {
    assert(std::is_integral<FromT>::value);
    assert(std::is_signed<FromT>::value);
    assert(std::is_integral<ToT>::value);
    assert(std::is_unsigned<FromT>::value);
    assert(sizeof(FromT) > sizeof(ToT));

    if (a_from < 0)
        return CastResult::FailedCast;

    if (a_from > static_cast<FromT>(std::numeric_limits<ToT>::max()))
        return CastResult::FailedCast;

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLoseless;
}

template <typename FromT, typename ToT>
inline CastResult CastValue_FromUnsignedInt_ToSignedInt_ToBigger(const FromT& a_from, ToT& a_to) {
    assert(std::is_integral<FromT>::value);
    assert(std::is_unsigned<FromT>::value);
    assert(std::is_integral<ToT>::value);
    assert(std::is_signed<FromT>::value);
    assert(sizeof(FromT) < sizeof(ToT));

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLoseless;
}


template <typename FromT, typename ToT>
inline CastResult CastValue_FromUnsignedInt_ToSignedInt_ToSmaller(const FromT& a_from, ToT& a_to) {
    assert(std::is_integral<FromT>::value);
    assert(std::is_unsigned<FromT>::value);
    assert(sizeof(FromT) >= sizeof(ToT));

    if (a_from > static_cast<FromT>(std::numeric_limits<ToT>::max()))
        return CastResult::FailedCast;

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLoseless;
}

template <typename FromT, typename ToT>
inline CastResult CastValue_FromInt_ToFloat(const FromT& a_from, ToT& a_to) {
    assert(std::is_integral<FromT>::value);
    assert(std::is_floating_point<ToT>::value);

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLossy;
}

template <typename FromT, typename ToT>
inline CastResult CastValue_FromFloat_ToInt(const FromT& a_from, ToT& a_to) {
    assert(std::is_floating_point<FromT>::value);
    assert(std::is_integral<ToT>::value);

    if (a_from > static_cast<FromT>(std::numeric_limits<ToT>::max()) ||
        a_from < static_cast<FromT>(std::numeric_limits<ToT>::lowest()))
        return CastResult::FailedCast;

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLossy;
}

template <typename FromT, typename ToT>
inline CastResult CastValue_FromFloat_ToDouble(const FromT& a_from, ToT& a_to) {
    assert(std::is_floating_point<FromT>::value);
    assert(std::is_floating_point<ToT>::value);
    assert(sizeof(FromT) > sizeof(ToT));

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_NoCast;
}

template <typename FromT, typename ToT>
inline CastResult CastValue_FromDouble_ToFloat(const FromT& a_from, ToT& a_to) {
    assert(std::is_floating_point<FromT>::value);
    assert(std::is_floating_point<ToT>::value);
    assert(sizeof(FromT) < sizeof(ToT));

    a_to = static_cast<ToT>(a_from);
    return CastResult::Ok_CastLossy;
}


} // namespace detail

template <typename FromT, typename ToT>
CastResult CastValue(const FromT& a_from, ToT& a_to) {
    // TODO: replace 'if' with 'constexpr if' if 17 standard is supported

    if (std::is_same<FromT, ToT>::value)
        return detail::CastValue_Same(a_from, a_to);
    else if (std::is_integral<FromT>::value)
    {
        if (std::is_integral<ToT>::value)
        {
            if (std::is_signed<FromT>::value == std::is_signed<ToT>::value)
            {
                if (sizeof(FromT) <= sizeof(ToT))
                    return detail::CastValue_FromInt_ToInt_SameSign_ToBigger(a_from, a_to);
                else
                {
                    if (std::is_unsigned<FromT>::value) // TODO: to ternary operator
                        return detail::CastValue_FromUnsignedInt_ToUnsignedInt_ToSmaller(a_from, a_to);
                    else
                        return detail::CastValue_FromSignedInt_ToSignedInt_ToSmaller(a_from, a_to);
                }
            }
            else
            {
                if (std::is_signed<FromT>::value) // from signed to unsigned
                {
                    if (sizeof(FromT) <= sizeof(ToT))
                        return detail::CastValue_FromSignedInt_ToUnsignedInt_ToBigger(a_from, a_to);
                    else
                        return detail::CastValue_FromSignedInt_ToUnsignedInt_ToSmaller(a_from, a_to);
                }
                else // from unsigned to signed
                {
                    if (sizeof(FromT) < sizeof(ToT))
                        return detail::CastValue_FromUnsignedInt_ToSignedInt_ToBigger(a_from, a_to);
                    else
                        return detail::CastValue_FromUnsignedInt_ToSignedInt_ToSmaller(a_from, a_to);
                }
            }
        }
        else if (std::is_floating_point<ToT>::value)
        {
            return detail::CastValue_FromInt_ToFloat(a_from, a_to);
        }
    }
    else if (std::is_floating_point<FromT>::value)
    {
        if (std::is_integral<ToT>::value)
        {
            return detail::CastValue_FromFloat_ToInt(a_from, a_to);
        }
        else
        {
            if (sizeof(FromT) > sizeof(ToT))
                return detail::CastValue_FromDouble_ToFloat(a_from, a_to);
            else
                return detail::CastValue_FromFloat_ToDouble(a_from, a_to);
        }
    }
    else
        return CastResult::FailedCast;
}

#endif // _CAST_VALUE_2B4B4879_93AE_47A7_881F_FD6A4A5F6544_
