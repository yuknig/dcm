#pragma once
#include <stdexcept>
#include <type_traits>
#include <new>
#include <memory>

struct FreeDeleter
{
    void operator()(void* x)
    {
        free(x);
    }
};

template <typename T, typename SizeT = size_t, bool Reallocatable = false>
class MVector
{
public:
    MVector()
    {}

    ~MVector()
    {
        deleteObjects();
        m_data.reset();
        m_size_alloc = 0;
    }

    template <typename... ArgsT>
    void emplace_back(ArgsT&&... a_args)
    {
        checkAllocSize();

        new (m_data.get() + m_size_used) T {std::forward<ArgsT>(a_args)...};
        ++m_size_used;
    }

    void resize(size_t a_new_size)
    {
        reallocate(a_new_size);
    }

    SizeT size() const
    {
        return m_size_used;
    }

    bool empty() const
    {
        return (0 == size());
    }

    const T& operator[](const size_t a_pos) const
    {
        assert(a_pos < m_size_used);

        return *(m_data.get() + a_pos);
    }

    const T* data() const
    {
        return m_data.get();
    }

    T* data()
    {
        return m_data.get();
    }

    const T* cbegin() const
    {
        return m_data.get();
    }

    T* begin()
    {
        return m_data.get();
    }

    const T* cend() const
    {
        return cbegin() + m_size_used;
    }

    T* end()
    {
        return begin() + m_size_used;
    }

private: // functions
    void checkAllocSize()
    {
        if (m_size_alloc > m_size_used)
            return;

        const auto new_size_alloc = sizeIncrement(m_size_alloc);
        reallocate(new_size_alloc);
    }

    static SizeT sizeIncrement(size_t a_size)
    {
        const size_t result = 1 < a_size ? a_size + a_size / 2 : 4;
        if (std::numeric_limits<SizeT>::max() < result)
            throw std::runtime_error("Too many elements");
        return static_cast<SizeT>(result);
    }

    template <bool Enabled = Reallocatable,
              typename std::enable_if<Enabled, int>::type = 0>
    void reallocate(const SizeT a_new_size)
    {
        auto* old_data = m_data.release();
        m_data = std::unique_ptr<T, FreeDeleter>(static_cast<T*>(realloc(old_data, a_new_size * sizeof(T))));
        if (!m_data)
            throw std::runtime_error("Failed realloc SortedTagList");

        m_size_alloc = a_new_size;
    }

    template <typename U = T,
              bool Enabled = Reallocatable,
              typename std::enable_if<std::is_move_constructible<U>::value &&
                                      !Reallocatable, int>::type = 0>
    void reallocate(const SizeT a_new_size)
    {
        std::unique_ptr<T, FreeDeleter> new_data(static_cast<T*>(malloc(a_new_size * sizeof(T))));
        if (!new_data)
            throw std::runtime_error("Failed to alloc SortedTagList");

        for(size_t i = 0; i < m_size_used; ++i)
            new (new_data.get() + i) T (std::move(*(m_data.get() + i)));

        m_data.swap(new_data);
        m_size_alloc = a_new_size;
    }

    template <typename U = T,
              typename std::enable_if<std::is_destructible<U>::value, int>::type = 0>
    void deleteObjects()
    {
        for (size_t i = 0; i < m_size_used; ++i)
            (m_data.get() + i)->~T();
        m_size_used = 0;
    }

    template <typename U = T,
              typename std::enable_if<!std::is_destructible<U>::value, int>::type = 0>
    void deleteObjects()
    {
        m_size_used = 0;
    }



private: // data
    std::unique_ptr<T, FreeDeleter> m_data;
    SizeT m_size_alloc = 0;
    SizeT m_size_used = 0;
};
