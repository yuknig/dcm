#ifndef _TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_
#define _TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_

#include <Dicom/Vr.h>
#include <Dicom/Tag.h>
#include <Dicom/Util.h>
#include <Util/Stream.h>
#include <Util/MVector.h>

#include <vector>
#include <bitset>
#include <memory>
#include <algorithm>
#include <cstring>
#include <cassert>

namespace dcm
{

class Tag_NoValue
{
public://functions
    Tag_NoValue(const Tag a_tag, const VRType a_vr);

    Tag tag() const;
    VRType vr() const;
    bool operator<(const Tag_NoValue& a_rhs) const;

protected://data
    Tag    m_tag;
    VRType m_vr;
};

template <typename T>
class Tag_Value: public Tag_NoValue
{
public://functions
    template <typename... ArgsT>
    Tag_Value(const Tag a_tag, const VRType a_vr, ArgsT&&... a_constuctArgs)
        : Tag_NoValue(a_tag, a_vr)
        , m_value(std::forward<ArgsT>(a_constuctArgs)...)
    {}

    const T& value() const
    {
        return m_value;
    }

protected://data
    T m_value;
};

template <typename T>
class Tag_ValuePtr: public Tag_NoValue
{
public://functions
    template <typename... ArgsT>
    Tag_ValuePtr(const Tag a_tag, const VRType a_vr, ArgsT&&... a_constuctArgs)
        : Tag_NoValue(a_tag, a_vr)
        , m_valuePtr(std::make_unique<T>(std::forward<ArgsT>(a_constuctArgs)...))
    {}

    const T& value() const
    {
        assert(m_valuePtr);
        return *m_valuePtr;
    }

protected://data
    std::unique_ptr<T> m_valuePtr;
};


enum GetValueResult
{
    DoesNotExists = 0,
    FailedCast    = 1,
    Ok_WithCast   = 2,
    Ok_NoCast     = 3
};

inline bool GetValueSucceeded(const GetValueResult a_result)
{
    return (GetValueResult::Ok_WithCast <= a_result);
}

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
    char     m_char;
    float    m_float;
};
static_assert(sizeof(SingleValueUnion) == 4, "ValueUnion wrong layout");


class SingleValue: public Tag_Value<SingleValueUnion> //TODO: rename to Tag_SingleValue
{
public://functions
    template <typename T>
    explicit SingleValue(const Tag a_tag, const VRType a_vr, const T a_value)
        : Tag_Value<SingleValueUnion>(a_tag, a_vr, a_value)
    {
        //TODO: asserts that checks matching VR/value_type
    }

    template <typename T>
    GetValueResult get(T& a_result) const;

    template<typename T>
    GetValueResult get(std::basic_string<T>& a_result) const;
};
static_assert(sizeof(SingleValue) > 8, "dcm::SingleValue: unexpected memory layout.");

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
public://functions
    SingleValues()
    {}

    template <typename T>
    void add(bool& a_is_sorted, const Tag a_tag, const VRType a_vr, const T& a_value)
    {
        static_assert(!std::is_same<T, double>::value, "Specialization for double should be used");
        Base::emplace(a_is_sorted, a_tag, a_vr, a_value);
    }

private://data
    typedef SortedList_Tag_Value<SingleValue, true> Base;

private://data
    MVector<double, SizeType, true> m_doubles; // store double values separately
};

class StringValue: public Tag_NoValue
{
public://functions
    StringValue(const Tag a_tag, const VRType a_vr, const uint32_t a_length, const StreamRead& a_stream)
        : Tag_NoValue(a_tag, a_vr)
        , m_value(a_length, '\0')
    {
        size_t bytes_read = a_stream.readToMemInPlace(&m_value[0], a_length);
        if (bytes_read != a_length)
        {
            assert(false);
        }
    }

    const std::string& get() const
    {
        return m_value;
    }

private://data
    std::string m_value;
};

template <size_t LengthMax>
class ShortStringValueT: public Tag_NoValue
{
public://functions
    ShortStringValueT(const Tag a_tag, const VRType a_vr, const uint32_t a_length, const StreamRead& a_stream)
        : Tag_NoValue(a_tag, a_vr)
    {
        assert(LengthMax > a_length);

        a_stream.readToMemInPlace(m_value.data(), std::min<size_t>(a_length, LengthMax));
        for (size_t i = a_length; i < m_value.size(); ++i)
            m_value[i] = '\0';
    }

    size_t length() const
    {
        return strlen(m_value.data());
    }

    static size_t getLengthMax()
    {
        return LengthMax;
    }

private://data
    std::array<char, LengthMax + 1/*null terminator*/> m_value;
};

using ShortStringValue = ShortStringValueT<31>;

//template<typename T>
//inline std::vector<uint8_t> readVectorFromString(const StreamRead& a_stream, const uint32_t a_size)
//{
//    std::vector<uint8_t> result(a_size);
//    assert(false);
//    return result;
//}

class MultiValue: public Tag_ValuePtr<MVector<uint8_t, size_t, true>>
{
public://function
    MultiValue(const Tag a_tag, const VRType a_vr, const uint32_t a_size_in_bytes, const StreamRead& a_stream)
        : Base(a_tag, a_vr)
    {
        m_valuePtr->resize(a_size_in_bytes);
        auto bytes_read = a_stream.readToMemInPlace(m_valuePtr->data(), a_size_in_bytes);
        if (bytes_read != a_size_in_bytes)
        {
            assert(false);
        }
    }

private://types
    typedef Tag_ValuePtr<MVector<uint8_t, size_t, true>> Base;
};

//class MultiValues:public SortedList<MultiValue>
//{
//public:
//    MultiValues() {}
//};

class Sequence;

class Group
{
public:
    Group()
    {
        m_valuesSorted.set();
    }

    template <typename T>
    void addTag(const Tag a_tag, const VRType a_vr, const uint32_t a_valueElements, StreamRead& a_stream)///add MultiValue, SingleValue or NoValue
    {
        if (1 < a_valueElements)
        {
            if (isStringVr(a_vr))
            {
                if (ShortStringValue::getLengthMax() >= a_valueElements)
                {
                    bool sorted = m_valuesSorted[ValueBit::ShortString];
                    m_shortStringValues.emplace(sorted, a_tag, a_vr, a_valueElements, a_stream);
                    m_valuesSorted[ValueBit::ShortString] = sorted;
                }
                else
                {
                    bool sorted = m_valuesSorted[ValueBit::String];
                    m_stringValues.emplace(sorted, a_tag, a_vr, a_valueElements, a_stream);
                    m_valuesSorted[ValueBit::String] = sorted;
                }
            }
            else
            {
                bool sorted = m_valuesSorted[ValueBit::Multi];
                m_multiValues.emplace(sorted, a_tag, a_vr, a_valueElements, a_stream);
                m_valuesSorted[ValueBit::Multi] = sorted;
            }
        }
        else if (1 == a_valueElements)
        {
            addTag(a_tag, a_vr, a_stream.read<T>());
        }
        else
        {
            bool sorted = m_valuesSorted[ValueBit::No];
            m_noValues.emplace(sorted, a_tag, a_vr); //for has_tag() only
            m_valuesSorted[ValueBit::No] = sorted;
        }
    }

    void addSequence(const Tag a_tag, std::vector<std::shared_ptr<Group>> a_groups);

    bool hasTag(const Tag a_tag) const;

    void sort(bool a_recursive);

    template <typename T>
    bool getTag(const Tag a_tag, T& a_value) const
    {
        GetValueResult res = DoesNotExists;

        if (m_noValues.hasTag(a_tag, m_valuesSorted[ValueBit::No]))
            return GetValueResult::FailedCast;

        if (GetValueSucceeded(m_singleValues.get(a_tag, a_value, m_valuesSorted[ValueBit::No])))
            return true;
        else if (GetValueResult::DoesNotExists != res)
            return false;

        if (GetValueSucceeded(m_multiValues.get(a_tag, a_value, m_valuesSorted[ValueBit::Multi])))
            return true;
        else if (GetValueResult::DoesNotExists != res)
            return false;

        return false;
    }

protected://functions
    template <typename T>
    void addTag(const Tag a_tag, const VRType a_vr, const T a_value)/// Add SingleValue
    {
        static_assert(std::is_arithmetic<T>::value, "T should be arithmetic");

        bool sorted = m_valuesSorted[ValueBit::Single];
        m_singleValues.add(sorted, a_tag, a_vr, a_value);
        m_valuesSorted[ValueBit::Single] = sorted;
    }

public://types
    typedef SortedList_Tag_Value<Tag_NoValue, true>      NoValues;
    typedef SortedList_Tag_ValuePtr<MultiValue>          MultiValues;
    typedef SortedList_Tag_Value<StringValue, false>     StringValues;
    typedef SortedList_Tag_Value<ShortStringValue, true> ShortStringValues; // TODO: place before ordinary string
    typedef SortedList_Tag_ValuePtr<Sequence>            SequenceValues;

    enum ValueBit
    {
        No = 0,
        Single,
        String,
        ShortString,
        Multi,
        Sequence,
        MaxValue = Sequence
    };

public://data
    NoValues          m_noValues;
    SingleValues      m_singleValues;
    StringValues      m_stringValues;
    ShortStringValues m_shortStringValues;
    MultiValues       m_multiValues;
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
