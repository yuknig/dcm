#ifndef _BUFFER_VIEW_6F6DAAD8_EB09_4B88_9C7E_C40BAECD60B1_
#define _BUFFER_VIEW_6F6DAAD8_EB09_4B88_9C7E_C40BAECD60B1_

#include <cassert>

template <typename T>
class buffer_view {
public:
    buffer_view(const T* a_begin, const T* a_end)
        : m_begin(a_begin)
        , m_end(a_end)
    {}

    buffer_view(const T* a_begin, size_t a_size)
        : buffer_view(a_begin, a_begin + a_size)
    {}

    const T* begin() const {
        return m_begin;
    }

    const T* end() const {
        return m_end;
    }

    size_t size() const {
        assert(reinterpret_cast<size_t>(m_end) >= reinterpret_cast<size_t>(m_begin));
        return m_end - m_begin;
    }

private:
    const T* const m_begin;
    const T* const m_end;
};

#endif // _BUFFER_VIEW_6F6DAAD8_EB09_4B88_9C7E_C40BAECD60B1_
