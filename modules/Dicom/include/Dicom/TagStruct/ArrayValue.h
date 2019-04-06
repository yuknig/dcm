#ifndef _ARRAY_VALUE_5372D784_0A9D_4458_97C7_85027302A5FA_
#define _ARRAY_VALUE_5372D784_0A9D_4458_97C7_85027302A5FA_

#include <Dicom/TagStruct/TagValueBasic.h>
#include <Util/Stream.h>
#include <cassert>
#include <vector>

namespace dcm
{

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

} // namespace dcm


#endif // _ARRAY_VALUE_5372D784_0A9D_4458_97C7_85027302A5FA_