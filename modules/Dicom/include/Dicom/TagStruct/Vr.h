#ifndef _VR_D51624A6_039B_4C94_9797_94E013D36F74_
#define _VR_D51624A6_039B_4C94_9797_94E013D36F74_

#include <cstdint>

namespace dcm
{

enum class VRType: uint16_t {
    Undefined = 0,
    AE,
    AS,
    AT,
    CS,
    DA,
    DS,
    DT,
    FL,
    FD,
    IS,
    LO,
    LT,
    OB,
    OD,
    OF,
    OL,
    OW,
    PN,
    SH,
    SL,
    SQ,
    SS,
    ST,
    TM,
    UC,
    UI,
    UL,
    UN,
    UR,
    US,
    UT,
    Max = UT
};

enum class VRCode: uint16_t {
    Undefined = 0,
    AE = 0x4541,
    AS = 0x5341,
    AT = 0x5441,
    CS = 0x5343,
    DA = 0x4144,
    DS = 0x5344,
    DT = 0x5444,
    FL = 0x4C46,
    FD = 0x4446,
    IS = 0x5349,
    LO = 0x4F4C,
    LT = 0x544C,
    OB = 0x424F,
    OD = 0x444F,
    OF = 0x464F,
    OL = 0x4C4F,
    OW = 0x574F,
    PN = 0x4E50,
    SH = 0x4853,
    SL = 0x4C53,
    SQ = 0x5153,
    SS = 0x5353,
    ST = 0x5453,
    TM = 0x4D54,
    UC = 0x4355,
    UI = 0x4955,
    UL = 0x4C55,
    UN = 0x4E55,
    UR = 0x5255,
    US = 0x5355,
    UT = 0x5455
};


VRType vrCodeToVrType(const VRCode a_vrCode);
bool isStringVr(const VRType a_vr);
bool isSpecialVr(const VRType a_vr);

}//namespace dcm

#endif //_VR_D51624A6_039B_4C94_9797_94E013D36F74_
