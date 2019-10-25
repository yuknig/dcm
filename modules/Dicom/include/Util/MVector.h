#ifndef _MVECTOR_095A6230_53C4_44F3_9960_EA0D824802FF_
#define _MVECTOR_095A6230_53C4_44F3_9960_EA0D824802FF_

#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <new>
#include <memory>

struct FreeDeleter {
    void operator()(void* x) {
        free(x);
    }
};

template <typename T, typename SizeT = size_t, bool Reallocatable = false>
class MVector {
    static_assert(std::is_integral<SizeT>::value && std::is_unsigned<SizeT>::value, "SizeT type should be integral and unsigned");

public:
    MVector() {
    }

    ~MVector() {
        deleteObjects();
        m_data.reset();
        m_capacity = 0;
    }

    template <typename... ArgsT>
    void emplace_back(ArgsT&&... a_args) {
        checkAllocSize();

        new (m_data.get() + m_size) T {std::forward<ArgsT>(a_args)...};
        ++m_size;
    }

    void resize(size_t a_new_size) {
        reallocate(a_new_size);
    }

    SizeT size() const {
        return m_size;
    }

    bool empty() const {
        return (0 == size());
    }

    const T& operator[](const size_t a_pos) const {
        assert(a_pos < size());

        return *(m_data.get() + a_pos);
    }

    const T* data() const {
        return m_data.get();
    }

    T* data() {
        return m_data.get();
    }

    const T* cbegin() const {
        return data();
    }

    T* begin() {
        return data();
    }

    const T* cend() const {
        return cbegin() + m_size;
    }

    T* end() {
        return begin() + m_size;
    }

private: // functions
    void checkAllocSize() {
        if (m_capacity > m_size)
            return;

        const auto new_size_alloc = sizeIncrement(m_capacity);
        reallocate(new_size_alloc);
    }

    static SizeT sizeIncrement(const size_t a_capacity) {
        size_t result = a_capacity + a_capacity / 2; // 1.5x increase
        if (result <= a_capacity)
            result = a_capacity + 4u;

        if (std::numeric_limits<SizeT>::max() < result)
            throw std::runtime_error("Too many elements");
        assert(result > a_capacity);
        return static_cast<SizeT>(result);
    }

    template <bool Enabled = Reallocatable,
              typename std::enable_if<Enabled, int>::type = 0>
    void reallocate(const SizeT a_new_size) {
        auto new_data = std::unique_ptr<T, FreeDeleter>(static_cast<T*>(realloc(m_data.get(), a_new_size * sizeof(T))));
        if (!new_data)
            throw std::runtime_error("Failed realloc MVector");

        m_data.release();
        m_data = std::move(new_data);
        m_capacity = a_new_size;
    }

    template <typename U = T,
              bool Enabled = Reallocatable,
              typename std::enable_if<std::is_move_constructible<U>::value &&
                                      !Reallocatable, int>::type = 0>
    void reallocate(const SizeT a_new_size) {
        std::unique_ptr<T, FreeDeleter> new_data(static_cast<T*>(malloc(a_new_size * sizeof(T))));
        if (!new_data)
            throw std::runtime_error("Failed to alloc MVector");

        for(size_t i = 0; i < m_size; ++i)
            new (new_data.get() + i) T (std::move(*(m_data.get() + i)));

        m_data.swap(new_data);
        m_capacity = a_new_size;
    }

    template <typename U = T,
              typename std::enable_if<std::is_destructible<U>::value, int>::type = 0>
    void deleteObjects() {
        for (size_t i = 0; i < m_size; ++i)
            (m_data.get() + i)->~T();
        m_size = 0;
    }

    template <typename U = T,
              typename std::enable_if<!std::is_destructible<U>::value, int>::type = 0>
    void deleteObjects() {
        m_size = 0;
    }

private:
    std::unique_ptr<T, FreeDeleter> m_data;
    SizeT m_capacity = 0;
    SizeT m_size = 0;
};

#endif // _MVECTOR_095A6230_53C4_44F3_9960_EA0D824802FF_
