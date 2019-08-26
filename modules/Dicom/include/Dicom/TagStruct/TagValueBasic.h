#ifndef _TAG_VALUE_BASIC_E9402C2E_177D_4ADC_A4E3_B4B0DAEE26AB_
#define _TAG_VALUE_BASIC_E9402C2E_177D_4ADC_A4E3_B4B0DAEE26AB_

#include <Dicom/TagStruct/TagNum.h>
#include <Dicom/TagStruct/Vr.h>
#include <Util/CastValue.h>
#include <cassert>
#include <memory>

namespace dcm
{

class Tag_NoValue {
public:
    Tag_NoValue(const Tag a_tag, const VRType a_vr);

    Tag tag() const;
    VRType vr() const;
    bool operator<(const Tag_NoValue& a_rhs) const;

protected:
    Tag    m_tag;
    VRType m_vr;
};

template <typename T>
class Tag_Value: public Tag_NoValue {
public:
    template <typename... ArgsT>
    Tag_Value(const Tag a_tag, const VRType a_vr, ArgsT&&... a_constuct_args)
        : Tag_NoValue(a_tag, a_vr)
        , m_value(std::forward<ArgsT>(a_constuct_args)...)
    {}

    const T& value() const {
        return m_value;
    }

protected:
    T m_value;
};

template <typename T>
class Tag_ValuePtr: public Tag_NoValue {
public:
    template <typename... ArgsT>
    Tag_ValuePtr(const Tag a_tag, const VRType a_vr, ArgsT&&... a_constuct_args)
        : Tag_NoValue(a_tag, a_vr)
        , m_value_ptr(std::make_shared<T>(std::forward<ArgsT>(a_constuct_args)...))
    {}

    const T& value() const {
        assert(m_value_ptr);
        return *m_value_ptr;
    }

    std::shared_ptr<const T> valueSharedPtr() const {
        return m_value_ptr;
    }

protected:
    std::shared_ptr<T> m_value_ptr;
};

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
