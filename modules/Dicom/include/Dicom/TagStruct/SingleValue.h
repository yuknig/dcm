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

} // namespace dcm

#endif // _SINGLE_VALUE_48AD71D4_AD7F_4CF6_933F_2FE8E60243BC_
