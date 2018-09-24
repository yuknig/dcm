#include <Dicom/TagValues.h>

namespace dcm
{

////////////////////////////////////////
//Tag_NoValue
////////////////////////////////////////

Tag_NoValue::Tag_NoValue(const Tag a_tag, const VRType a_vr)
    : m_tag(a_tag)
    , m_vr(a_vr)
{}

Tag Tag_NoValue::tag() const
{
    return m_tag;
}

VRType Tag_NoValue::vr() const
{
    return m_vr;
}

bool Tag_NoValue::operator<(const Tag_NoValue& a_rhs) const
{
    return std::tie(m_tag, m_vr) < std::tie(a_rhs.m_tag, a_rhs.m_vr);

}

////////////////////////////////////////
//Group
////////////////////////////////////////

void Group::sort(bool a_recursive)
{
    m_noValues.sort();
    m_singleValues.sort();
    m_stringValues.sort();
    m_shortStringValues.sort();
    m_multiValues.sort();
    m_sequences.sort();
    m_valuesSorted.set();

    if (a_recursive)
    {
        for (const auto& sequence : m_sequences)
        {
            const auto& groups = sequence.value();
            for (const auto& groupPtr : groups)
            {
                groupPtr->sort(true);
            }
        }
    }
}

}//namespace dcm
