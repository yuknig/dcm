#ifndef _TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_
#define _TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_

#include <Dicom/TagStruct/SingleValue.h>
#include <Dicom/TagStruct/ArrayValue.h>
#include <Dicom/TagStruct/SortedTagList.h>
#include <Dicom/Util.h>
#include <Util/Stream.h>
#include <Util/MVector.h>
#include "Util/optional.h"

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

inline bool Succeeded(const GetValueResult a_result)
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

    template <typename T>
    std::shared_ptr<std::vector<T>> getTagVector(const Tag a_tag) const
    {
        if (m_noValues.hasTag(a_tag, m_valuesSorted[ValueBit::NoValue]))
            return std::shared_ptr<std::vector<T>>();

        // single value
        {
            T value = {};
            auto getValRes = m_singleValues.get(a_tag, value, m_valuesSorted[ValueBit::Single]);
            if (Succeeded(getValRes))
                return std::make_shared<std::vector<T>>(1, std::move(value));
        }

        // TODO: finish

        return GetValueResult::DoesNotExists;
    }

public://types
    typedef SortedList_Tag_Value<Tag_NoValue, true>      NoValues;
    typedef SortedList_Tag_Value<ShortArrayValue, true>  ShortArrayValues;
    typedef SortedList_Tag_ValuePtr<LongArrayValue>      LongArrayValues;
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



class TagValue {
public:
    // no value ctor
    explicit TagValue(const Tag& a_tag, const VRType& a_vr)
        : m_tag(a_tag)
        , m_value(0u)
        , m_storage(ValueStorage::NoValue)
        , m_vr(a_vr)
    {}

    // single small value ctor (<= 4bytes)
    explicit TagValue(const Tag& a_tag, const VRType& a_vr, const SingleValueUnion& a_value)
        : m_tag(a_tag)
        , m_value(a_value)
        , m_storage(ValueStorage::Inplace)
        , m_vr(a_vr)
    {}

    // big or multiple value ctor( > 4 bytes)
    explicit TagValue(const Tag& a_tag, const VRType& a_vr, const uint32_t a_offset_in_buf)
        : m_tag(a_tag)
        , m_value(a_offset_in_buf)
        , m_storage(ValueStorage::Buffer)
        , m_vr(a_vr)
    {}

    const Tag& tag() const {
        return m_tag;
    }

private:
    enum class ValueStorage: uint16_t {
        NoValue = 0,
        Inplace,
        Buffer
    };

private:
    Tag m_tag;
    SingleValueUnion m_value;
    ValueStorage m_storage;
    VRType m_vr;
};

class TagVector {
public:
    template <typename... ArgsT>
    void emplace(ArgsT&&... a_args) {
        m_tags.emplace_back(std::forward<ArgsT>(a_args)...);
        if (m_sorted && m_tags.size() > 1) // check that vector is still sorted
            m_sorted = m_tags[m_tags.size() - 2].tag() < m_tags.back().tag()
    }

    bool HasTag(const Tag& a_tag) const {
        return (nullptr != GetTagPtr(a_tag));
    }

    void Sort() {
        std::sort(m_tags.begin(), m_tags.end(),
            [](const TagValue& lhs, const TagValue& rhs) {
            return lhs.tag() < rhs.tag();
        });
    }

private:
    const TagValue* GetTagPtr(const Tag& a_tag) const {
        return m_sorted ? GetTagPtr_Sorted(a_tag)
                        : GetTagPtr_Unsorted(a_tag);
    }

    const TagValue* GetTagPtr_Sorted(const Tag& a_tag) const {
        assert(m_sorted);
        auto find_it = std::lower_bound(m_tags.begin(), m_tags.end(), a_tag,
            [](const TagValue& lhs, const Tag& rhs) {
                return lhs.tag() < rhs;
        });
        if (m_tags.end() == find_it || a_tag != find_it->tag())
            return nullptr;
        return &*find_it;
    }

    const TagValue* GetTagPtr_Unsorted(const Tag& a_tag) const {
        auto find_it = std::find_if(m_tags.begin(), m_tags.end(),
            [a_tag](const TagValue& tag_value) {
            return tag_value.tag() == a_tag;
        });
        if (m_tags.end() == find_it)
            return nullptr;
        return &*find_it;
    }

private:
    std::vector<TagValue> m_tags;
    bool m_sorted = true;
};

class NewGroup {
public:
    template <typename T>
    void AddTag(const Tag a_tag, const VRType a_vr, const uint32_t a_valueElements, StreamRead& a_stream) {

        if (1 < a_valueElements) { // multiple values
            const auto offset = LoadDataToBuf(a_stream, a_valueElements * sizeof(T));
            if (offset.has_value())
                m_tags.emplace(a_tag, a_vr, *offset);
        }
        else if (1 == a_valueElements) // single value
            m_tags.emplace(a_tag, a_vr, SingleValueUnion(a_stream.read<T>()));
        else
            m_tags.emplace(a_tag, a_vr); // no value
    }

private:
    std::optional<uint32_t> LoadDataToBuf(StreamRead& a_stream, uint32_t bytes) {
        const size_t elms_old = m_data_buf.size();
        const size_t elms_add = (bytes + 3) / 4 + 1; // aligned uint32_t and 1 extra element to hold length
        const size_t elms_new = elms_old + elms_add;

        // try to pre-reserve some size to reduce number of reallocations
        if (elms_new > m_data_buf.capacity())
        {
            size_t elms_to_reserve = elms_old + elms_old / 2; // 1.5x increase
            elms_to_reserve = ((elms_to_reserve + 15) / 16) * 16 ; // align to 16 dwords
            if (elms_to_reserve > elms_new &&
                elms_to_reserve - elms_new <= 1024)
                m_data_buf.reserve(elms_to_reserve);
        }

        m_data_buf.resize(elms_new, 0u);
        m_data_buf[elms_old] = bytes;
        const size_t bytes_readed = a_stream.readToMem(m_data_buf.data() + elms_old + 1, bytes);
        if (bytes_readed != bytes) {
            m_data_buf.resize(elms_old); // resize back
            return std::nullopt;
        }
        return static_cast<uint32_t>(elms_old);
    }

    bool HasTag(const Tag a_tag) const {
        return m_tags.HasTag(a_tag);
    }

    template <typename T>
    GetValueResult getTag(const Tag a_tag, T& a_value) const {
        const auto tag_ptr = m_tags.GetTagPtr(a_tag);
        if (!tag_ptr)
            return GetValueResult::DoesNotExists;
    }

private:
    TagVector m_tags;
    std::vector<uint32_t> m_data_buf;
};

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
