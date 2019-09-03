#pragma once
#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace string_util {

inline std::string TrimLeft(std::string str) {
    auto start = std::find_if_not(str.begin(), str.end(), std::isspace);
    str.erase(str.begin(), start);
    return str;
}

inline std::string TrimRight(std::string str) {
    auto end = std::find_if_not(str.rbegin(), str.rend(), std::isspace);
    str.erase(end.base(), str.end());
    return str;
}

inline std::string Strip(std::string str) {
    return TrimLeft(TrimRight(std::move(str)));
}

template <typename T>
inline bool ConvertFromString(const std::string& a_from, T& a_to) {
    std::istringstream iss(a_from);
    iss.imbue(std::locale::classic());
    T value;
    iss >> value;
    if (iss.fail())
        return false;

    a_to = std::move(value);
    return true;
}

class Tokenizer {
public:
    Tokenizer(std::string str, const std::string& seps, bool add_empty)
        : m_str(std::move(str))
        , m_tokens(Tokenize(m_str, seps, add_empty))
    {
    }

    size_t GetTokenCount() const {
        return m_tokens.size();
    }

    std::string GetToken(const size_t idx) const {
        const auto& token = m_tokens.at(idx);
        auto beg = m_str.begin() + token.m_start;
        auto end = beg + token.m_length;
        return std::string(beg, end);
    }

    std::vector<std::string> GetTokens() const {
        std::vector<std::string> result;
        result.reserve(m_tokens.size());
        for (size_t t = 0; t < m_tokens.size(); ++t)
            result.push_back(GetToken(t));
        return result;
    }

private:
    struct Token {
        size_t m_start  = 0;
        size_t m_length = 0;
    };

    static std::vector<Token> Tokenize(const std::string& str, const std::string& seps, bool add_empty) {
        std::vector<Token> result;

        size_t pos = 0u;
        while (pos < str.size()) {
            size_t sep_pos = str.find_first_of(seps, pos);
            if (sep_pos == std::string::npos)
                sep_pos = str.size();

            size_t length = sep_pos - pos;
            if (length || add_empty)
                result.push_back(Token{pos, length});
            pos = sep_pos + 1;
        }

        return result;
    }

private:
    std::string m_str;
    std::vector<Token> m_tokens;
};

} // namespace string_util
