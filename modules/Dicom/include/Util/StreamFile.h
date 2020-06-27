#ifndef STREAM_FILE_FDF99E61_DB60_4AD1_899C_D4F7E1E2C3DB_
#define STREAM_FILE_FDF99E61_DB60_4AD1_899C_D4F7E1E2C3DB_

#include <limits>
#include <cstdio>
#include <string>
#include <stdexcept>

class FileStreamRead
{
public:
    FileStreamRead(const std::string& path)
        : m_file(fopen(path.c_str(), "rb"), fclose)
    {
        if (!m_file)
            throw std::runtime_error(std::string(__func__) + " : failed to open file "+ path);

        // get file size
        {
            if (fseek(m_file.get(), 0, SEEK_END) != 0)
                assert(false);
            fpos_t size = 0;
            if (fgetpos(m_file.get(), &size) != 0)
                assert(false);

            m_size = static_cast<size_t>(size);
            if (static_cast<fpos_t>(m_size) != size)
                throw std::runtime_error(std::string(__func__) + " : file is too big (" + std::to_string(size) + " bytes):" + path);
            if (fseek(m_file.get(), 0, SEEK_SET) != 0)
                assert(false);
        }
    }

    size_t read(void* a_dest, size_t a_size_in_bytes)
    {
        assert(a_dest);

        auto bytes_read = fread(a_dest, 1, a_size_in_bytes, m_file.get());
        return bytes_read;
    }

    size_t size() const
    {
        return m_size;
    }

    size_t pos() const
    {
        fpos_t pos = 0;
        if (fgetpos(m_file.get(), &pos) != 0)
            assert(false);
        return static_cast<size_t>(pos);
    }

    bool seek(size_t a_pos)
    {
        if (fseek(m_file.get(), static_cast<long>(a_pos), SEEK_SET) != 0) {
            assert(false);
            return false;
        }
        return true;
    }

    bool advance(const ptrdiff_t a_offset)
    {
        if (fseek(m_file.get(), static_cast<long>(a_offset), SEEK_CUR) != 0) {
            assert(false);
            return false;
        }
        return true;
    }

private:
    std::unique_ptr<FILE, decltype(&fclose)> m_file;
    size_t m_size = 0u;
};

#endif
