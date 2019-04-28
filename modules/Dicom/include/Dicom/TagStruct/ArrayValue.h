#ifndef _ARRAY_VALUE_5372D784_0A9D_4458_97C7_85027302A5FA_
#define _ARRAY_VALUE_5372D784_0A9D_4458_97C7_85027302A5FA_

#include <Dicom/TagStruct/TagValueBasic.h>
#include <Util/Stream.h>
#include <cassert>
#include <vector>

namespace dcm
{

namespace detail
{

template <typename SrcT, typename DstT>
GetValueResult GetAndCastSingleValueFromArray(const char* a_data, size_t a_data_size_bytes, size_t a_idx, DstT& a_dst) {
    assert(a_data);
    assert((a_data_size_bytes % sizeof(SrcT)) == 0);

    const size_t elms = a_data_size_bytes / sizeof(SrcT);
    if (elms <= a_idx)
        return GetValueResult::DoesNotExists;

    const SrcT* ptr = reinterpret_cast<const SrcT*>(a_data) + a_idx;
    const auto cast_res = CastValue<SrcT>(*ptr, a_dst);
    return CastResult_To_GetValueResult(cast_res);
}

template <typename CharT, typename DstT>
GetValueResult GetSingleValueFromString(const CharT* a_data, size_t a_data_size_bytes, DstT& a_dst) {
    assert(a_data);
    assert((a_data_size_bytes % sizeof(CharT)) == 0);

    if (!a_data || !a_data_size_bytes)
        return GetValueResult::DoesNotExists;

    const auto cast_res = CastValueFromString(std::basic_string<CharT>(a_data, a_data_size_bytes), a_dst);
    return CastResult_To_GetValueResult(cast_res);
}

template <typename T>
GetValueResult GetSingleValueFromArray(const char* a_data, size_t a_data_size_bytes, VRType a_vr, size_t idx, T& a_result) {

    assert(a_data);
    assert(a_data_size_bytes);

    if (!a_data || !a_data_size_bytes)
        return GetValueResult::DoesNotExists;

    switch (a_vr) {
    case VRType::OL:
        return GetAndCastSingleValueFromArray<uint32_t>(a_data, a_data_size_bytes, idx, a_result);
    case VRType::OW:
        return GetAndCastSingleValueFromArray<uint16_t>(a_data, a_data_size_bytes, idx, a_result);
    case VRType::OB:
        return GetAndCastSingleValueFromArray<uint8_t>(a_data, a_data_size_bytes, idx, a_result);
    case VRType::OD:
        return GetAndCastSingleValueFromArray<double>(a_data, a_data_size_bytes, idx, a_result);
    case VRType::OF:
        return GetAndCastSingleValueFromArray<float>(a_data, a_data_size_bytes, idx, a_result);
    case VRType::AE:
    case VRType::AS:
    case VRType::CS:
    case VRType::DA:
    case VRType::DS:
    case VRType::DT:
    case VRType::IS:
    case VRType::LO:
    case VRType::LT:
    case VRType::PN:
    case VRType::SH:
    case VRType::ST:
    case VRType::TM:
    case VRType::UC:
    case VRType::UI:
    case VRType::UR:
    case VRType::UT:
        assert(isStringVr(a_vr));
        return GetSingleValueFromString(a_data, a_data_size_bytes, a_result);
        //TODO: std::wstring
    default:
        assert(false);
        return GetValueResult::FailedCast;
    }
}

} // namespace detail

template <size_t Size>
class ShortArrayValueT : public Tag_Value<std::array<char, Size>>
{
    using Base = Tag_Value<std::array<char, Size>>;
public:
    ShortArrayValueT(const Tag a_tag, const VRType a_vr, const size_t a_length_in_bytes, const StreamRead& a_stream)
        : Base(a_tag, a_vr)
    {
        //TODO: isArrayVR()
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
    GetValueResult get(T& a_value) const
    {
        return detail::GetSingleValueFromArray(Base::m_value.data(), Base::m_value.size(), Base::m_vr, 0u, a_value);
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
        return Base::m_value_ptr->size();
    }

    template <typename T>
    GetValueResult get(T& a_value) const
    {
        return detail::GetSingleValueFromArray(Base::m_value_ptr->data(), Base::m_value_ptr->size(), Base::m_vr, 0u, a_value);
    }
};

} // namespace dcm


#endif // _ARRAY_VALUE_5372D784_0A9D_4458_97C7_85027302A5FA_