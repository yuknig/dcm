#ifndef _TAG_VALUE_BASIC_E9402C2E_177D_4ADC_A4E3_B4B0DAEE26AB_
#define _TAG_VALUE_BASIC_E9402C2E_177D_4ADC_A4E3_B4B0DAEE26AB_

#include <Dicom/TagStruct/TagNum.h>
#include <Dicom/TagStruct/Vr.h>
#include <Util/CastValue.h>
#include <cassert>
#include <memory>

namespace dcm
{

enum class GetValueResult {
    DoesNotExists = 0, // fix to DoesNotExist
    FailedCast,
    Ok_WithCast,
    Ok_NoCast
};

inline GetValueResult CastResult_To_GetValueResult(const CastResult& a_from) {
    switch (a_from) {
        case CastResult::Ok_NoCast:
            return GetValueResult::Ok_NoCast;
        case CastResult::Ok_CastLoseless:
        case CastResult::Ok_CastLossy:
            return GetValueResult::Ok_WithCast;
        case CastResult::FailedCast:
        default:
            return GetValueResult::FailedCast;
    }
}


} // namespace dcm

#endif // _TAG_VALUE_BASIC_E9402C2E_177D_4ADC_A4E3_B4B0DAEE26AB_
