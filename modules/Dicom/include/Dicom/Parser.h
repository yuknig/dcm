#ifndef _PARSER_C6B68E8E_AFAF_499F_95AE_249B47CE139F_
#define _PARSER_C6B68E8E_AFAF_499F_95AE_249B47CE139F_

#include <Dicom/Vr.h>
#include <Dicom/Tag.h>
#include <Dicom/TagValues.h>

#include <deque>
#include <vector>
#include <memory>
#include <assert.h>
#include <Util/optional.h>
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

struct ParserConfig
{
public:
    ParserConfig()
        : m_explicit(true)
    {}

    bool IsExplicit() const 
    {
        return m_explicit;
    }

private://data
    bool m_explicit;
};

struct ParseGroupDesc
{
    StreamRead*  m_stream;
    size_t       m_stream_begin;
    size_t       m_stream_end;
    Tag          m_max_tag;
    ParserConfig m_config;
    Group*       m_dest_group;
};

class Parser
{
public://functions
    Parser(StreamRead& a_stream);

    GroupPtr root() const;

protected://functions
    // move to testable ParserHelper
    static bool Parse(StreamRead& a_stream, GroupPtr& a_root);
    static std::optional<size_t> ParseHeader(StreamRead& a_stream);
    static bool ParseGroup(std::deque<ParseGroupDesc>& a_groupQueue);
    static bool ParseSequence(StreamRead* a_stream, size_t a_begin_offset, size_t a_end_offset, const ParserConfig& a_config, Sequence& a_sequence, std::deque<ParseGroupDesc>& a_items);

    static std::optional<TagDesc> getTagDesc(StreamRead& a_stream, const bool a_explicitFile);
    static std::optional<std::pair<Tag, VRType>> getTagAndVr(const StreamRead& a_stream, const bool a_explicitFile);
    static std::optional<Tag> getTag(const StreamRead& a_stream);

private://data
    GroupPtr     m_root;
    ParserConfig m_config;
};

}//namespace dcm

#endif //_PARSER_C6B68E8E_AFAF_499F_95AE_249B47CE139F_
