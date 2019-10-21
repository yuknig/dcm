#include <Dicom/TagValues.h>

namespace dcm
{

void Group::AddSequence(const Tag a_tag, std::vector<std::unique_ptr<Group>> a_groups) {
    auto offset = static_cast<uint32_t>(m_sequences.size());
    m_tags.emplace(a_tag, VRType::SQ, offset);
    m_sequences.emplace_back(std::move(a_groups));
    //TODO: implement
}

bool Group::HasTag(const Tag a_tag) const {
    return m_tags.HasTag(a_tag);
}

void Group::OnLoadingFinished() {
    m_tags.Sort();
    m_tags.ShrinkToFit();
    for (auto& sq : m_sequences) {
        for (auto& sq_item : sq)
            sq_item->OnLoadingFinished();
    }
    //TODO: shrink to fit
}

std::optional<uint32_t> Group::LoadDataToBuf(StreamRead& a_stream, uint32_t bytes) {
    const size_t elms_old = m_data_buf.size();
    const size_t elms_add = (bytes + 3) / 4 + 1; // aligned uint32_t and 1 extra element to hold length
    const size_t elms_new = elms_old + elms_add;

    // try to pre-reserve some size to reduce number of reallocations
    if (elms_new > m_data_buf.capacity())
    {
        size_t elms_to_reserve = elms_old + elms_old / 2; // 1.5x increase
        elms_to_reserve = ((elms_to_reserve + 15) / 16) * 16; // align to 16 dwords
        if (elms_to_reserve > elms_new &&
            elms_to_reserve - elms_new <= 1024)
            m_data_buf.reserve(elms_to_reserve);
    }

    m_data_buf.resize(elms_new, 0u);
    m_data_buf[elms_old] = bytes;
    const size_t bytes_readed = a_stream.readToMem(m_data_buf.data() + elms_old + 1, bytes);
    if (bytes_readed != bytes) {
        m_data_buf.resize(elms_old); // resize back
        return std::nullopt;
    }
    return static_cast<uint32_t>(elms_old);
}

buffer_view<uint8_t> Group::GetValueBuffer(const TagValue& a_desc) const {
    const auto& val_dsc = a_desc.GetValue();

    const auto storage = a_desc.GetStorage();
    if (storage == TagValue::ValueStorage::Inplace)
        return { ptrCast<uint8_t>(&val_dsc), sizeof(SingleValueUnion) };

    auto offset = val_dsc.m_uint32;
    assert(offset < m_data_buf.size());
    const auto* buf_ptr = ptrCast<uint8_t>(m_data_buf.data() + offset);
    auto buf_size = *ptrCast<uint32_t>(buf_ptr);
    buf_ptr += sizeof(uint32_t);
    return { buf_ptr, buf_size };
}

}//namespace dcm
