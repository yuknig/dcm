#include <Dicom/TagStruct/TagValueBasic.h>
#include <tuple>

namespace dcm
{

Tag_NoValue::Tag_NoValue(const Tag a_tag, const VRType a_vr)
    : m_tag(a_tag)
    , m_vr(a_vr)
{}

Tag Tag_NoValue::tag() const {
    return m_tag;
}

VRType Tag_NoValue::vr() const {
    return m_vr;
}

bool Tag_NoValue::operator<(const Tag_NoValue& a_rhs) const {
    return std::tie(m_tag, m_vr) < std::tie(a_rhs.m_tag, a_rhs.m_vr);
}

} // namespace dcm
