#ifndef _PARSE_CONFIG_1E0CEA41_37F7_4351_B590_AFA2A7492616_
#define _PARSE_CONFIG_1E0CEA41_37F7_4351_B590_AFA2A7492616_

namespace dcm
{

struct ParserConfig
{
public://data
    ParserConfig()
        : m_explicit(true)
        , m_little_endian(true)
    {}

    ParserConfig(bool a_explicit, bool a_little_endian)
        : m_explicit(a_explicit)
        , m_little_endian(a_little_endian)
    {}

    bool IsExplicit() const
    {
        return m_explicit;
    }

public://data
    bool m_explicit;
    bool m_little_endian;
};

} // namespace dcm

#endif // _PARSE_CONFIG_1E0CEA41_37F7_4351_B590_AFA2A7492616_
