#ifndef _SORTED_TAG_LIST_F1DEEE08_B4D8_44C1_B3E3_BFFECB0D1472_
#define _SORTED_TAG_LIST_F1DEEE08_B4D8_44C1_B3E3_BFFECB0D1472_

#include <Util/MVector.h>
#include <Dicom/TagStruct/SingleValue.h> // TODO: remove

namespace dcm
{

template <typename T, bool Realloc>
class SortedList_Tag_Base
{
public://functions
    SortedList_Tag_Base()
    {}

    template <typename... ArgsT>
    void emplace(bool& a_is_sorted, ArgsT&&... a_args);

    T* begin();
    T* end();

    bool hasTag(const Tag a_tag, bool a_is_sorted) const;

    void sort();

protected://functions
    const T* getTagPtr(const Tag a_tag, bool a_is_sorted) const;

    bool areLastTagsSorted() const;

protected://types
#if INTPTR_MAX == INT32_MAX
    using SizeType = uint16_t;
#else
    using SizeType = uint32_t;
#endif

protected://data
    //std::vector<T> m_list;
    MVector<T, SizeType, Realloc> m_list;
};

template <typename T, bool Realloc>
class SortedList_Tag_Value: public SortedList_Tag_Base<T, Realloc>
{
public://functions
    template <typename V>
    GetValueResult get(const Tag a_tag, V& a_result, bool a_is_sorted) const;

private://types
    typedef SortedList_Tag_Base<T, Realloc> Base;
};

template <typename T>
class SortedList_Tag_ValuePtr: public SortedList_Tag_Base<T, false>
{
public://functions
    template <typename V>
    GetValueResult get(const Tag a_tag, V& a_result, bool a_is_sorted) const;

private://types
    typedef SortedList_Tag_Base<T, false> Base;
};

} // namespace dcm
#endif // _SORTED_TAG_LIST_F1DEEE08_B4D8_44C1_B3E3_BFFECB0D1472_
