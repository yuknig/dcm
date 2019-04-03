#ifndef _TAG_VALUE_BASIC_E9402C2E_177D_4ADC_A4E3_B4B0DAEE26AB_
#define _TAG_VALUE_BASIC_E9402C2E_177D_4ADC_A4E3_B4B0DAEE26AB_

#include <Dicom/TagStruct/TagNum.h>
#include <Dicom/Vr.h>
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
    Tag_Value(const Tag a_tag, const VRType a_vr, ArgsT&&... a_constuctArgs)
        : Tag_NoValue(a_tag, a_vr)
        , m_value(std::forward<ArgsT>(a_constuctArgs)...)
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
    Tag_ValuePtr(const Tag a_tag, const VRType a_vr, ArgsT&&... a_constuctArgs)
        : Tag_NoValue(a_tag, a_vr)
        , m_valuePtr(std::make_unique<T>(std::forward<ArgsT>(a_constuctArgs)...))
    {}

    const T& value() const {
        assert(m_valuePtr);
        return *m_valuePtr;
    }

protected:
    std::unique_ptr<T> m_valuePtr;
};

} // namespace dcm

#endif // _TAG_VALUE_BASIC_E9402C2E_177D_4ADC_A4E3_B4B0DAEE26AB_
