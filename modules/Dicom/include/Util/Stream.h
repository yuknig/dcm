#ifndef _STREAM_E7ABA6CD_887D_4BBD_A25E_29D6917D82EA_
#define _STREAM_E7ABA6CD_887D_4BBD_A25E_29D6917D82EA_

#include <memory>
#include <vector>

class StreamRead;

class StreamRead
{
public://data
    //using T

public://functions
    template <typename T>
    static std::unique_ptr<StreamRead> Create(const std::shared_ptr<const std::vector<T>>& a_data, const size_t a_begin, const size_t a_end);

    //TODO
    //static std::unique_ptr<StreamRead> Create(const std::string& a_filename);

    virtual ~StreamRead();

    template <typename T>
    T read();

    size_t readToMem(void* a_dest, size_t a_size_in_bytes);

    bool isEnd() const;

    bool empty() const;
    size_t size() const;
    size_t sizeToEnd() const;
    size_t pos() const;
    bool seek(size_t a_pos);
    bool advance(ptrdiff_t a_offset);

public://types
    class Impl
    {
    public://functions
        virtual ~Impl() {}
        //virtual std::unique_ptr<Impl> shrinkClone(size_t a_parent_start, size_t a_parent_end) const = 0;
        virtual size_t read(void* a_dest, size_t a_size_in_bytes) = 0;
        virtual size_t size() const = 0;
        virtual size_t pos() const = 0;
        virtual bool seek(size_t a_pos) = 0;
        virtual bool advance(const ptrdiff_t a_offset) = 0;
    };

protected://functions
    StreamRead(std::unique_ptr<Impl> a_impl);
    //StreamRead(const StreamRead& a_src, const size_t a_sub_start, const size_t a_sub_end);

protected://data
    std::unique_ptr<Impl> m_impl;
};

class AutoRewind
{
public://AutoRewind
    AutoRewind(StreamRead& a_stream)
        : m_stream(a_stream)
        , m_pos(a_stream.pos())
    {}

    ~AutoRewind()
    {
        m_stream.seek(m_pos);
    }

private://data
    StreamRead& m_stream;
    size_t      m_pos;
};

#include <Util/Stream.inl>

#endif
