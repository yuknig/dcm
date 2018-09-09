namespace dcm
{

template <typename To>
inline To* ptrCast(void* const a_ptr)
{
    To* const result = static_cast<To*>(a_ptr);
    return result;
}

template <typename To>
inline const To* ptrCast(const void* const a_ptr)
{
    const To* const result = static_cast<const To*>(a_ptr);
    return result;
}

} // dcm
