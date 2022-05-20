#ifndef _TAG_DESC_F126BFDF_6042_4968_9E77_795C87A8F298_
#define _TAG_DESC_F126BFDF_6042_4968_9E77_795C87A8F298_
#include <Dicom/TagStruct/Vr.h>
#include <Dicom/TagStruct/TagNum.h>

#include <cstdint>

namespace dcm
{

struct TagDesc
{
    TagDesc() noexcept
        : TagDesc(0u, 0u, 0u, 0u, VRType::Undefined, false)
    {}

    TagDesc(Tag a_tag, uint32_t a_fullLength, uint32_t a_valueLength, uint8_t a_valueOffset, VRType a_vr, bool a_nested)
        : m_tag(a_tag)
        , m_fullLength(a_fullLength)
        , m_valueLength(a_valueLength)
        , m_valueOffset(a_valueOffset)
        , m_nested(a_nested)
        , m_vr(a_vr)
    {}

public://data
    Tag      m_tag;
    uint32_t m_fullLength;
    uint32_t m_valueLength;
    uint8_t  m_valueOffset;
    bool     m_nested;
    VRType   m_vr;
    //TODO: add flag is length defined
};

} // namespace dcm

#endif // _TAG_DESC_F126BFDF_6042_4968_9E77_795C87A8F298_
