#include <cassert>
#include <Util/StreamMem.h>

template <typename T>
T StreamRead::read()
{
    const T result = readInPlace<T>();
    advance(sizeof(T));
    return result;
}

template <typename T>
T StreamRead::readInPlace() const
{
    return readInPlaceWithOffset<T>(0);
}

template <typename T>
T StreamRead::readInPlaceWithOffset(size_t a_offset_in_bytes) const
{
    //TODO static_assert is_pod
    T res = {};
    if (!m_impl)
    {
        assert(false);
        return res;
    }

    const size_t bytes_read = m_impl->readInPlace(&res, sizeof(T), a_offset_in_bytes);
    if (bytes_read != sizeof(T))
    {
        assert(false);
    }
    return res;
}

template <typename T>
std::unique_ptr<StreamRead> StreamRead::Create(const std::shared_ptr<const std::vector<T>>& a_data, const size_t a_begin, const size_t a_end)
{
    return std::unique_ptr<StreamRead>(new StreamRead(std::make_unique<MemStreamImpl<T>>(a_data, a_begin)));
}
