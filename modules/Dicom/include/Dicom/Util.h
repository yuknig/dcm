#ifndef _UTIL_2B78AC42_69B0_4C67_9BFB_51750AD342B3_
#define _UTIL_2B78AC42_69B0_4C67_9BFB_51750AD342B3_

#include <stddef.h>
#include <array>

namespace dcm
{

template <typename To>
To* ptrCast(void* const a_ptr);

template <typename To>
const To* ptrCast(const void* const a_ptr);

}//namespace dcm

#include <Dicom/Util.inl>

#endif //_UTIL_2B78AC42_69B0_4C67_9BFB_51750AD342B3_