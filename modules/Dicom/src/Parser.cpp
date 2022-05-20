#include <Dicom/Parser.h>
#include <Dicom/TagStruct/ParseHelpers.h>
#include <Dicom/Util.h>
#include <Dicom/dict.h>
#include <Util/string_util.h>

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
    a_param.m_dest.AddTag<T>(a_param.m_tag, a_vr, valueCount, a_param.m_stream);
    return true;
}

template <typename T>
bool readParamArrayT(const ReadParamDesc& a_param, const VRType& a_vr)
{
    assert(!(a_param.m_length % sizeof(T)));
    const uint32_t valueCount = a_param.m_length / sizeof(T);
    a_param.m_dest.AddTag<T>(a_param.m_tag, a_vr, valueCount, a_param.m_stream);
    return true;
}

bool readParamStr(const ReadParamDesc& a_param, const VRType& a_vr)
{
    a_param.m_dest.AddTag<char>(a_param.m_tag, a_vr, a_param.m_length, a_param.m_stream);
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
            VRType::Undefined, [](const ReadParamDesc& param) {//TODO: move args to capture?
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
    TagMemoryGroup(const char* a_ptr, size_t a_memSize, const std::shared_ptr<Group>& a_parentGroup/*, const ParserConfig& a_config*/)
        : m_ptr(a_ptr)
        , m_memSize(a_memSize)
        , m_parentGroup(a_parentGroup)
        //, m_config(a_config)
    {}

public://data
    const char*  m_ptr;
    size_t       m_memSize;
    std::shared_ptr<Group> m_parentGroup;
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
        auto data_offset = GetFirstTagOffset(a_stream);
        if (!data_offset)
            return false;

        std::deque<ParseGroupDesc> groups_to_parse;
        ParserConfig def_config;
        groups_to_parse.emplace_back(ParseGroupDesc{ *data_offset, a_stream.size(), def_config, root.get() });
        auto parsed_to = PickAndParseGroup(a_stream, groups_to_parse, std::min(a_max_tag, Tag(0x3, 0x0))); // parse group 0x0002
        if (!parsed_to)
        {
            return false;
        }
        start_offset = *parsed_to;
    }

    ParserConfig config = {};
    {
        std::string transfer_syntax;
        if (Succeeded(root->GetTag(dcm::dict::TransferSyntaxUID, transfer_syntax)))
        {
            transfer_syntax = string_util::TrimRight(transfer_syntax, " \0");
            if (transfer_syntax == "1.2.840.10008.1.2") //TODO: make named consexpr
                config.m_explicit = false;
        }
    }

    std::deque<ParseGroupDesc> groups_to_parse;
    groups_to_parse.emplace_back(ParseGroupDesc{ start_offset, a_stream.size() - start_offset, config, root.get() });

    while (!groups_to_parse.empty())
    {
        auto parsed_to_offset = PickAndParseGroup(a_stream, groups_to_parse, a_max_tag);
        if (!parsed_to_offset.has_value())
        {
            assert(false);
        }
    }
    root->OnLoadingFinished();

    a_root.swap(root);
    return true;
}

std::shared_ptr<Group> Parser::root() const
{
    return m_root;
}

////////////////////////////////////////
// ParseHelper
////////////////////////////////////////

namespace ParseHelper
{


} // namespace ParseHelper

} // namespace dcm
