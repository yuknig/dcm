#include <Dicom/Vr.h>

#include <array>
#include <algorithm>
#include <assert.h>


namespace dcm
{

VRType vrCodeToVrType(const VRCode a_vrCode)
{
    typedef std::pair<VRCode, VRType> VRCodeTypePair;

    static const std::array<VRCodeTypePair, 32> SortedVRs
    {{
        {VRCode::Undefined, VRType::Undefined},
        {VRCode::DA, VRType::DA}, {VRCode::OB, VRType::OB}, {VRCode::UC, VRType::UC},
        {VRCode::FD, VRType::FD}, {VRCode::OD, VRType::OD}, {VRCode::AE, VRType::AE},
        {VRCode::OF, VRType::OF}, {VRCode::SH, VRType::SH}, {VRCode::UI, VRType::UI},
        {VRCode::FL, VRType::FL}, {VRCode::OL, VRType::OL}, {VRCode::SL, VRType::SL},
        {VRCode::UL, VRType::UL}, {VRCode::TM, VRType::TM}, {VRCode::PN, VRType::PN},
        {VRCode::UN, VRType::UN}, {VRCode::LO, VRType::LO}, {VRCode::SQ, VRType::SQ},
        {VRCode::UR, VRType::UR}, {VRCode::AS, VRType::AS}, {VRCode::CS, VRType::CS},
        {VRCode::DS, VRType::DS}, {VRCode::IS, VRType::IS}, {VRCode::SS, VRType::SS},
        {VRCode::US, VRType::US}, {VRCode::AT, VRType::AT}, {VRCode::DT, VRType::DT},
        {VRCode::LT, VRType::LT}, {VRCode::ST, VRType::ST}, {VRCode::UT, VRType::UT},
        {VRCode::OW, VRType::OW}
    }};

    assert(std::is_sorted(SortedVRs.begin(), SortedVRs.end(), [](const VRCodeTypePair& a_lhs, const VRCodeTypePair& a_rhs) {return a_lhs.first < a_rhs.first;}));
    static_assert(SortedVRs.size() == static_cast<size_t>(VRType::Max) + 1, "VRCode / VRType mismatch");

    const auto findIt = std::lower_bound(SortedVRs.begin(), SortedVRs.end(), a_vrCode, [](const VRCodeTypePair& a_lhs, const VRCode& a_rhs) {
        return (a_lhs.first < a_rhs);
    });
    if (SortedVRs.end() == findIt)
        return VRType::Undefined;
    return findIt->second;
}

bool isStringVr(const VRType a_vr)
{
    static const std::array<VRType, 17> StringVRs
    {
        VRType::AE, VRType::AS, VRType::CS, VRType::DA, VRType::DS, VRType::DT, VRType::IS, VRType::LO, VRType::LT,
        VRType::PN, VRType::SH, VRType::ST, VRType::TM, VRType::UC, VRType::UI, VRType::UR, VRType::UT
    };
    assert(std::is_sorted(StringVRs.begin(), StringVRs.end()));

    return std::binary_search(StringVRs.begin(), StringVRs.end(), a_vr);
}

bool isSpecialVr(const VRType a_vr)
{
    static const std::array<VRType, 10> specialVrs =
    {{
        VRType::OB, VRType::OD, VRType::OF, VRType::OL, VRType::OW, VRType::SQ, VRType::UC, VRType::UN, VRType::UR, VRType::UT
    }};
    assert(std::is_sorted(specialVrs.begin(), specialVrs.end()));

    const bool result = std::binary_search(specialVrs.begin(), specialVrs.end(), a_vr);
    return result;
}

} // namespace dcm
