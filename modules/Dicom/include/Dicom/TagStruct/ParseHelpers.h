#ifndef _PARSE_HELPERS_3DCF27BD_A44D_4D60_9AE4_50B1B37456C5_
#define _PARSE_HELPERS_3DCF27BD_A44D_4D60_9AE4_50B1B37456C5_

#include <Dicom/TagStruct/TagDesc.h>
#include <Dicom/TagStruct/TagNum.h>
#include <Dicom/TagStruct/Vr.h>
#include <Dicom/dict.h>
#include <Util/optional.h>

#include <cassert>
#include <cstdint>
#include <deque>
#include <memory>
#include <vector>

namespace dcm
{

template <typename StreamT>
std::optional<Tag> GetTag(StreamT& a_stream)
{
    if (sizeof(Tag) > a_stream.sizeToEnd()) // 4 bytes tag
        return std::nullopt;

    const auto data = a_stream.read<uint32_t>(); // swapped words
    return Tag(data & 0xffff, data >> 16);
}

template <typename StreamT>
std::optional<std::pair<Tag, VRType>> GetTagAndVr(StreamT& a_stream, const bool a_explicit_file)
{
    constexpr uint32_t ItemTag           = 0xfffee000;
    constexpr uint32_t ItemDelimiter     = 0xfffee00d;
    constexpr uint32_t SequenceDelimiter = 0xfffee0dd;

    const auto tag = GetTag(a_stream);
    if (!tag)
        return std::nullopt;

    const bool is_no_vr_tag = (*tag == ItemTag) || (*tag == ItemDelimiter) || (*tag == SequenceDelimiter);
    const bool is_explicit_tag = is_no_vr_tag ? false : a_explicit_file;

    VRType vr_type = VRType::Undefined;
    if (is_explicit_tag)
    {
        if (6 > a_stream.sizeToEnd())
            return std::nullopt;

        a_stream.advance(4);
        const auto vr = a_stream.read<uint16_t>();
        const VRCode vr_code = static_cast<VRCode>(vr);
        vr_type = vrCodeToVrType(vr_code);
        assert(VRType::Undefined != vr_type);
    }
    else
        vr_type = dict::GetTagVR(*tag);

    return std::make_pair(*tag, vr_type);
}

template <typename StreamT>
std::optional<TagDesc> GetTagDesc(StreamT& a_stream, const bool a_explicitFile)
{
    // stream should be positioned to the start of the tag
    const auto tag_offset = a_stream.pos();
    const auto tag_vr = GetTagAndVr(a_stream, a_explicitFile);
    if (!tag_vr)
        return std::nullopt;

    const auto& tag = tag_vr->first;
    const auto& vr  = tag_vr->second;

    uint8_t valueLengthOffset = 0;
    uint8_t valueLengthSize = 0;
    uint8_t valueOffset = 0;
    const bool isNoVrTag = (tag == ItemTag) || (tag == ItemDelimiter) || (tag == SequenceDelimiter);
    const bool isExplicitVr = isNoVrTag ? false : a_explicitFile;
    if (isExplicitVr)
    {
        if (isSpecialVr(vr))
        {
            valueLengthOffset = 8;
            valueLengthSize = 4;
            valueOffset = 12;
        }
        else
        {
            valueLengthOffset = 6;
            valueLengthSize = 2;
            valueOffset = 8;
        }
    }
    else
    {
        valueLengthOffset = 4;
        valueLengthSize = 4;
        valueOffset = 8;
    }

    uint32_t valueLength = 0;
    a_stream.seek(tag_offset + valueLengthOffset);
    a_stream.read(&valueLength, valueLengthSize);

    const bool isNested = ((VRType::SQ == vr) || (tag == ItemTag));
    static const uint32_t undefLength = 0xffffffff;
    if (undefLength != valueLength)
        return TagDesc(tag, valueOffset + valueLength, valueLength, valueOffset, vr, isNested);

    const bool isTagSubSequnce = (tag == ItemTag);
    if ((!isTagSubSequnce) && (isExplicitVr) && (VRType::SQ != vr) && (VRType::OB != vr) && (VRType::OW != vr) && (VRType::UN != vr))
        return std::nullopt;

    const uint32_t delim = isTagSubSequnce ? ItemDelimiter : SequenceDelimiter;
    std::optional<TagDesc> nestedLayout;
    valueLength = 0;

    a_stream.seek(tag_offset + valueOffset);
    do
    {
        a_stream.seek(tag_offset + valueOffset + valueLength);
        nestedLayout = GetTagDesc(a_stream, a_explicitFile);
        if (!nestedLayout)
        {
            return std::nullopt;
        }

        if (nestedLayout->m_tag == delim)
        {
            return TagDesc(tag,
                           valueOffset + valueLength + nestedLayout->m_fullLength,
                           valueLength,
                           valueOffset,
                           vr,
                           true);
        }

        valueLength += nestedLayout->m_fullLength;
        a_stream.advance(nestedLayout->m_fullLength);
    } while (!a_stream.isEnd());

    return std::nullopt;
}

template <typename StreamT>
std::optional<size_t> GetFirstTagOffset(StreamT& a_stream)
{
    // returns offset of first tag in file
    constexpr uint32_t sign = 0x4D434944; // DICM
    if (sign == a_stream.read<uint32_t>())
        return 4u;

    const size_t SignatureOffset = 128U;
    if (SignatureOffset + 4 > a_stream.size())
    {
        return std::nullopt;
    }

    a_stream.seek(SignatureOffset);
    if (sign != a_stream.read<uint32_t>())
        return std::nullopt;
    return SignatureOffset + 4/*sizeof(uint32_t)*/;
}

struct ParseGroupDesc
{
    size_t       m_stream_begin;
    size_t       m_stream_end;
    ParserConfig m_config;
    Group* m_dest_group;
};

template <typename StreamT>
bool ParseSequence(StreamT& a_stream, const size_t a_begin_offset, const size_t a_end_offset, const ParserConfig& a_config, std::vector<std::unique_ptr<Group>>& a_items, std::deque<ParseGroupDesc>& a_items_to_parse)
{
    auto tag_offset = a_begin_offset;

    std::vector<std::unique_ptr<Group>> items;

    while (tag_offset < a_end_offset)
    {
        a_stream.seek(tag_offset);
        const auto tag_desc = GetTagDesc(a_stream, a_config.IsExplicit());
        if (!tag_desc)
        {
            assert(false);
            return false;
        }
        if (ItemTag != tag_desc->m_tag.m_solid)
        {
            assert(false);
        }

        const auto value_offset = tag_offset + tag_desc->m_valueOffset;

        auto item = std::make_unique<Group>();
        a_items_to_parse.emplace_back(ParseGroupDesc{ value_offset,
                                             value_offset + tag_desc->m_valueLength,
                                             a_config,
                                             item.get() });
        items.emplace_back(std::move(item));

        tag_offset += tag_desc->m_fullLength;
    }
    assert(tag_offset == a_end_offset);

    a_items.swap(items);
    return true;
}

// parse 1 group from queue
template <typename StreamT>
std::optional<size_t> PickAndParseGroup(StreamT& a_stream, std::deque<ParseGroupDesc>& a_groupQueue, const Tag a_max_tag)
{
    if (a_groupQueue.empty())
        return std::nullopt;

    auto group = std::move(a_groupQueue.back());
    a_groupQueue.pop_back();

    auto tag_offset = group.m_stream_begin;
    Tag tagNum(0);

    while (tag_offset < group.m_stream_end)
    {
        a_stream.seek(tag_offset);
        const auto tag_desc = GetTagDesc(a_stream, group.m_config.IsExplicit());
        if (!tag_desc)
        {
            assert(false);
            return false;
        }

        tagNum = tag_desc->m_tag;
        if (a_max_tag < tagNum)
            break;
        const auto value_offset = tag_offset + tag_desc->m_valueOffset;

        if (VRType::SQ == tag_desc->m_vr)
        {
            std::vector<std::unique_ptr<Group>> sequence_items;
            if (ParseSequence(a_stream, value_offset, value_offset + tag_desc->m_valueLength, group.m_config, sequence_items, a_groupQueue))
                group.m_dest_group->AddSequence(tagNum, std::move(sequence_items));
            else
            {
                assert(false);
                return false;
            }
        }
        else
        {
            a_stream.seek(value_offset);
            if (!readDicomParams(*group.m_dest_group, a_stream, *tag_desc))
            {
                assert(false);
                return false;
            }
        }
        tag_offset += tag_desc->m_fullLength;
    }

    //assert(tag_offset == group.m_stream_end);
    return tag_offset;
}

} // namespace dcm

#endif // _PARSE_HELPERS_3DCF27BD_A44D_4D60_9AE4_50B1B37456C5_
