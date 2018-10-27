#ifndef _CAST_VALUE_2B4B4879_93AE_47A7_881F_FD6A4A5F6544_
#define _CAST_VALUE_2B4B4879_93AE_47A7_881F_FD6A4A5F6544_

enum class CastResult
{
    FailedCast = 0,
    Ok_WithCast,
    Ok_NoCast
};

//template <typename FromT, typename ToT>
//CastResult castValue(const FromT& a_from, ToT& a_to);


#include "CastValue.inl"

#endif // _CAST_VALUE_2B4B4879_93AE_47A7_881F_FD6A4A5F6544_
