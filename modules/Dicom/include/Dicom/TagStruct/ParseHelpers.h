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

} // namespace dcm

#endif // _PARSE_HELPERS_3DCF27BD_A44D_4D60_9AE4_50B1B37456C5_
