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
    MVector() = default;

    ~MVector() {
        deleteElementsFrom(0);
        m_data.reset();
        m_capacity = 0;
    }

    template <typename... ArgsT>
    void emplace_back(ArgsT&&... a_args) {
        checkAllocSize();

        new (m_data.get() + m_size) T {std::forward<ArgsT>(a_args)...};
        ++m_size;
    }

    void resize(SizeT a_new_size) {
        //TODO if (a_new_size < a_size)
        
        reallocate(a_new_size);
    }

    SizeT size() const {
        return m_size;
    }

    bool empty() const {
        return (0 == size());
    }

    const T& operator[](const SizeT a_pos) const {
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

    static SizeT sizeIncrement(const SizeT a_capacity) {
        size_t result = a_capacity + a_capacity / 2; // 1.5x increase
        if (result <= a_capacity)
            result = a_capacity + 4u;

        if (std::numeric_limits<SizeT>::max() < result)
            throw std::runtime_error("Too many elements");
        assert(result > a_capacity);
        return static_cast<SizeT>(result);
    }

    void reallocate(SizeT a_new_capacity) {
        if (a_new_capacity < m_capacity)
            deleteElementsFrom(a_new_capacity);
        reallocAndRecreateElements(a_new_capacity);
    }

    void reallocAndRecreateElements(typename std::enable_if<Reallocatable, SizeT>::type a_new_capacity) {
        static_assert(Reallocatable, "wrong specialization");

        auto new_data = std::unique_ptr<T, FreeDeleter>(static_cast<T*>(realloc(m_data.get(), a_new_capacity * sizeof(T))));
        if (!new_data)
            throw std::runtime_error("Failed realloc MVector");

        m_data.release();
        m_data = std::move(new_data);
        m_capacity = a_new_capacity;
    }

    template <bool Enabled = !Reallocatable && std::is_move_constructible<T>::value>
    void reallocAndRecreateElements(const typename std::enable_if<Enabled, SizeT>::type a_new_capacity) {
        static_assert(!Reallocatable && std::is_move_constructible<T>::value, "wrong specialization");

        std::unique_ptr<T, FreeDeleter> new_data(static_cast<T*>(malloc(a_new_capacity * sizeof(T))));
        if (!new_data)
            throw std::runtime_error("Failed to alloc MVector");

        for (SizeT i = 0; i < m_size; ++i)
            new (new_data.get() + i) T(std::move(*(m_data.get() + i)));
        m_data = std::move(new_data);
        m_capacity = a_new_capacity;
    }

    template <bool Enabled = std::is_destructible<T>::value>
    void deleteElementsFrom(typename std::enable_if<Enabled, SizeT>::type a_start_from) {
        static_assert(std::is_destructible<T>::value, "wrong specialization");

        if (a_start_from < m_size) {
            for (SizeT i = a_start_from; i < m_size; ++i)
                (m_data.get() + i)->~T();
            m_size = a_start_from;
        }
    }

    template <typename U = T,
              bool Enabled = !std::is_destructible<U>::value>
    void deleteElementsFrom(typename std::enable_if<Enabled, SizeT>::type a_start_from) {
        static_assert(!std::is_destructible<T>::value, "wrong specialization");

        m_size = a_start_from;
    }

private:
    std::unique_ptr<T, FreeDeleter> m_data;
    SizeT m_capacity = 0;
    SizeT m_size = 0;
};

#endif // _MVECTOR_095A6230_53C4_44F3_9960_EA0D824802FF_
