#ifndef _MVECTOR_095A6230_53C4_44F3_9960_EA0D824802FF_
#define _MVECTOR_095A6230_53C4_44F3_9960_EA0D824802FF_

#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <new>
#include <memory>
#include <string>

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
        delete_elements_from(0);
        m_data.reset();
        m_capacity = 0;
    }

    template <typename... ArgsT>
    void emplace_back(ArgsT&&... a_args) {
        check_capacity(1);

        new (m_data.get() + m_size) T {std::forward<ArgsT>(a_args)...};
        ++m_size;
    }

    void resize(SizeT a_new_size) {
        if (a_new_size > m_capacity)
            realloc_expand(a_new_size);
        else if (a_new_size < m_capacity)
            realloc_shrink(a_new_size, false);
    }

    void reserve(SizeT a_new_size) {
        if (a_new_size > m_capacity)
            realloc_expand(a_new_size);
    }

    void shrink_to_fit() {
        if (m_capacity > m_size)
            realloc_shrink(m_size, true);
    }

    SizeT capacity() const {
        return m_capacity;
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

    T& operator[](const SizeT a_pos) {
        assert(a_pos < size());
        return *(m_data.get() + a_pos);
    }

    const T& at(const SizeT a_pos) const {
        if (a_pos >= size())
            throw std::out_of_range(std::string(__func__) + "wrong index: " + std::to_string(a_pos));
        return this->operator[](a_pos);
    }

    T& at(const SizeT a_pos) {
        if (a_pos >= size())
            throw std::out_of_range(std::string(__func__) + "wrong index: " + std::to_string(a_pos));
        return this->operator[](a_pos);
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

    const T* begin() const {
        return cbegin();
    }

    T* begin() {
        return data();
    }

    const T* cend() const {
        return cbegin() + m_size;
    }

    const T* end() const {
        return cend();
    }

    T* end() {
        return begin() + m_size;
    }

    const T& front() const {
        return this->at(0);
    }

    T& front() {
        return this->at(0);
    }

    const T& back() const {
        if (this->empty())
            throw std::out_of_range(std::string(__func__) + ": container is empty");
        return this->operator[](size() - 1);
    }

    T& back() {
        if (this->empty())
            throw std::out_of_range(std::string(__func__) + ": container is empty");
        return this->operator[](size() - 1);
    }

private: // functions
    void check_capacity(SizeT elements_to_add) {
        size_t min_capacity = m_size + elements_to_add;
        if (min_capacity <= m_capacity)
            return;

        size_t capacity = std::max(min_capacity, capacity_incremented(m_capacity));
        if (capacity > std::numeric_limits<SizeT>::max()) {
            capacity = std::numeric_limits<SizeT>::max();
            if (capacity < min_capacity)
                throw std::length_error(std::string("too many elements:") + std::to_string(min_capacity));
        }
        realloc_expand(static_cast<SizeT>(capacity));
    }

    static size_t capacity_incremented(const size_t a_capacity) {
        size_t result = a_capacity + a_capacity / 2; // 1.5x increase
        if (result <= a_capacity)
            result = a_capacity + 4u;

        //TODO: align capacity
        assert(result > a_capacity);
        return result;
    }

    void realloc_expand(SizeT a_new_capacity) {
        assert(a_new_capacity > m_capacity);

        realloc_and_recreate_elements(a_new_capacity);
    }

    void realloc_shrink(SizeT a_new_capacity, bool do_realloc) {
        assert(a_new_capacity < m_capacity);

        delete_elements_from(a_new_capacity);
        if (do_realloc)
            realloc_and_recreate_elements(a_new_capacity);
    }

    void realloc_and_recreate_elements(typename std::enable_if<Reallocatable, SizeT>::type a_new_capacity) {
        static_assert(Reallocatable, "wrong specialization");

        auto new_data = std::unique_ptr<T, FreeDeleter>(static_cast<T*>(realloc(m_data.get(), a_new_capacity * sizeof(T))));
        if (!new_data)
            throw std::runtime_error("Failed realloc MVector");

        m_data.release();
        m_data = std::move(new_data);
        m_capacity = a_new_capacity;
    }

    template <bool Enabled = !Reallocatable && std::is_move_constructible<T>::value>
    void realloc_and_recreate_elements(const typename std::enable_if<Enabled, SizeT>::type a_new_capacity) {
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
    void delete_elements_from(typename std::enable_if<Enabled, SizeT>::type a_start_from) {
        static_assert(std::is_destructible<T>::value, "wrong specialization");

        if (a_start_from < m_size) {
            for (SizeT i = a_start_from; i < m_size; ++i)
                (m_data.get() + i)->~T();
            m_size = a_start_from;
        }
    }

    template <typename U = T,
              bool Enabled = !std::is_destructible<U>::value>
    void delete_elements_from(typename std::enable_if<Enabled, SizeT>::type a_start_from) {
        static_assert(!std::is_destructible<T>::value, "wrong specialization");

        m_size = a_start_from;
    }

private:
    std::unique_ptr<T, FreeDeleter> m_data;
    SizeT m_capacity = 0;
    SizeT m_size = 0;
};

#endif // _MVECTOR_095A6230_53C4_44F3_9960_EA0D824802FF_
