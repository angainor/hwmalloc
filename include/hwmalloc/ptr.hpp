#pragma once

#include <hwmalloc/void_ptr.hpp>
#include <hwmalloc/memfct_ptr.hpp>
#include <iterator>

namespace hwmalloc
{
template<typename T, memory_type MemoryType>
class hw_ptr
{
  private:
    friend class hw_vptr<MemoryType, void*>;
    using this_type = hw_ptr<T, MemoryType>;
    using void_ptr_t = hw_vptr<MemoryType, void*>;
    using const_void_ptr_t = hw_vptr<MemoryType, void const*>;
    void_ptr_t m_ptr;

  public: // iteator typedefs
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::random_access_iterator_tag;

  public:
    constexpr hw_ptr() noexcept = default;
    constexpr hw_ptr(hw_ptr const&) noexcept = default;
    constexpr hw_ptr(std::nullptr_t) noexcept
    : m_ptr{nullptr}
    {
    }
    hw_ptr& operator=(hw_ptr const&) noexcept = default;
    hw_ptr& operator=(std::nullptr_t) noexcept
    {
        m_ptr = nullptr;
        return *this;
    }

    constexpr friend bool operator==(hw_ptr a, hw_ptr b) noexcept { return (a.m_ptr == b.m_ptr); }
    constexpr friend bool operator!=(hw_ptr a, hw_ptr b) noexcept { return (a.m_ptr != b.m_ptr); }

  public:
    reference operator*() const noexcept { return *reinterpret_cast<T*>(m_ptr.get()); }
    pointer   operator->() const noexcept { return reinterpret_cast<T*>(m_ptr.get()); }
    pointer   get() const noexcept { return reinterpret_cast<T*>(m_ptr.get()); }

    // pointer to member function
    template<typename R, typename U, typename... Args>
    typename std::enable_if<std::is_same<U, T>::value && std::is_class<U>::value,
        const pmfc<R (U::*)(Args...)>>::type
    operator->*(R (U::*pmf)(Args...)) const noexcept
    {
        return {get(), pmf};
    }
    template<typename R, typename U, typename... Args>
    typename std::enable_if<std::is_same<U, T>::value && std::is_class<U>::value,
        const pmfc<R (U::*)(Args...) const>>::type
    operator->*(R (U::*pmf)(Args...) const) const noexcept
    {
        return {get(), pmf};
    }
    // pointer to member
    template<typename M, typename U>
    typename std::enable_if<std::is_same<U, T>::value && std::is_class<U>::value, M&>::type
    operator->*(M U::*pm) const noexcept
    {
        return get()->*pm;
    }

    constexpr explicit operator void_ptr_t() const noexcept { return m_ptr; }
    constexpr explicit operator const_void_ptr_t() const noexcept { return m_ptr; }
    // needed for std::allocator_traits::construct
    constexpr explicit operator void*() const noexcept { return m_ptr.get(); }
    constexpr          operator bool() const noexcept { return (bool)m_ptr; }

  public: // iterator functions
    this_type& operator++() noexcept
    {
        m_ptr.set(get() + 1);
        return *this;
    }
    this_type operator++(int) noexcept
    {
        auto tmp = *this;
        m_ptr.set(get() + 1);
        return tmp;
    }
    this_type& operator+=(std::size_t n) noexcept
    {
        m_ptr.set(get() + n);
        return *this;
    }
    friend this_type operator+(this_type a, std::size_t n) noexcept { return (a += n); }

    reference& operator[](std::size_t n) const noexcept { return *(get() + n); }
};

template<memory_type MemoryType, typename VoidPtr>
template<typename T>
constexpr hw_vptr<MemoryType, VoidPtr>&
hw_vptr<MemoryType, VoidPtr>::operator=(hw_ptr<T, MemoryType> const& ptr) noexcept
{
    return *this = (this_type)ptr;
}

template<memory_type MemoryType, class VoidPtr>
template<class T>
constexpr hw_vptr<MemoryType, VoidPtr>::operator hw_ptr<T, MemoryType>() const noexcept
{
    auto p = hw_ptr<T, MemoryType>();
    p.m_ptr = *this;
    return p;
}

template<typename T, memory_type MemoryType>
class hw_ptr<const T, MemoryType>
{
  private:
    friend class hw_vptr<MemoryType, void*>;
    friend class hw_vptr<MemoryType, void const*>;
    using this_type = hw_ptr<const T, MemoryType>;
    using void_ptr_t = hw_vptr<MemoryType, void*>;
    using const_void_ptr_t = hw_vptr<MemoryType, void const*>;
    const_void_ptr_t m_ptr;

  public: // iteator typedefs
    using value_type = T const;
    using difference_type = std::ptrdiff_t;
    using pointer = T const*;
    using reference = T const&;
    using iterator_category = std::random_access_iterator_tag;

  public:
    constexpr hw_ptr() noexcept = default;
    constexpr hw_ptr(hw_ptr const&) noexcept = default;
    constexpr hw_ptr(hw_ptr<T, MemoryType> const& ptr) noexcept
    : m_ptr((void_ptr_t)ptr)
    {
    }
    constexpr hw_ptr(std::nullptr_t) noexcept
    : m_ptr{nullptr}
    {
    }
    hw_ptr& operator=(hw_ptr const&) noexcept = default;
    hw_ptr& operator=(std::nullptr_t) noexcept
    {
        m_ptr = nullptr;
        return *this;
    }

    constexpr friend bool operator==(hw_ptr a, hw_ptr b) noexcept { return (a.m_ptr == b.m_ptr); }
    constexpr friend bool operator!=(hw_ptr a, hw_ptr b) noexcept { return (a.m_ptr != b.m_ptr); }

  public:
    reference operator*() const noexcept { return *reinterpret_cast<T const*>(m_ptr.get()); }
    pointer   operator->() const noexcept { return reinterpret_cast<T const*>(m_ptr.get()); }
    pointer   get() const noexcept { return reinterpret_cast<T const*>(m_ptr.get()); }

    constexpr explicit operator const_void_ptr_t() const noexcept { return m_ptr; }
    constexpr          operator bool() const noexcept { return (bool)m_ptr; }
};

template<memory_type MemoryType>
template<typename T>
constexpr hw_vptr<MemoryType, void const*>&
hw_vptr<MemoryType, void const*>::operator=(hw_ptr<T, MemoryType> const& ptr) noexcept
{
    return *this = (this_type)ptr;
}

template<memory_type MemoryType>
template<class T, typename>
constexpr hw_vptr<MemoryType, void const*>::operator hw_ptr<T, MemoryType>() const noexcept
{
    auto p = hw_ptr<T, MemoryType>();
    p.m_ptr = *this;
    return p;
}

template<memory_type MemoryType>
class hw_ptr<void, MemoryType>
{
};
template<memory_type MemoryType>
class hw_ptr<void const, MemoryType>
{
};

} // namespace hwmalloc
