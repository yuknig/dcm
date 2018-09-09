#ifndef _TAG_CCD09E06_6C85_49F8_AE49_308141CDC268_
#define _TAG_CCD09E06_6C85_49F8_AE49_308141CDC268_

#include <cstdint>

namespace dcm
{

struct TagNum
{
public://functions
    TagNum(const uint16_t a_groupNum, const uint16_t a_elementNum);
    bool operator< (const TagNum& a_rhs) const;
    bool operator==(const TagNum& a_rhs) const;
    bool operator!=(const TagNum& a_rhs) const;

public://data
    uint16_t m_elementNum;
    uint16_t m_groupNum;
};


union Tag
{
public://functions
    Tag(const uint16_t a_group, const uint16_t a_element);
    Tag(const uint32_t a_tag);
    bool operator< (const Tag& a_rhs) const;
    bool operator==(const Tag& a_rhs) const;
    bool operator!=(const Tag& a_rhs) const;

    bool operator==(const uint32_t a_rhs) const;
    bool operator!=(const uint32_t a_rhs) const;

public://data
    TagNum    m_num;
    uint32_t  m_solid;
};

static_assert(sizeof(Tag) == 4, "Wrong packing of Tag");

}//namespace dcm

#endif //_TAG_CCD09E06_6C85_49F8_AE49_308141CDC268_
