#include <Dicom/TagValues.h>

namespace dcm
{


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
    if (m_noValues.hasTag(a_tag, m_valuesSorted[ValueBit::NoValue]))
        return true;
    if (m_singleValues.hasTag(a_tag, m_valuesSorted[ValueBit::Single]))
        return true;
    if (m_shortArrayValues.hasTag(a_tag, m_valuesSorted[ValueBit::ShortArray]))
        return true;
    if (m_longArrayValues.hasTag(a_tag, m_valuesSorted[ValueBit::LongArray]))
        return true;
    if (m_sequences.hasTag(a_tag, m_valuesSorted[ValueBit::Sequence]))
        return true;
    return false;
}

bool Group::hasValue(const Tag a_tag) const
{
    if (m_noValues.hasTag(a_tag, m_valuesSorted[ValueBit::NoValue]))
        return false;
    if (m_singleValues.hasTag(a_tag, m_valuesSorted[ValueBit::Single]))
        return true;
    if (m_shortArrayValues.hasTag(a_tag, m_valuesSorted[ValueBit::ShortArray]))
        return true;
    if (m_longArrayValues.hasTag(a_tag, m_valuesSorted[ValueBit::LongArray]))
        return true;
    if (m_sequences.hasTag(a_tag, m_valuesSorted[ValueBit::Sequence]))
        return true;
    return false;
}

void Group::sort(bool a_recursive)
{
    m_noValues.sort();
    m_singleValues.sort();
    m_shortArrayValues.sort();
    m_longArrayValues.sort();
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
