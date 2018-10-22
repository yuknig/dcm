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

void Group::addSequence(const Tag a_tag, std::vector<std::shared_ptr<Group>> a_groups)
{
    bool sorted = m_valuesSorted[ValueBit::Sequence];
    m_sequences.emplace(sorted, a_tag, std::move(a_groups));
    m_valuesSorted[ValueBit::Sequence] = sorted;
}

bool Group::hasTag(const Tag a_tag) const
{
    if (m_noValues.hasTag(a_tag, m_valuesSorted[ValueBit::No]))
        return true;
    if (m_singleValues.hasTag(a_tag, m_valuesSorted[ValueBit::Single]))
        return true;
    if (m_stringValues.hasTag(a_tag, m_valuesSorted[ValueBit::String]))
        return true;
    if (m_shortStringValues.hasTag(a_tag, m_valuesSorted[ValueBit::ShortString]))
        return true;
    if (m_multiValues.hasTag(a_tag, m_valuesSorted[ValueBit::Multi]))
        return true;
    if (m_sequences.hasTag(a_tag, m_valuesSorted[ValueBit::Sequence]))
        return true;
    return false;
}

bool Group::hasValue(const Tag a_tag) const
{
    if (m_noValues.hasTag(a_tag, m_valuesSorted[ValueBit::No]))
        return false;
    if (m_singleValues.hasTag(a_tag, m_valuesSorted[ValueBit::Single]))
        return true;
    if (m_stringValues.hasTag(a_tag, m_valuesSorted[ValueBit::String]))
        return true;
    if (m_shortStringValues.hasTag(a_tag, m_valuesSorted[ValueBit::ShortString]))
        return true;
    if (m_multiValues.hasTag(a_tag, m_valuesSorted[ValueBit::Multi]))
        return true;
    if (m_sequences.hasTag(a_tag, m_valuesSorted[ValueBit::Sequence]))
        return true;
    return false;
}

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