#ifndef _TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_
#define _TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_

#include <Dicom/TagStruct/SingleValue.h>
#include <Dicom/TagStruct/TagNum.h>
#include <Dicom/TagStruct/Vr.h>
#include <Dicom/Util.h>
#include <Util/buffer_view.h>
#include <Util/Stream.h>
#include <Util/MVector.h>
#include "Util/CastValue.h" //TODO: harmonize style of includes

#include <vector>
#include <memory>
#include <algorithm>
#include <cstring>
#include <cassert>
#include <optional>

namespace dcm
{

enum class GetValueResult {
    DoesNotExists = 0, // fix to DoesNotExist
    FailedCast,
    Ok_WithCast,
    Ok_NoCast
};

inline bool Succeeded(const GetValueResult a_result) {
    return (GetValueResult::Ok_WithCast <= a_result);
}

inline GetValueResult CastResult_To_GetValueResult(const CastResult& a_from) {
    switch (a_from) {
    case CastResult::Ok_NoCast:
        return GetValueResult::Ok_NoCast;
    case CastResult::Ok_CastLoseless:
    case CastResult::Ok_CastLossy:
        return GetValueResult::Ok_WithCast;
    case CastResult::FailedCast:
    default:
        return GetValueResult::FailedCast;
    }
}


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

} // namespace detail


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
        m_sorted = true;
    }

    void ShrinkToFit() {
        m_tags.shrink_to_fit();
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
    MVector<TagT> m_tags;
    //std::vector<TagT> m_tags; // left here for more convinient debugging
    bool m_sorted = true;
};

class Group;

class Sequence {
public:
    using Item = Group;
    using ItemPtr = std::unique_ptr<Item>;
    using ItemPtrContainer = std::vector<ItemPtr>;

    Sequence(std::vector<ItemPtr> a_items)
        : m_items(std::move(a_items))
    {}

    ItemPtrContainer::iterator begin() {
        return m_items.begin();
    }

    ItemPtrContainer::iterator end() {
        return m_items.end();
    }

private:
    ItemPtrContainer m_items;
};


class Group {
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

    void AddSequence(const Tag a_tag, std::vector<std::unique_ptr<Group>> a_groups);

    bool HasTag(const Tag a_tag) const;

    template <typename T>
    GetValueResult GetTag(const Tag a_tag, T& a_value) const {
        const TagValue* tag_ptr = m_tags.GetTagPtr(a_tag);
        if (!tag_ptr || tag_ptr->GetStorage() == TagValue::ValueStorage::NoValue)
            return GetValueResult::DoesNotExists;

        CastResult res = GetAndCastValue(*tag_ptr, a_value);
        return CastResult_To_GetValueResult(res);
    }

    void OnLoadingFinished();

private:
    std::optional<uint32_t> LoadDataToBuf(StreamRead& a_stream, uint32_t bytes);

    buffer_view<uint8_t> GetValueBuffer(const TagValue& a_desc) const;

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
    MVector<uint32_t> m_data_buf;
    //std::vector<uint32_t> m_data_buf; // left here for more convinient debugging
    std::vector<Sequence> m_sequences;
};

template <>
inline void Group::AddTag<double>(const Tag a_tag, const VRType a_vr, const uint32_t a_valueElements, StreamRead& a_stream) {
    const auto offset = LoadDataToBuf(a_stream, a_valueElements * sizeof(double));
    if (offset.has_value())
        m_tags.emplace(a_tag, a_vr, *offset);
}

}//namespace dcm

#endif //_TAG_VALUES_6E3840DA_ADC7_47BF_843A_7ABDA93A5263_
