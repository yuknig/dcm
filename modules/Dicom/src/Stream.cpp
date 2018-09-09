#include <Util/Stream.h>

StreamRead::StreamRead(std::unique_ptr<Impl> a_impl)
    : m_impl(std::move(a_impl))
{}

//StreamRead::StreamRead(const StreamRead& a_src, const size_t a_sub_start, const size_t a_sub_end)
//    : m_impl(a_src.m_impl->shrinkClone(0, a_src.m_impl->size()))
//{
//}

StreamRead::~StreamRead()
{}

size_t StreamRead::readToMem(void* a_dest, size_t a_size_in_bytes)
{
    const size_t res = readToMemInPlace(a_dest, a_size_in_bytes);
    advance(res);
    return res;
}

size_t StreamRead::readToMemInPlace(void* a_dest, size_t a_size_in_bytes) const
{
    return readToMemInPlaceWithOffset(a_dest, a_size_in_bytes, 0);
}

size_t StreamRead::readToMemInPlaceWithOffset(void* a_dest, size_t a_size_in_bytes, size_t a_offset_in_bytes) const
{
    assert(m_impl);
    return m_impl->readInPlace(a_dest, a_size_in_bytes, a_offset_in_bytes);
}

bool StreamRead::isEnd() const
{
    return (0 == sizeToEnd());
}

bool StreamRead::empty() const
{
    return 0 == size();
}

size_t StreamRead::size() const
{
    assert(m_impl);
    return m_impl->size();
}

size_t StreamRead::sizeToEnd() const
{
    const size_t s = size();
    const size_t p = pos();
    if (s >= p)
        return s - p;
    if (s != p)
    {
        assert(false);
    }
    return 0u;
}

size_t StreamRead::pos() const
{
    assert(m_impl);
    return m_impl->pos();
}

bool StreamRead::seek(size_t a_pos)
{
    assert(m_impl);
    return m_impl->seek(a_pos);
}

bool StreamRead::advance(ptrdiff_t a_offset)
{
    assert(m_impl);
    return m_impl->advance(a_offset);
}
