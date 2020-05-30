#include <cassert>
#include <Util/StreamMem.h>

template <typename T>
T StreamRead::read()
{
    assert(m_impl);

    T res = {};
    const size_t bytes_read = m_impl->read(&res, sizeof(T));
    if (bytes_read != sizeof(T))
    {
        assert(false);
    }
    return res;
}

template <typename T>
std::unique_ptr<StreamRead> StreamRead::Create(const std::shared_ptr<const std::vector<T>>& a_data, const size_t a_begin, const size_t a_end)
{
    auto impl = std::make_unique<MemStreamImpl<T>>(a_data, a_begin);
    return std::unique_ptr<StreamRead>(new StreamRead(std::move(impl)));
}
