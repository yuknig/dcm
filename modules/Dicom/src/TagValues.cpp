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
    return (m_noValues.hasTag(a_tag, m_valuesSorted[ValueBit::NoValue]) ||
            m_singleValues.hasTag(a_tag, m_valuesSorted[ValueBit::Single]) ||
            m_shortArrayValues.hasTag(a_tag, m_valuesSorted[ValueBit::ShortArray]) ||
            m_longArrayValues.hasTag(a_tag, m_valuesSorted[ValueBit::LongArray]) ||
            m_sequences.hasTag(a_tag, m_valuesSorted[ValueBit::Sequence]));
}

bool Group::hasValue(const Tag a_tag) const
{
    if (m_noValues.hasTag(a_tag, m_valuesSorted[ValueBit::NoValue]))
        return false;

    return (m_singleValues.hasTag(a_tag, m_valuesSorted[ValueBit::Single]) ||
            m_shortArrayValues.hasTag(a_tag, m_valuesSorted[ValueBit::ShortArray]) ||
            m_longArrayValues.hasTag(a_tag, m_valuesSorted[ValueBit::LongArray]) ||
            m_sequences.hasTag(a_tag, m_valuesSorted[ValueBit::Sequence]));

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
            const auto& items = sequence.value();
            for (const auto& item : items)
            {
                item->sort(true);
            }
        }
    }
}

}//namespace dcm
