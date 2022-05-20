#ifndef _PARSER_C6B68E8E_AFAF_499F_95AE_249B47CE139F_
#define _PARSER_C6B68E8E_AFAF_499F_95AE_249B47CE139F_

#include <Dicom/TagValues.h>
#include <Dicom/TagStruct/ParseConfig.h>

#include <memory>
#include <assert.h>
#include <Util/Stream.h>

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

class Parser
{
public://functions
    using GroupPtr = std::shared_ptr<Group>;

    Parser(StreamRead& a_stream, const Tag& a_max_tag = 0xffffffff);

    GroupPtr root() const;

protected://functions
    // move to testable ParserHelper
    static bool Parse(StreamRead& a_stream, GroupPtr& a_root, const Tag& a_max_tag);

private://data
    GroupPtr     m_root;
    ParserConfig m_config;
};

}//namespace dcm

#endif //_PARSER_C6B68E8E_AFAF_499F_95AE_249B47CE139F_
