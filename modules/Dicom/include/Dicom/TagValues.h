#ifndef _TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_
#define _TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_

#include <Dicom/Vr.h>
#include <Dicom/TagStruct/TagNum.h>
#include <Dicom/TagStruct/TagValueBasic.h>
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

union SingleValueUnion
{
public://constructors
    explicit SingleValueUnion(uint32_t a_value)
        : m_uint32(a_value)
    {}

    explicit SingleValueUnion(int32_t a_value)
        : m_sint32(a_value)
    {}

    explicit SingleValueUnion(uint16_t a_value)
        : m_uint32(a_value)
    {}

    explicit SingleValueUnion(int16_t a_value)
        : m_sint32(a_value)
    {}

    explicit SingleValueUnion(uint8_t a_value)
        : m_uint32(a_value)
    {}

    explicit SingleValueUnion(int8_t a_value)
        : m_sint32(a_value)
    {}

    explicit SingleValueUnion(char a_value)
        : m_sint32(a_value)
    {}

    explicit SingleValueUnion(float a_value)
        : m_float(a_value)
    {}

public://data
    uint32_t m_uint32;
    int32_t  m_sint32;
    uint16_t m_uint16;
    int16_t  m_sint16;
    uint8_t  m_uint8;
    uint8_t  m_sint8;
    char     m_char; //TODO: delete?
    float    m_float;
};
static_assert(sizeof(SingleValueUnion) == 4, "ValueUnion wrong layout");


enum GetValueResult
{
    DoesNotExists = 0,
    FailedCast = 1,
    Ok_WithCast = 2,
    Ok_NoCast = 3
};

inline bool GetValueSucceeded(const GetValueResult a_result)
{
    return (GetValueResult::Ok_WithCast <= a_result);
}

class SingleValue: public Tag_Value<SingleValueUnion> //TODO: rename to Tag_SingleValue
{
public://functions
    template <typename T>
    explicit SingleValue(const Tag a_tag, const VRType a_vr, const T a_value)
        : Tag_Value<SingleValueUnion>(a_tag, a_vr, a_value)
    {
        assert(a_vr == VRType::SL ||
               a_vr == VRType::UL ||
               a_vr == VRType::OL ||
               a_vr == VRType::SS ||
               a_vr == VRType::US ||
               a_vr == VRType::OW ||
               a_vr == VRType::FL ||
               a_vr == VRType::OF ||
               a_vr == VRType::OB);
    }

    template <typename T>
    GetValueResult get(T& a_result) const;

    //template<typename CharT>
    //GetValueResult get(std::basic_string<CharT>& a_result) const;
};
static_assert(sizeof(SingleValue) > 8, "dcm::SingleValue: unexpected memory layout.");


template <size_t Size>
class ShortArrayValueT : public Tag_Value<std::array<char, Size>>
{
    using Base = Tag_Value<std::array<char, Size>>;
public:
    ShortArrayValueT(const Tag a_tag, const VRType a_vr, const size_t a_length_in_bytes, const StreamRead& a_stream)
        : Base(a_tag, a_vr)
    {
        static_assert(Size < 256, "Too big array size");

        const auto length_in_bytes = std::min(a_length_in_bytes, getLengthMax());
        assert(length_in_bytes == a_length_in_bytes);

        const auto bytes_read = a_stream.readToMemInPlace(Base::m_value.data(), length_in_bytes);
        if (bytes_read != length_in_bytes)
        {
            assert(false);
        }

        for (size_t i = bytes_read; i < getLengthMax(); ++i)
            Base::m_value[i] = '\0';
        Base::m_value[Size - 1] = static_cast<unsigned char>(a_length_in_bytes);
    }

    size_t getLengthInBytes() const
    {
        const size_t result = static_cast<unsigned char>(Base::m_value[Size - 1]);
        assert(result < getLengthMax());
        return result;
    }

    static constexpr size_t getLengthMax()
    {
        return Size - 1;
    }

    template <typename T>
    GetValueResult get(T& /*a_value*/) const
    {
        return GetValueResult::FailedCast;
    }
};

using ShortArrayValue = ShortArrayValueT<32>;

class LongArrayValue: public Tag_ValuePtr<std::vector<char>>
{
    using Base = Tag_ValuePtr<std::vector<char>>;

public:
    LongArrayValue(const Tag a_tag, const VRType a_vr, const size_t a_length_in_bytes, const StreamRead& a_stream)
        : Base(a_tag, a_vr, a_length_in_bytes)
    {
        const auto bytes_read = a_stream.readToMemInPlace(m_value_ptr->data(), a_length_in_bytes);
        if (bytes_read != a_length_in_bytes)
        {
            assert(false);
        }
    }

    size_t getLengthInBytes() const
    {
        return m_value_ptr->size();
    }

    template <typename T>
    GetValueResult get(T& /*a_value*/) const
    {
        return GetValueResult::FailedCast;
    }
};


template <typename T, bool Realloc>
class SortedList_Tag_Base
{
public://functions
    SortedList_Tag_Base()
    {}

    template <typename... ArgsT>
    void emplace(bool& a_is_sorted, ArgsT&&... a_args);

    T* begin();
    T* end();

    bool hasTag(const Tag a_tag, bool a_is_sorted) const;

    void sort();

protected://functions
    const T* getTagPtr(const Tag a_tag, bool a_is_sorted) const;

    bool areLastTagsSorted() const;

protected://types
#if INTPTR_MAX == INT32_MAX
    using SizeType = uint16_t;
#else
    using SizeType = uint32_t;
#endif

protected://data
    //std::vector<T> m_list;
    MVector<T, SizeType, Realloc> m_list;
};

template <typename T, bool Realloc>
class SortedList_Tag_Value: public SortedList_Tag_Base<T, Realloc>
{
public://functions
    template <typename V>
    GetValueResult get(const Tag a_tag, V& a_result, bool a_is_sorted) const;

private://types
    typedef SortedList_Tag_Base<T, Realloc> Base;
};

template <typename T>
class SortedList_Tag_ValuePtr: public SortedList_Tag_Base<T, false>
{
public://functions
    template <typename V>
    GetValueResult get(const Tag a_tag, V& a_result, bool a_is_sorted) const;

private://types
    typedef SortedList_Tag_Base<T, false> Base;
};


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
            return details::GetFromNoValue<T>(a_value);

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
