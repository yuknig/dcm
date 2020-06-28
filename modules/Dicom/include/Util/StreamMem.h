#ifndef _STREAMMEM_AB7E4C09_71A8_490B_9116_AAB2C1A13DA8_
#define _STREAMMEM_AB7E4C09_71A8_490B_9116_AAB2C1A13DA8_

#include <Dicom/Util.h>
#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>

template <typename T>
class MemStreamRead
{
public://functions
    MemStreamRead(const std::shared_ptr<const std::vector<T>>& a_data)
        : MemStreamRead(a_data, 0, a_data->size())
    {}

    MemStreamRead(const std::shared_ptr<const std::vector<T>>& a_data, size_t a_begin)
        : MemStreamRead(a_data, a_begin, a_data->size())
    {}

    MemStreamRead(const std::shared_ptr<const std::vector<T>>& a_data, size_t a_begin, size_t a_end)
        : m_data(a_data)
        , m_begin(a_begin)
        , m_end(a_end)
        , m_cur(a_begin)
    {
        static_assert(sizeof(T) == sizeof(char), "Only for 1 byte types");

        if (!m_data)
            throw std::invalid_argument("Null memory");
        if (a_end < a_begin)
            throw std::invalid_argument("Wrong memory range (a_end<a_begin )");
        if (a_end > a_data->size())
            throw std::invalid_argument("Wrong memory range (a_end>size)");
    }

    ~MemStreamRead() = default;

    /*std::unique_ptr<Impl> shrinkClone(size_t a_parent_start, size_t a_parent_end) const override
    {
        assert(a_parent_start <= a_parent_end);
        assert(size() >= a_parent_end);

        return std::make_unique<MemStreamImpl>(m_data, m_begin + a_parent_begin, m_begin + a_parent_end);
    }*/

    size_t read(void* a_dest, size_t a_size_in_bytes)
    {
        assert(a_dest);
        assert(0 < a_size_in_bytes);
        assert(m_end >= m_cur);

        const size_t bytes_left = GetBytesLeft();
        const size_t bytes_to_read = std::min(a_size_in_bytes, bytes_left);
        memcpy(a_dest, m_data->data() + m_cur, bytes_to_read);
        assert(bytes_to_read == a_size_in_bytes);
        return bytes_to_read;
    }

    size_t size() const
    {
        return (m_end > m_begin ? m_end - m_begin : 0u);
    }

    size_t pos() const
    {
        return m_cur - m_begin;
    }

    bool seek(size_t a_pos)
    {
        const size_t range = m_end - m_begin;
        if (a_pos > range)
        {
            assert(false);
            return false;
        }

        m_cur = a_pos;
        return true;
    }

    bool advance(const ptrdiff_t a_offset)
    {
        if (a_offset > 0)
        {
            const size_t bytes_left = GetBytesLeft();
            if (a_offset > static_cast<ptrdiff_t>(bytes_left))
            {
                assert(false);
                return false;
            }
        }
        else
        {
            if (a_offset < static_cast<ptrdiff_t>(m_begin - m_cur))
            {
                assert(false);
                return false;
            }
        }
        m_cur += a_offset;
        return true;
    }

protected://functions
    inline size_t GetBytesLeft() const
    {
        assert(m_data);
        assert(m_data->size() >= m_end);
        return m_end - m_cur;
    }

private://data
    std::shared_ptr<const std::vector<T>> m_data;
    size_t m_begin;
    size_t m_end;
    size_t m_cur;
};

#endif //_STREAMMEM_AB7E4C09_71A8_490B_9116_AAB2C1A13DA8_
