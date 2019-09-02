#include <Util/CastValue.h>
#include <type_traits>
#include <limits>
#include <cassert>
#include <cstdint>
#include <string>


namespace dcm
{

namespace details
{

//template <typename To>
//GetValueResult castCopy(To& a_to, const To* a_from)
//{
//    assert(a_from);
//
//    a_to = *a_from;
//    return GetValueResult::Ok_NoCast;
//}
//
//template <typename To, typename From>
//GetValueResult castNoCheck(To& a_to, const From* a_from)
//{
//    static_assert((sizeof(To) > sizeof(From)) ||
//                  (std::is_floating_point<To>::value), "Wrong types");
//    assert(a_from);
//
//    a_to = static_cast<To>(*a_from);
//    return GetValueResult::Ok_WithCast;
//}
//
//template <typename Comp, typename To, typename From>
//GetValueResult castUnsignedToSigned(To& a_to, const From* a_from)
//{
//    //static_assert(std::is_same<Comp, typename std::common_type<To, From>::type>::value, "Wrong Comp type");
//    static_assert(std::is_unsigned<From>::value, "Wrong From type");
//    static_assert(std::is_signed<To>::value, "Wrong To type");
//    assert(a_from);
//
//    if (static_cast<Comp>(std::numeric_limits<To>::max()) < static_cast<Comp>(*a_from))
//        return GetValueResult::FailedCast;
//
//    a_to = static_cast<To>(*a_from);
//    return GetValueResult::Ok_WithCast;
//}
//
//template <typename Comp, typename To, typename From>
//GetValueResult castSignedToUnsigned(To& a_to, const From* a_from)
//{
//    //static_assert(std::is_same<Comp, typename std::common_type<To, From>::type>::value, "Wrong Comp type");
//    static_assert(std::is_signed<From>::value, "Wrong From type");
//    static_assert(std::is_unsigned<To>::value, "Wrong To type");
//    assert(a_from);
//
//    if ((0 > *a_from) || (static_cast<Comp>(std::numeric_limits<To>::max()) < static_cast<Comp>(*a_from)))
//        return GetValueResult::FailedCast;
//
//    a_to = static_cast<To>(*a_from);
//    return GetValueResult::Ok_WithCast;
//}
//
//template <typename Comp, typename To, typename From>
//GetValueResult castDown(To& a_to, const From* a_from)
//{
//    //static_assert(sizeof(Comp) == sizeof(typename std::common_type<To, From>::type), "WrongSize");
//    //static_assert(std::is_same<Comp, typename std::common_type<To, From>::type>::value, "Wrong Comp type");
//    assert(a_from);
//
//    if ((static_cast<Comp>(std::numeric_limits<To>::lowest()) > static_cast<Comp>(*a_from)) ||
//        (static_cast<Comp>(std::numeric_limits<To>::max()) < static_cast<Comp>(*a_from)))
//        return GetValueResult::FailedCast;
//
//    a_to = static_cast<To>(*a_from);
//    return GetValueResult::Ok_WithCast;
//}

//
//template <typename From>
//GetValueResult castToString(std::string& a_to, const From* a_from)
//{
//    assert(a_from);
//    a_to = std::to_string(*a_from);//TODO: use custom to avoid localization issues
//    return GetValueResult::Ok_WithCast; //TODO: add check
//}
//
//template <typename From>
//GetValueResult castToString(std::wstring& a_to, const From* a_from)
//{
//    assert(a_from);
//    a_to = std::to_wstring(*a_from);
//    return GetValueResult::Ok_WithCast; //TODO: add check
//}


template <typename T, typename ListT>
const T* findTagBinSearch(const ListT& a_tags, const Tag a_tag)
{
    auto findIt = std::lower_bound(a_tags.cbegin(), a_tags.cend(), a_tag, [](const T& a_lhs, const Tag& a_tag) {
        return (a_lhs.tag() < a_tag);
    });
    if ((a_tags.cend() == findIt) || (a_tag != findIt->tag()))
        return nullptr;
    return &(*findIt);
}

template <typename T, typename ListT>
const T* findTagLinearSearch(const ListT& a_tags, const Tag a_tag)
{
    auto findIt = std::find_if(a_tags.cbegin(), a_tags.cend(), [a_tag](const T& a_lhs) {
        return (a_tag == a_lhs.tag());
    });
    if (a_tags.cend() == findIt)
        return nullptr;
    return &(*findIt);
}

}//namespace details

//
//template <>
//inline GetValueResult SingleValue::get(int32_t& a_result) const
//{
//    switch (m_vr)
//    {
//        case VRType::SL:
//            return details::castCopy(a_result, &m_value.m_sint32);
//        case VRType::UL:
//        case VRType::OL:
//            return details::castUnsignedToSigned<uint32_t>(a_result, &m_value.m_uint32);
//        case VRType::SS:
//            return details::castNoCheck(a_result, &m_value.m_sint16);
//        case VRType::US:
//        case VRType::OW:
//            return details::castNoCheck(a_result, &m_value.m_uint16);
//        case VRType::FL:
//        case VRType::OF:
//            return details::castDown<float>(a_result, &m_value.m_float);
//        default:
//            break;
//    }
//    assert(false);
//    return GetValueResult::FailedCast;
//}
//
//template <>
//inline GetValueResult SingleValue::get(uint32_t& a_result) const
//{
//    switch (m_vr)
//    {
//        case VRType::SL:
//            return details::castSignedToUnsigned<uint32_t>(a_result, &m_value.m_sint32);
//        case VRType::UL:
//        case VRType::OL:
//            return details::castCopy(a_result, &m_value.m_uint32);
//        case VRType::SS:
//            return details::castSignedToUnsigned<uint32_t>(a_result, &m_value.m_sint16);
//        case VRType::US:
//        case VRType::OW:
//            return details::castNoCheck(a_result, &m_value.m_uint16);
//        case VRType::FL:
//        case VRType::OF:
//            return details::castSignedToUnsigned<float>(a_result, &m_value.m_float);
//        default:
//            break;
//    }
//    assert(false);
//    return GetValueResult::FailedCast;
//}
//
//template <>
//inline GetValueResult SingleValue::get(int64_t& a_result) const
//{
//    //no 64bit int types in Dicom
//    int32_t result = 0;
//    const GetValueResult res = get(result);
//    if (GetValueSucceeded(res))
//        a_result = result;
//    return res;
//}
//
//template <>
//inline GetValueResult SingleValue::get(uint64_t& a_result) const
//{
//    //no 64bit int types in Dicom
//    uint32_t result = 0;
//    const GetValueResult res = get(result);
//    if (GetValueSucceeded(res))
//        a_result = result;
//    return res;
//}
//
//template<>
//inline GetValueResult SingleValue::get(int16_t& a_result) const
//{
//    switch (m_vr)
//    {
//        case VRType::SL:
//            return details::castDown<int32_t>(a_result, &m_value.m_sint32);
//        case VRType::UL:
//        case VRType::OL:
//            return details::castUnsignedToSigned<uint32_t>(a_result, &m_value.m_uint32);
//        case VRType::SS:
//            return details::castCopy(a_result, &m_value.m_sint16);
//        case VRType::US:
//        case VRType::OW:
//            return details::castUnsignedToSigned<int32_t>(a_result, &m_value.m_uint16);
//        case VRType::FL:
//        case VRType::OF:
//            return details::castDown<float>(a_result, &m_value.m_float);
//        default:
//            break;
//    }
//    assert(false);
//    return GetValueResult::FailedCast;
//}
//
//template<>
//inline GetValueResult SingleValue::get(uint16_t& a_result) const
//{
//    switch (m_vr)
//    {
//        case VRType::SL:
//            return details::castSignedToUnsigned<int32_t>(a_result, &m_value.m_sint32);
//        case VRType::UL:
//        case VRType::OL:
//            return details::castDown<uint32_t>(a_result, &m_value.m_uint32);
//        case VRType::SS:
//            return details::castSignedToUnsigned<int32_t>(a_result, &m_value.m_sint16);
//        case VRType::US:
//        case VRType::OW:
//            return details::castCopy(a_result, &m_value.m_uint16);
//        case VRType::FL:
//        case VRType::OF:
//            return details::castDown<float>(a_result, &m_value.m_float);
//        default:
//            break;
//    }
//    assert(false);
//    return GetValueResult::FailedCast;
//}
//
//template<>
//inline GetValueResult SingleValue::get(int8_t& a_result) const
//{
//    switch (m_vr)
//    {
//        case VRType::SL:
//            return details::castDown<int32_t>(a_result, &m_value.m_sint32);
//        case VRType::UL:
//        case VRType::OL:
//            return details::castUnsignedToSigned<uint32_t>(a_result, &m_value.m_uint32);
//        case VRType::SS:
//            return details::castDown<int16_t>(a_result, &m_value.m_sint16);
//        case VRType::US:
//        case VRType::OW:
//            return details::castUnsignedToSigned<uint16_t>(a_result, &m_value.m_uint16);
//        case VRType::FL:
//        case VRType::OF:
//            return details::castDown<float>(a_result, &m_value.m_float);
//        default:
//            break;
//    }
//    assert(false);
//    return GetValueResult::FailedCast;
//}
//
//template<>
//inline GetValueResult SingleValue::get(uint8_t& a_result) const
//{
//    switch (m_vr)
//    {
//        case VRType::SL:
//            return details::castSignedToUnsigned<int32_t>(a_result, &m_value.m_sint32);
//        case VRType::UL:
//        case VRType::OL:
//            return details::castDown<uint32_t>(a_result, &m_value.m_uint32);
//        case VRType::SS:
//            return details::castSignedToUnsigned<int16_t>(a_result, &m_value.m_sint16);
//        case VRType::US:
//        case VRType::OW:
//            return details::castDown<uint16_t>(a_result, &m_value.m_uint16);
//        case VRType::FL:
//        case VRType::OF:
//            return details::castDown<float>(a_result, &m_value.m_float);
//        default:
//            break;
//    }
//    assert(false);
//    return GetValueResult::FailedCast;
//}
//
//template<>
//inline GetValueResult SingleValue::get(float& a_result) const
//{
//    switch (m_vr)
//    {
//        case VRType::SL:
//            return details::castNoCheck(a_result, &m_value.m_sint32);
//        case VRType::UL:
//        case VRType::OL:
//            return details::castNoCheck(a_result, &m_value.m_uint32);
//        case VRType::SS:
//            return details::castNoCheck(a_result, &m_value.m_sint16);
//        case VRType::US:
//        case VRType::OW:
//            return details::castNoCheck(a_result, &m_value.m_uint16);
//        case VRType::FL:
//        case VRType::OF:
//            return details::castCopy(a_result, &m_value.m_float);
//        default:
//            break;
//    }
//    assert(false);
//    return GetValueResult::FailedCast;
//}

//template <typename CharT>
//GetValueResult SingleValue::get(std::basic_string<CharT>& a_result) const
//{
//    switch (m_vr)
//    {
//        case VRType::SL:
//            return details::castToString(a_result, &m_value.m_sint32);
//        case VRType::UL:
//        case VRType::OL:
//            return details::castToString(a_result, &m_value.m_uint32);
//        case VRType::SS:
//            return details::castToString(a_result, &m_value.m_sint16);
//        case VRType::US:
//        case VRType::OW:
//            return details::castToString(a_result, &m_value.m_uint16);
//        case VRType::FL:
//        case VRType::OF:
//            return details::castToString(a_result, &m_value.m_float);
//        default:
//            assert(false);
//            break;
//    }
//    return GetValueResult::FailedCast;
//}

////////////////////////////////////////
//SortedList_Tag
////////////////////////////////////////

template <typename T, bool Realloc>
template <typename... ArgsT>
void SortedList_Tag_Base<T, Realloc>::emplace(bool& a_is_sorted, ArgsT&&... a_args)
{
    m_list.emplace_back(std::forward<ArgsT>(a_args)...);
    a_is_sorted = areLastTagsSorted();
}

template <typename T, bool Realloc>
void SortedList_Tag_Base<T, Realloc>::sort()
{
    std::sort(m_list.begin(), m_list.end(), [](const T& a_lhs, const T& a_rhs) {
        return (a_lhs.tag() < a_rhs.tag());
    });
}

template <typename T, bool Realloc>
bool SortedList_Tag_Base<T, Realloc>::areLastTagsSorted() const
{
    const size_t size = m_list.size();
    const bool result = (1u < size ? (m_list[size - 2].tag() < m_list[size - 1].tag()) : true);
    return result;
}

template <typename T, bool Realloc>
T* SortedList_Tag_Base<T, Realloc>::begin()
{
    return m_list.begin();
}

template <typename T, bool Realloc>
T* SortedList_Tag_Base<T, Realloc>::end()
{
    return m_list.end();
}

template <typename T, bool Realloc>
bool SortedList_Tag_Base<T, Realloc>::hasTag(const Tag a_tag, bool a_is_sorted) const
{
    const T* valuePtr = nullptr;
    if (a_is_sorted)
        valuePtr = details::findTagBinSearch<T>(m_list, a_tag);
    else
        valuePtr = details::findTagLinearSearch<T>(m_list, a_tag);

    return (nullptr != valuePtr);
}

template <typename T, bool Realloc>
template <typename V>
GetValueResult SortedList_Tag_Base<T, Realloc>::get(const Tag a_tag, V& a_result, bool a_is_sorted) const
{
    const T* valuePtr = getTagPtr(a_tag, a_is_sorted);
    if (!valuePtr)
        return GetValueResult::DoesNotExists;
    return valuePtr->get(a_result);
}


template <typename T, bool Realloc>
const T* SortedList_Tag_Base<T, Realloc>::getTagPtr(const Tag a_tag, bool a_is_sorted) const
{
    if (a_is_sorted)
        return details::findTagBinSearch<T>(m_list, a_tag);
    return details::findTagLinearSearch<T>(m_list, a_tag);
}

////////////////////////////////////////
//SortedList_Tag_Value
////////////////////////////////////////


//template <typename T, bool Realloc>
//template <typename V>
//std::shared_ptr<std::vector<V>> SortedList_Tag_Value<T, Realloc>::getVector(const Tag& a_tag, bool a_is_sorted) const
//{
//    T value = {};
//    GetValueResult res = this->get(a_tag, value, a_is_sorted);
//    if (!Succeeded(res))
//        return std::make_shared<std::vector<T>>();
//
//    return std::make_shared<std::vector<T>>(1, std::move(value));
//}

////////////////////////////////////////
//SortedTagPtrList
////////////////////////////////////////


template <typename T>
template <typename V>
std::shared_ptr<std::vector<V>> SortedList_Tag_ValuePtr<T>::getVector(const Tag& a_tag, bool a_is_sorted) const
{
    const T* valuePtr = Base::getTagPtr(a_tag, a_is_sorted);
    if (valuePtr)
        return valuePtr->valueSharedPtr();

    return {};
}

}//namespace dcm
