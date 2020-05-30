#include <Util/Stream.h>
#include <Util/StreamFile.h>

StreamRead::StreamRead(std::unique_ptr<Impl> a_impl)
    : m_impl(std::move(a_impl))
{}

StreamRead::~StreamRead()
{}

size_t StreamRead::readToMem(void* a_dest, size_t a_size_in_bytes)
{
    assert(m_impl);
    return m_impl->read(a_dest, a_size_in_bytes);
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

std::unique_ptr<StreamRead> StreamRead::Create(const std::string& a_filename)
{
    std::unique_ptr<FileStreamImpl> impl(new FileStreamImpl(a_filename));
    return std::unique_ptr<StreamRead>(new StreamRead(std::move(impl)));
}
