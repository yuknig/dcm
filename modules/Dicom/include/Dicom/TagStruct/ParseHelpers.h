#ifndef _PARSE_HELPERS_3DCF27BD_A44D_4D60_9AE4_50B1B37456C5_
#define _PARSE_HELPERS_3DCF27BD_A44D_4D60_9AE4_50B1B37456C5_

#include <Dicom/TagStruct/TagNum.h>
#include <Dicom/TagStruct/Vr.h>
#include <Dicom/dict.h>
#include <Util/optional.h>

#include <cassert>
#include <cstdint>

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


} // namespace dcm

#endif // _PARSE_HELPERS_3DCF27BD_A44D_4D60_9AE4_50B1B37456C5_
