#ifndef _OPTIONAL_23690FF7_DD0A_4952_823E_1DA8E5AA703C_
#define _OPTIONAL_23690FF7_DD0A_4952_823E_1DA8E5AA703C_

#if (((defined(_MSC_VER)) && (_MSC_VER >=1910)) || (defined(__GNUC__)))
    #include <optional>
#else
    #include <boost/optional.hpp>

    namespace std
    {
        using boost::optional;
        const auto nullopt = boost::none;
    }
#endif //

#endif //_OPTIONAL_23690FF7_DD0A_4952_823E_1DA8E5AA703C_