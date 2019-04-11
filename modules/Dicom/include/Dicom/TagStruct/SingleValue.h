#ifndef _SINGLE_VALUE_48AD71D4_AD7F_4CF6_933F_2FE8E60243BC_
#define _SINGLE_VALUE_48AD71D4_AD7F_4CF6_933F_2FE8E60243BC_

//#include <cstdint>
#include <Dicom/TagStruct/TagValueBasic.h>

namespace dcm
{

union SingleValueUnion {
public:
    explicit SingleValueUnion(uint32_t a_value)
        : m_uint32(a_value)
    {}

    explicit SingleValueUnion(int32_t a_value)
        : m_sint32(a_value)
    {}

    explicit SingleValueUnion(uint16_t a_value)
        : m_uint32(a_value)
    {}

    explicit SingleValueUnion(int16_t a_value)
        : m_sint32(a_value)
    {}

    explicit SingleValueUnion(uint8_t a_value)
        : m_uint32(a_value)
    {}

    explicit SingleValueUnion(int8_t a_value)
        : m_sint32(a_value)
    {}

    explicit SingleValueUnion(char a_value)
        : m_sint32(a_value)
    {}

    explicit SingleValueUnion(float a_value)
        : m_float(a_value)
    {}

public://data
    uint32_t m_uint32;
    int32_t  m_sint32;
    uint16_t m_uint16;
    int16_t  m_sint16;
    uint8_t  m_uint8;
    uint8_t  m_sint8;
    char     m_char; //TODO: delete?
    float    m_float;
};
static_assert(sizeof(SingleValueUnion) == 4, "ValueUnion wrong layout");

//TODO: rename to Tag_SingleValue
class SingleValue : public Tag_Value<SingleValueUnion> {
public:
    template <typename T>
    explicit SingleValue(const Tag a_tag, const VRType a_vr, const T a_value)
        : Tag_Value<SingleValueUnion>(a_tag, a_vr, a_value)
    {
        assert(a_vr == VRType::SL ||
            a_vr == VRType::UL ||
            a_vr == VRType::OL ||
            a_vr == VRType::SS ||
            a_vr == VRType::US ||
            a_vr == VRType::OW ||
            a_vr == VRType::FL ||
            a_vr == VRType::OF ||
            a_vr == VRType::OB);
    }

    template <typename T>
    GetValueResult get(T& a_result) const {
        const CastResult cast_result = castSingleValue(this->m_value, this->m_vr, a_result);
        return CastResult_To_GetValueResult(cast_result);
    }
private:
    template <typename ToT>
    static CastResult castSingleValue(const SingleValueUnion& a_from, const VRType a_vr, ToT& a_result) {
        switch (a_vr) {
        case VRType::SL:
            return CastValue<int32_t>(a_from.m_sint32, a_result);
        case VRType::UL:
        case VRType::OL:
            return CastValue<uint32_t>(a_from.m_uint32, a_result);
        case VRType::SS:
            return CastValue<int16_t>(a_from.m_sint16, a_result);
        case VRType::US:
        case VRType::OW:
            return CastValue<uint16_t>(a_from.m_uint16, a_result);
        case VRType::OB:
            return CastValue<uint8_t>(a_from.m_uint8, a_result);
        case VRType::FL:
        case VRType::OF:
            return CastValue<float>(a_from.m_float, a_result);
        default:
            assert(false);
            return CastResult::FailedCast;
        }
    }
};
static_assert(sizeof(SingleValue) > 8, "dcm::SingleValue: unexpected memory layout.");

} // namespace dcm

#endif // _SINGLE_VALUE_48AD71D4_AD7F_4CF6_933F_2FE8E60243BC_
