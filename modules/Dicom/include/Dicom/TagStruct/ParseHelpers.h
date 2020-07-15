#ifndef _PARSE_HELPERS_3DCF27BD_A44D_4D60_9AE4_50B1B37456C5_
#define _PARSE_HELPERS_3DCF27BD_A44D_4D60_9AE4_50B1B37456C5_

#include <Dicom/TagStruct/TagNum.h>
#include <Util/optional.h>

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

} // namespace dcm

#endif // _PARSE_HELPERS_3DCF27BD_A44D_4D60_9AE4_50B1B37456C5_
