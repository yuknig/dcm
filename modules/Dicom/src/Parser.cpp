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

Parser::Parser(StreamRead& a_stream, const Tag& a_max_tag)
{
    bool result = Parse(a_stream, m_root, a_max_tag);
    if (!result)
    {
        assert(false);
        throw std::runtime_error("Failed to parse");
    }
}

bool Parser::Parse(StreamRead& a_stream, GroupPtr& a_root, const Tag& a_max_tag)
{
    if (a_stream.isEnd())
        return false;

    auto root = std::make_shared<Group>();
    size_t start_offset = 0;

    {
        auto data_offset = ParseHeader(a_stream);
        if (!data_offset)
            return false;

        std::deque<ParseGroupDesc> groups_to_parse;
        ParserConfig def_config;
        groups_to_parse.emplace_back(ParseGroupDesc{ *data_offset, a_stream.size(), def_config, root.get() });
        auto parsed_to = ParseHelper::PickAndParseGroup(a_stream, groups_to_parse, std::min(a_max_tag, Tag(0x3, 0x0))); // parse group 0x0002
        if (!parsed_to)
        {
            return false;
        }
        start_offset = *parsed_to;
    }

    ParserConfig config;
    {
        std::string transfer_syntax;
        if (GetValueSucceeded(root->getTag(dcm::TransferSyntaxUID, transfer_syntax)))
        {
            if (transfer_syntax == "1.2.840.10008.1.2")
                config.m_explicit = false;
        }
    }

    std::deque<ParseGroupDesc> groups_to_parse;
    groups_to_parse.emplace_back(ParseGroupDesc{ start_offset, a_stream.size() - start_offset, config, root.get() });

    while (!groups_to_parse.empty())
    {
        auto parsed_to_offset = ParseHelper::PickAndParseGroup(a_stream, groups_to_parse, a_max_tag);
        if (!parsed_to_offset.has_value())
        {
            assert(false);
        }
    }
    root->sort(true);

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

GroupPtr Parser::root() const
{
    return m_root;
}

////////////////////////////////////////
// ParseHelper
////////////////////////////////////////

namespace ParseHelper
{

// parse 1 group from queue
std::optional<size_t> PickAndParseGroup(StreamRead& a_stream, std::deque<ParseGroupDesc>& a_groupQueue, const Tag a_max_tag)
{
    if (a_groupQueue.empty())
        return std::nullopt;

    auto group = std::move(a_groupQueue.back());
    a_groupQueue.pop_back();

    auto tag_offset = group.m_stream_begin;
    Tag tagNum(0);

    while (tag_offset < group.m_stream_end && tagNum < a_max_tag)
    {
        a_stream.seek(tag_offset);
        const auto tag_desc = ParseHelper::getTagDesc(a_stream, group.m_config.IsExplicit());
        if (!tag_desc)
        {
            assert(false);
            return false;
        }

        tagNum = tag_desc->m_tag;
        const auto value_offset = tag_offset + tag_desc->m_valueOffset;

        if (VRType::SQ == tag_desc->m_vr)
        {
            std::vector<GroupPtr> sequence_groups;
            if (ParseSequence(a_stream, value_offset, value_offset + tag_desc->m_valueLength, group.m_config, sequence_groups, a_groupQueue))
                group.m_dest_group->addSequence(tagNum, sequence_groups);
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

bool ParseSequence(StreamRead& a_stream, const size_t a_begin_offset, const size_t a_end_offset, const ParserConfig& a_config, std::vector<GroupPtr>& a_groups, std::deque<ParseGroupDesc>& a_items)
{
    auto tag_offset = a_begin_offset;

    std::vector<GroupPtr> groups;

    while (tag_offset < a_end_offset)
    {
        a_stream.seek(tag_offset);
        const auto tag_desc = ParseHelper::getTagDesc(a_stream, a_config.IsExplicit());
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
        a_items.emplace_back(ParseGroupDesc{ value_offset,
                                             value_offset + tag_desc->m_valueLength,
                                             a_config,
                                             item.get() });
        groups.emplace_back(item);

        tag_offset += tag_desc->m_fullLength;
    }
    assert(tag_offset == a_end_offset);

    a_groups.swap(groups);
    return true;
}

std::optional<TagDesc> getTagDesc(StreamRead& a_stream, const bool a_explicitFile)
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

std::optional<std::pair<Tag, VRType>> getTagAndVr(const StreamRead& a_stream, const bool a_explicitFile)
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

std::optional<Tag> getTag(const StreamRead& a_stream)
{
    if (sizeof(Tag) > a_stream.sizeToEnd()) // 4 bytes tag
    {
        return std::nullopt;
    }

    const auto data = a_stream.readInPlace<uint32_t>(); // swapped words
    return Tag(data & 0xffff, data >> 16);
}

} // namespace ParseHelper

} // namespace dcm
