#include <Dicom/Parser.h>
#include <Dicom/Util.h>
#include <Dicom/dict.h>

#include <array>
#include <deque>
#include <algorithm>
#include <fstream>
#include <iostream>

namespace
{

static const uint32_t ItemTag           = 0xfffee000;
static const uint32_t ItemDelimiter     = 0xfffee00d;
static const uint32_t SequenceDelimiter = 0xfffee0dd;

}//namespace anonymous

namespace dcm
{

struct ReadParamDesc
{
    Group&      m_dest;
    StreamRead& m_stream;
    uint32_t    m_length;
    Tag         m_tag;
};

template <typename T>
bool readParamT(const ReadParamDesc& a_param, const VRType& a_vr)
{
    assert(!(a_param.m_length % sizeof(T)));
    const uint32_t valueCount = a_param.m_length / sizeof(T);
    a_param.m_dest.addTag<T>(a_param.m_tag, a_vr, valueCount, a_param.m_stream);
    return true;
}

template <typename T>
bool readParamArrayT(const ReadParamDesc& a_param, const VRType& a_vr)
{
    assert(!(a_param.m_length % sizeof(T)));
    const uint32_t valueCount = a_param.m_length / sizeof(T);
    a_param.m_dest.addTag<T>(a_param.m_tag, a_vr, valueCount, a_param.m_stream);
    return true;
}

bool readParamStr(const ReadParamDesc& a_param, const VRType& a_vr)
{
    a_param.m_dest.addTag<char>(a_param.m_tag, a_vr, a_param.m_length, a_param.m_stream);
    return true;
}

bool readDicomParams(Group& a_dest,
                     StreamRead& a_stream,
                     const TagDesc& a_tag_desc)
{
    typedef bool(*AddParamFunc)(const ReadParamDesc& a_param);

    static const std::array<std::pair<VRType, AddParamFunc>, 32> funcs =
    {{
        {
            VRType::Undefined, [](const ReadParamDesc& param) {
                return readParamArrayT<uint8_t>(param, VRType::Undefined);
            }
        },
        {
            VRType::AE, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::AE);
            }
        },
        {
            VRType::AS, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::AS);
            }
        },
        {
            VRType::AT, [](const ReadParamDesc& param) {
                return readParamT<uint32_t>(param, VRType::AT);
            }
        },
        {
            VRType::CS, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::CS);
            }
        },
        {
            VRType::DA, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::DA);
            }
        },
        {
            VRType::DS, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::DS);
            }
        },
        {
            VRType::DT, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::DS);
            }
        },
        {
            VRType::FL, [](const ReadParamDesc& param) {
                return readParamT<float>(param, VRType::FL);
            }
        },
        {
            VRType::FD, [](const ReadParamDesc& param) {
                return readParamT<double>(param, VRType::FD);
            }
        },
        {
            VRType::IS, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::IS);
            }
        },
        {
            VRType::LO, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::LO);
            }
        },
        {
            VRType::LT, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::LT);
            }
        },
        {
            VRType::OB, [](const ReadParamDesc& param) {
                return readParamArrayT<uint8_t>(param, VRType::OB);
            }
        },
        {
            VRType::OD, [](const ReadParamDesc& param) {
                return readParamArrayT<double>(param, VRType::OD);
            }
        },
        {
            VRType::OF, [](const ReadParamDesc& param) {
                return readParamArrayT<float>(param, VRType::OF);
            }
        },
        {
            VRType::OL, [](const ReadParamDesc& param) {
                return readParamArrayT<float>(param, VRType::OL);
            }
        },
        {
            VRType::OW, [](const ReadParamDesc& param) {
                return readParamArrayT<uint16_t>(param, VRType::OW);
            }
        },
        {
            VRType::PN, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::PN);
            }
        },
        {
            VRType::SH, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::SH);
            }
        },
        {
            VRType::SL, [](const ReadParamDesc& param) {
                return readParamT<int32_t>(param, VRType::SL);
            }
        },
        {
            VRType::SQ, [](const ReadParamDesc& param) {
                return false;
            }
        },
        {
            VRType::SS, [](const ReadParamDesc& param) {
                return readParamT<int16_t>(param, VRType::SS);
            }
        },
        {
            VRType::ST, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::ST);
            }
        },
        {
            VRType::TM, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::TM);
            }
        },
        {
            VRType::UC, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::UC);
            }
        },
        {
            VRType::UI, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::UI);
            }
        },
        {
            VRType::UL, [](const ReadParamDesc& param) {
                return readParamT<uint32_t>(param, VRType::UL);
            }
        },
        {
            VRType::UN, [](const ReadParamDesc& param) {
                return false;
            }
        },
        {
            VRType::UR, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::UR);
            }
        },
        {
            VRType::US, [](const ReadParamDesc& param) {
                return readParamT<uint16_t>(param, VRType::US);
            }
        },
        {
            VRType::UT, [](const ReadParamDesc& param) {
                return readParamStr(param, VRType::UT);
            }
        }
    }};

    assert(std::is_sorted(funcs.begin(), funcs.end(),
        [](const auto& lhs, const auto& rhs) {
        return lhs.first < rhs.first;
    }));

    const auto funcNum = static_cast<size_t>(a_tag_desc.m_vr);
    if (funcs.size() <= funcNum)
    {
        assert(false);
        return false;
    }

    const auto& func = funcs[funcNum];
    if (a_tag_desc.m_vr != func.first)
    {
        assert(false);
        return false;
    }
    const bool result = func.second({ a_dest, a_stream, a_tag_desc.m_valueLength, a_tag_desc.m_tag });
    return result;
}

bool getTag(const char* a_tagPtr, const size_t a_bytesLeft, uint32_t& a_tag)
{
    if (sizeof(a_tag) > a_bytesLeft)//4 bytes tag
        return false;

    const uint16_t groupNum = *ptrCast<uint16_t>(a_tagPtr);
    const uint16_t elementNum = *ptrCast<uint16_t>(a_tagPtr + 2);
    a_tag = (groupNum << 16) | elementNum;
    return true;
}

bool getTagAndVr(const bool a_explicitFile, const char* a_tagPtr, const size_t a_bytesLeft, uint32_t& a_tag, VRType& a_vr)
{
    uint32_t tag = 0;
    if (!getTag(a_tagPtr, a_bytesLeft, tag))
        return false;

    const bool isNoVrTag = (ItemTag == tag) || (ItemDelimiter == tag) || (SequenceDelimiter == tag);
    const bool isExplicitTag = isNoVrTag ? false : a_explicitFile;

    VRType vrType = VRType::Undefined;
    if (isExplicitTag)
    {
        if (6 > a_bytesLeft)
            return false;

        const uint16_t vr = *ptrCast<uint16_t>(a_tagPtr + 4);
        const VRCode vrCode = static_cast<VRCode>(vr);
        vrType = vrCodeToVrType(vrCode);
        assert(VRType::Undefined != vrType);
    }

    a_tag = tag;
    a_vr = vrType;
    return true;
}

bool getTagDesc(const bool a_explicitFile,
                const char* const a_ptr, const size_t a_bytesLeft,
                TagDesc& a_result)
{
    uint32_t tag = 0;
    VRType vr = VRType::Undefined;
    if (!getTagAndVr(a_explicitFile, a_ptr, a_bytesLeft, tag, vr))
        return false;

    uint8_t  valueOffset = 0;
    uint32_t valueLength = 0;
    const bool isExplicitVr = (VRType::Undefined != vr);
    if (isExplicitVr)
    {
        if (isSpecialVr(vr))
        {
            valueOffset = 12;
            valueLength = *ptrCast<uint32_t>(a_ptr + 8);
        }
        else
        {
            valueOffset = 8;
            valueLength = *ptrCast<uint16_t>(a_ptr + 6);
        }
    }
    else
    {
        valueOffset = 8;
        valueLength = *ptrCast<uint32_t>(a_ptr + 4);
    }

    const bool isNested = ((VRType::SQ == vr) || (ItemTag == tag));

    if (0xffffffff != valueLength)
    {
        a_result = TagDesc(tag, valueOffset + valueLength, valueLength, valueOffset, vr, isNested);
        return true;
    }

    const bool isTagSubSequnce = (ItemTag == tag);
    if ((!isTagSubSequnce) && (isExplicitVr) && (VRType::SQ != vr) && (VRType::OB != vr) && (VRType::OW != vr) && (VRType::UN != vr))
    {
        return false;
    }

    const uint32_t delim = isTagSubSequnce ? ItemDelimiter : SequenceDelimiter;
    valueLength = 0;
    TagDesc nestedLayout;//sequence item or subitem
    uint32_t curItemTag = 0;

    do
    {
        const size_t itemOffset = valueOffset + valueLength;
        if (!getTag(a_ptr + itemOffset, a_bytesLeft - itemOffset, curItemTag))
        {
            return false;
        }
        if (!getTagDesc(a_explicitFile, a_ptr + itemOffset, a_bytesLeft - itemOffset, nestedLayout))
        {
            return false;
        }
        valueLength += nestedLayout.m_fullLength;
    } while (delim != curItemTag);

    assert(nestedLayout.m_fullLength < valueLength);

    a_result = TagDesc(tag,
                       valueOffset + valueLength,
                       valueLength - nestedLayout.m_fullLength,//not count last delimiter
                       valueOffset,
                       vr,
                       true);
    return true;
}

bool parseDicomHeaderMemory(const char* a_ptr, const size_t a_size, size_t& a_dataStartOffset)
{
    const uint32_t magic = 0x4D434944;
    if ((!a_ptr) || (4 > a_size))
    {
        return false;
    }

    if (magic == *ptrCast<uint32_t>(a_ptr))
    {
        a_dataStartOffset = 4;
        return true;
    }

    const size_t MetaHeaderLength = 128U;
    if (MetaHeaderLength + 4 > a_size)
        return false;

    if (magic == *ptrCast<uint32_t>(a_ptr + MetaHeaderLength))
    {
        a_dataStartOffset = MetaHeaderLength + 4;
        return true;
    }

    return false;
}

struct TagMemoryGroup
{
public://functions
    TagMemoryGroup(const char* a_ptr, size_t a_memSize, const GroupPtr& a_parentGroup/*, const ParserConfig& a_config*/)
        : m_ptr(a_ptr)
        , m_memSize(a_memSize)
        , m_parentGroup(a_parentGroup)
        //, m_config(a_config)
    {}

public://data
    const char*  m_ptr;
    size_t       m_memSize;
    GroupPtr     m_parentGroup;
    //ParserConfig m_config;
};

Parser::Parser(StreamRead& a_stream)
{
    bool result = Parse(a_stream, m_root);
    if (!result)
    {
        assert(false);
        throw std::runtime_error("Failed to parse");
    }
}

bool Parser::Parse(StreamRead& a_stream, GroupPtr& a_root)
{
    if (a_stream.isEnd())
        return false;

    auto root = std::make_shared<Group>();
    ParserConfig config;
    std::deque<ParseGroupDesc> groupsToParse;
    {
        auto data_offset = ParseHeader(a_stream);
        if (!data_offset)
            return false;

        groupsToParse.emplace_back(ParseGroupDesc{ &a_stream, *data_offset, a_stream.size(), Tag(0xffff, 0xffff), config, root.get() });
    }

    while (!groupsToParse.empty())
    {
        ParseGroup(groupsToParse);
    }
    a_root.swap(root);
    return true;
}

std::optional<size_t> Parser::ParseHeader(StreamRead& a_stream)
{
    const uint32_t sign = 0x4D434944;
    if (sign == a_stream.readInPlace<uint32_t>())
        return 4u;

    const size_t SignatureOfsset = 128U;
    if (SignatureOfsset + 4 > a_stream.size())
    {
        return std::nullopt;
    }
    if (sign != a_stream.readInPlaceWithOffset<uint32_t>(SignatureOfsset))
        return std::nullopt;
    return SignatureOfsset + 4/*sizeof(uint32_t)*/;
}

bool Parser::ParseGroup(std::deque<ParseGroupDesc>& a_groupQueue)
{
    if (a_groupQueue.empty())
        return true;

    auto group = std::move(a_groupQueue.back());
    a_groupQueue.pop_back();

    auto tag_offset = group.m_stream_begin;

    size_t tagCount = 0;
    while (tag_offset < group.m_stream_end)
    {
        //std::cout << tag_offset << " " << tagCount << std::endl;
        group.m_stream->seek(tag_offset);
        const auto tag_desc = getTagDesc(*group.m_stream, group.m_config.IsExplicit());
        if (!tag_desc)
        {
            assert(false);
            return false;
        }

        const auto value_offset = tag_offset + tag_desc->m_valueOffset;

        if (VRType::SQ == tag_desc->m_vr)
        {
            Sequence sequence(tag_desc->m_tag);
            if (ParseSequence(group.m_stream, value_offset, value_offset + tag_desc->m_valueLength, group.m_config, sequence, a_groupQueue))
                group.m_dest_group->m_sequences.emplace(std::move(sequence));
            else
            {
                assert(false);
                return false;
            }
        }
        else
        {
            group.m_stream->seek(value_offset);
            if (!readDicomParams(*group.m_dest_group, *group.m_stream, *tag_desc))
            {
                assert(false);
                return false;
            }
        }
        tag_offset += tag_desc->m_fullLength;
        ++tagCount;
    }

    assert(tag_offset == group.m_stream_end);
    return true;
}

bool Parser::ParseSequence(StreamRead* a_stream, size_t a_begin_offset, size_t a_end_offset, const ParserConfig& a_config, Sequence& a_sequence, std::deque<ParseGroupDesc>& a_items)
{
    assert(a_stream);

    auto tag_offset = a_begin_offset;

    while (tag_offset < a_end_offset)
    {
        a_stream->seek(tag_offset);
        const auto tag_desc = getTagDesc(*a_stream, a_config.IsExplicit());
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

        auto item = std::make_shared<Group>();
        a_items.emplace_back(ParseGroupDesc{ a_stream,
                                             value_offset,
                                             value_offset + tag_desc->m_valueLength,
                                             /*maxTax=*/0xffffffff,
                                             a_config,
                                             item.get() } );
        a_sequence.emplace_back(std::move(item));

        tag_offset += tag_desc->m_fullLength;
    }
    assert(tag_offset == a_end_offset);
    return true;
}

std::optional<TagDesc> Parser::getTagDesc(StreamRead& a_stream, const bool a_explicitFile)
{
    const auto tag_vr = getTagAndVr(a_stream, a_explicitFile);
    if (!tag_vr)
        return std::nullopt;

    const auto tag = tag_vr->first;
    const auto vr  = tag_vr->second;
    uint8_t  valueOffset = 0;
    uint32_t valueLength = 0;
    const bool isNoVrTag = (tag == ItemTag) || (tag == ItemDelimiter) || (tag == SequenceDelimiter);
    const bool isExplicitVr = isNoVrTag ? false : a_explicitFile;
    if (isExplicitVr)
    {
        if (isSpecialVr(vr))
        {
            valueOffset = 12;
            valueLength = a_stream.readInPlaceWithOffset<uint32_t>(8);
        }
        else
        {
            valueOffset = 8;
            valueLength = a_stream.readInPlaceWithOffset<uint16_t>(6);
        }
    }
    else
    {
        valueOffset = 8;
        valueLength = a_stream.readInPlaceWithOffset<uint32_t>(4);
    }

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

    AutoRewind rewind(a_stream);
    a_stream.advance(valueOffset);

    do
    {
        nestedLayout = getTagDesc(a_stream, a_explicitFile);
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

std::optional<std::pair<Tag, VRType>> Parser::getTagAndVr(const StreamRead& a_stream, const bool a_explicitFile)
{
    static const uint32_t ItemTag           = 0xfffee000;
    static const uint32_t ItemDelimiter     = 0xfffee00d;
    static const uint32_t SequenceDelimiter = 0xfffee0dd;

    const auto tag = getTag(a_stream);
    if (!tag)
    {
        return std::nullopt;
    }

    const bool isNoVrTag = (*tag == ItemTag) || (*tag == ItemDelimiter) || (*tag == SequenceDelimiter);
    const bool isExplicitTag = isNoVrTag ? false : a_explicitFile;

    VRType vrType = VRType::Undefined;
    if (isExplicitTag)
    {
        if (6 > a_stream.sizeToEnd())
        {
            return std::nullopt;
        }

        const auto vr = a_stream.readInPlaceWithOffset<uint16_t>(4);
        const VRCode vrCode = static_cast<VRCode>(vr);
        vrType = vrCodeToVrType(vrCode);
        assert(VRType::Undefined != vrType);
    }
    return std::make_pair(*tag, vrType);
}

std::optional<Tag> Parser::getTag(const StreamRead& a_stream)
{
    if (sizeof(Tag) > a_stream.sizeToEnd()) // 4 bytes tag
    {
        return std::nullopt;
    }

    const auto data = a_stream.readInPlace<uint32_t>(); // swapped words
    return Tag(data & 0xffff, data >> 16);
}

GroupPtr Parser::root() const
{
    return m_root;
}

} // namespace dcm
