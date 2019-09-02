#ifndef _TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_
#define _TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_

#include <Dicom/TagStruct/SingleValue.h>
#include <Dicom/TagStruct/ArrayValue.h>
#include <Dicom/TagStruct/SortedTagList.h>
#include <Dicom/TagValues.h>
#include <Dicom/Util.h>
#include <Util/buffer_view.h>
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



//typedef std::shared_ptr<NewGroup> GroupPtr;



class TagValue {
public:
    enum class ValueStorage : uint16_t {
        NoValue = 0,
        Inplace,
        Buffer
    };

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

    const SingleValueUnion& GetValue() const {
        return m_value;
    }

    VRType GetVR() const {
        return m_vr;
    }

    ValueStorage GetStorage() const {
        return m_storage;
    }

private:
    Tag m_tag;
    SingleValueUnion m_value;
    ValueStorage m_storage;
    VRType m_vr;
};

template <typename TagT>
class TagVector {
public:
    template <typename... ArgsT>
    void emplace(ArgsT&&... a_args) {
        m_tags.emplace_back(std::forward<ArgsT>(a_args)...);
        if (m_sorted && m_tags.size() > 1) // check that vector is still sorted
            m_sorted = m_tags[m_tags.size() - 2].tag() < m_tags.back().tag();
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

    const TagValue* GetTagPtr(const Tag& a_tag) const {
        return m_sorted ? GetTagPtr_Sorted(a_tag)
            : GetTagPtr_Unsorted(a_tag);
    }

private:

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
    std::vector<TagT> m_tags;
    bool m_sorted = true;
};

class NewGroup;

class NewSequence {
public:
    using Item = NewGroup;
    using ItemPtr = std::unique_ptr<Item>;

    NewSequence(const Tag& a_tag, std::vector<ItemPtr> a_items)
        : m_tag(a_tag)
        , m_items(std::move(a_items))
    {}

private:
    Tag m_tag;
    std::vector<ItemPtr> m_items;
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

    template <>
    void AddTag<double>(const Tag a_tag, const VRType a_vr, const uint32_t a_valueElements, StreamRead& a_stream) {
        const auto offset = LoadDataToBuf(a_stream, a_valueElements * sizeof(double));
        if (offset.has_value())
            m_tags.emplace(a_tag, a_vr, *offset);
    }

    void AddSequence(const Tag a_tag, std::vector<std::unique_ptr<NewGroup>> a_groups) {
        auto offset = static_cast<uint32_t>(m_sequences.size());
        m_tags.emplace(a_tag, VRType::SQ, offset);
        m_sequences.emplace_back(a_tag, std::move(a_groups));
        //TODO: implement
    }

    bool HasTag(const Tag a_tag) const {
        return m_tags.HasTag(a_tag);
    }

    template <typename T>
    GetValueResult GetTag(const Tag a_tag, T& a_value) const {
        const TagValue* tag_ptr = m_tags.GetTagPtr(a_tag);
        if (!tag_ptr || tag_ptr->GetStorage() == TagValue::ValueStorage::NoValue)
            return GetValueResult::DoesNotExists;

        CastResult res = GetAndCastValue(*tag_ptr, a_value);
        return CastResult_To_GetValueResult(res);
    }

    void OnLoadingFinished() {
        m_tags.Sort();
        //TODO: shrink to fit
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

    buffer_view<uint8_t> GetValueBuffer(const TagValue& a_desc) const {
        const auto& val_dsc = a_desc.GetValue();

        const auto storage = a_desc.GetStorage();
        if (storage == TagValue::ValueStorage::Inplace)
            return { ptrCast<uint8_t>(&val_dsc), sizeof(SingleValueUnion) };

        auto offset = val_dsc.m_uint32;
        assert(offset < m_data_buf.size());
        const auto* buf_ptr = ptrCast<uint8_t>(m_data_buf.data() + offset);
        auto buf_size = *ptrCast<uint32_t>(buf_ptr);
        buf_ptr += sizeof(uint32_t);
        return { buf_ptr, buf_size };
    }

    template <typename ToT>
    CastResult GetAndCastValue(const TagValue& a_desc, ToT& a_result) const {

        auto data = GetValueBuffer(a_desc);

        const VRType vr = a_desc.GetVR();

        if (isStringVr(vr)) {
            //TODO: handle wide chars
            //TODO: separate char and potential wchar_t VRs
            return CastValueFromString(std::string(ptrCast<char>(data.begin()), data.size()), a_result);
        }

        switch (vr) {
        case VRType::SL:
            return CastValue<int32_t>(*ptrCast<int32_t>(data.begin()), a_result);
        case VRType::UL:
        case VRType::OL:
            return CastValue<uint32_t>(*ptrCast<uint32_t>(data.begin()), a_result);
        case VRType::SS:
            return CastValue<int16_t>(*ptrCast<int16_t>(data.begin()), a_result);
        case VRType::US:
        case VRType::OW:
            return CastValue<uint16_t>(*ptrCast<int16_t>(data.begin()), a_result);
        case VRType::OB:
            return CastValue<uint8_t>(*ptrCast<uint8_t>(data.begin()), a_result);
        case VRType::FL:
        case VRType::OF:
            return CastValue<float>(*ptrCast<float>(data.begin()), a_result);
        case VRType::FD:
        case VRType::OD:
            return CastValue<double>(*ptrCast<double>(data.begin()), a_result);
        default:
            assert(false);
            return CastResult::FailedCast;
        }
    }

private:
    TagVector<TagValue> m_tags;
    std::vector<uint32_t> m_data_buf;
    std::vector<NewSequence> m_sequences;
};

}//namespace dcm

#include <Dicom/TagValues.inl>

#endif //_TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_
