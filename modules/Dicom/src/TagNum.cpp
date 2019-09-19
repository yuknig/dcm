#include <Dicom/TagStruct/TagNum.h>

#include <cassert>

namespace dcm
{

////////////////////////////////////////
//TagNum
////////////////////////////////////////

TagNum::TagNum(const uint16_t a_groupNum, const uint16_t a_elementNum)
    : m_elementNum(a_elementNum)
    , m_groupNum(a_groupNum)
{}

bool TagNum::operator<(const TagNum& a_rhs) const {
    if (m_groupNum < a_rhs.m_groupNum)
        return true;

    if (m_groupNum > a_rhs.m_groupNum)
        return false;

    return (m_elementNum < a_rhs.m_elementNum);
}

bool TagNum::operator==(const TagNum& a_rhs) const {
    return (this->m_groupNum   == a_rhs.m_groupNum &&
            this->m_elementNum == a_rhs.m_elementNum);
}

bool TagNum::operator!=(const TagNum& a_rhs) const {
    return !operator==(a_rhs);
}

////////////////////////////////////////
//Tag
////////////////////////////////////////

Tag::Tag(const uint16_t a_group, const uint16_t a_element)
    : m_num(a_group, a_element)
{
    assert(((a_group << 16) | a_element) == m_solid);
}

Tag::Tag(const uint32_t a_tag)
    : m_solid(a_tag)
{
    assert(m_solid >> 16 == m_num.m_groupNum);
    assert((m_solid & 0xFFFF) == m_num.m_elementNum);
}

bool Tag::operator<(const Tag& a_rhs) const {
    const bool result = (m_solid < a_rhs.m_solid);
    assert(result == m_num.operator<(a_rhs.m_num));
    return result;
}

bool Tag::operator==(const Tag& a_rhs) const {
    const bool result = (this->m_solid == a_rhs.m_solid);
    assert(result == m_num.operator==(a_rhs.m_num));
    return result;
}

bool Tag::operator!=(const Tag& a_rhs) const {
    const bool result = (this->m_solid != a_rhs.m_solid);
    assert(result == m_num.operator!=(a_rhs.m_num));
    return result;
}

bool Tag::operator==(const uint32_t a_rhs) const {
    const bool result = (this->m_solid == a_rhs);
    return result;
}

bool Tag::operator!=(const uint32_t a_rhs) const {
    return !operator==(a_rhs);
}

}//namespace dcm
