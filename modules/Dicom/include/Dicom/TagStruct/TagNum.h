#ifndef _TAG_NUM_099D67A6_E804_480C_9E89_E32F81AE356B_
#define _TAG_NUM_099D67A6_E804_480C_9E89_E32F81AE356B_

#include <cstdint>

namespace dcm
{

struct TagNum {
public:
    TagNum(const uint16_t a_groupNum, const uint16_t a_elementNum);
    bool operator< (const TagNum& a_rhs) const;
    bool operator==(const TagNum& a_rhs) const;
    bool operator!=(const TagNum& a_rhs) const;

public:
    uint16_t m_elementNum;
    uint16_t m_groupNum;
};

union Tag {
public:
    Tag(const uint16_t a_group, const uint16_t a_element);
    Tag(const uint32_t a_tag);
    bool operator< (const Tag& a_rhs) const;
    bool operator==(const Tag& a_rhs) const;
    bool operator!=(const Tag& a_rhs) const;

    bool operator==(const uint32_t a_rhs) const;
    bool operator!=(const uint32_t a_rhs) const;

public:
    TagNum    m_num;
    uint32_t  m_solid;
};

} // namespace dcm

#endif // _TAG_NUM_099D67A6_E804_480C_9E89_E32F81AE356B_
