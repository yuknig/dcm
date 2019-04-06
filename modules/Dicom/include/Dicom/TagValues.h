#ifndef _TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_
#define _TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_

#include <Dicom/TagStruct/SingleValue.h>
#include <Dicom/TagStruct/ArrayValue.h>
#include <Dicom/TagStruct/SortedTagList.h>
#include <Dicom/Util.h>
#include <Util/Stream.h>
#include <Util/MVector.h>

#include <vector>
#include <bitset>
#include <memory>
#include <algorithm>
#include <cstring>
#include <cassert>
#include <type_traits>

namespace dcm
{

namespace detail
{
template <typename T>
GetValueResult GetFromNoValue(T& /*a_value*/)
{
    return GetValueResult::FailedCast;
}

template <>
inline GetValueResult GetFromNoValue(std::string& a_value)
{
    a_value = std::string();
    return GetValueResult::Ok_NoCast;
}

}

inline bool GetValueSucceeded(const GetValueResult a_result)
{
    return (GetValueResult::Ok_WithCast <= a_result);
}


class SingleValues: public SortedList_Tag_Value<SingleValue, true>
{
    using Base = SortedList_Tag_Value<SingleValue, true>;
public://functions
    SingleValues()
    {}

    template <typename T>
    void add(bool& a_is_sorted, const Tag a_tag, const VRType a_vr, const T& a_value)
    {
        static_assert(std::is_same_v<T, uint32_t> ||
                      std::is_same_v<T, int32_t>  ||
                      std::is_same_v<T, uint16_t> ||
                      std::is_same_v<T, int16_t>  ||
                      std::is_same_v<T, uint8_t>  ||
                      std::is_same_v<T, uint8_t>  ||
                      std::is_same_v<T, char>     ||
                      std::is_same_v<T, float>, "Wrong Type");

        Base::emplace(a_is_sorted, a_tag, a_vr, a_value);
    }
};


class Sequence;

class Group
{
public:
    Group()
    {
        m_valuesSorted.set();
    }

    //TODO: try to make const?
    template <typename T>
    void addTag(const Tag a_tag, const VRType a_vr, const uint32_t a_valueElements, StreamRead& a_stream)///add MultiValue, SingleValue or NoValue
    {
        if (1 < a_valueElements)
        {
            const size_t size_in_bytes = a_valueElements * sizeof(T);
            
            if (size_in_bytes <= ShortArrayValue::getLengthMax())
            {
                bool sorted = m_valuesSorted[ValueBit::ShortArray];
                m_shortArrayValues.emplace(sorted, a_tag, a_vr, size_in_bytes, a_stream);
                m_valuesSorted[ValueBit::ShortArray] = sorted;
            }
            else
            {
                bool sorted = m_valuesSorted[ValueBit::LongArray];
                m_longArrayValues.emplace(sorted, a_tag, a_vr, size_in_bytes, a_stream);
                m_valuesSorted[ValueBit::LongArray] = sorted;
            }
        }
        else if (1 == a_valueElements)
        {
            bool sorted = m_valuesSorted[ValueBit::Single];
            m_singleValues.add(sorted, a_tag, a_vr, a_stream.read<T>());
            m_valuesSorted[ValueBit::Single] = sorted;
        }
        else
        {
            bool sorted = m_valuesSorted[ValueBit::NoValue];
            m_noValues.emplace(sorted, a_tag, a_vr);
            m_valuesSorted[ValueBit::NoValue] = sorted;
        }
    }

    //TODO: specialization for double

    void addSequence(const Tag a_tag, std::vector<std::shared_ptr<Group>> a_groups);

    bool hasTag(const Tag a_tag) const;

    bool hasValue(const Tag a_tag) const;

    void sort(bool a_recursive);

    template <typename T>
    GetValueResult getTag(const Tag a_tag, T& a_value) const
    {
        if (m_noValues.hasTag(a_tag, m_valuesSorted[ValueBit::NoValue]))
            return detail::GetFromNoValue<T>(a_value);

        GetValueResult res;
        res = m_singleValues.get(a_tag, a_value, m_valuesSorted[ValueBit::Single]);
        if (res != GetValueResult::DoesNotExists)
            return res;

        res = m_shortArrayValues.get(a_tag, a_value, m_valuesSorted[ValueBit::ShortArray]);
        if (res != GetValueResult::DoesNotExists)
            return res;

        res = m_longArrayValues.get(a_tag, a_value, m_valuesSorted[ValueBit::LongArray]);
        if (res != GetValueResult::DoesNotExists)
            return res;

        if (m_sequences.hasTag(a_tag, m_valuesSorted[ValueBit::Sequence]))
            return GetValueResult::FailedCast;

        return GetValueResult::DoesNotExists;
    }

public://types
    typedef SortedList_Tag_Value<Tag_NoValue, true>      NoValues;
    typedef SortedList_Tag_Value<ShortArrayValue, true>  ShortArrayValues;
    typedef SortedList_Tag_Value<LongArrayValue, true>   LongArrayValues;
    typedef SortedList_Tag_ValuePtr<Sequence>            SequenceValues;

    enum ValueBit
    {
        NoValue = 0,
        Single,
        ShortArray,
        LongArray,
        Sequence,
        MaxValue = Sequence
    };

public://data
    NoValues          m_noValues;
    SingleValues      m_singleValues;
    ShortArrayValues  m_shortArrayValues;
    LongArrayValues   m_longArrayValues;
    SequenceValues    m_sequences;
    std::bitset<ValueBit::MaxValue + 1> m_valuesSorted;
};

typedef std::shared_ptr<Group> GroupPtr;

class Sequence: public Tag_ValuePtr<std::vector<std::shared_ptr<Group>>>
{
public://types
    typedef Group Item;
    typedef std::shared_ptr<Item> ItemPtr;

public://functions
    Sequence(const Tag& a_tag, std::vector<ItemPtr> a_items)
        : Base(a_tag, VRType::SQ, std::move(a_items))
    {}

private://types
    typedef Tag_ValuePtr<std::vector<std::shared_ptr<Group>>> Base;
};

}//namespace dcm

#include <Dicom/TagValues.inl>

#endif //_TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_
