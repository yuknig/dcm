#ifndef _PARSER_C6B68E8E_AFAF_499F_95AE_249B47CE139F_
#define _PARSER_C6B68E8E_AFAF_499F_95AE_249B47CE139F_

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
public://data
    ParserConfig()
        : m_explicit(true)
        , m_little_endian(true)
    {}

    ParserConfig(bool a_explicit, bool a_little_endian)
        : m_explicit(a_explicit)
        , m_little_endian(a_little_endian)
    {}

    bool IsExplicit() const 
    {
        return m_explicit;
    }

public://data
    bool m_explicit;
    bool m_little_endian;
};

struct ParseGroupDesc
{
    size_t       m_stream_begin;
    size_t       m_stream_end;
    ParserConfig m_config;
    Group*       m_dest_group;
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
    static std::optional<size_t> ParseHeader(StreamRead& a_stream);

private://data
    GroupPtr     m_root;
    ParserConfig m_config;
};

namespace ParseHelper
{

std::optional<size_t> PickAndParseGroup(StreamRead& a_stream, std::deque<ParseGroupDesc>& a_groupQueue, const Tag a_max_tag);
bool ParseSequence(StreamRead& a_stream, const size_t a_begin_offset, const size_t a_end_offset, const ParserConfig& a_config, std::vector<std::unique_ptr<Group>>& a_items, std::deque<ParseGroupDesc>& a_items_to_parse);
std::optional<TagDesc> GetTagDesc(StreamRead& a_stream, const bool a_explicitFile);
std::optional<std::pair<Tag, VRType>> GetTagAndVr(StreamRead& a_stream, const bool a_explicitFile);
std::optional<Tag> GetTag(StreamRead& a_stream);

} // namespace ParseHelper

}//namespace dcm

#endif //_PARSER_C6B68E8E_AFAF_499F_95AE_249B47CE139F_
