#ifndef __STRING_FORMAT_HPP__
#define __STRING_FORMAT_HPP__

#include <sstream>


namespace cdba  {
namespace sec21 {


class StringFormat
{
public:
    explicit StringFormat(const std::string &fmt) : m_text(fmt), m_counter(1)
        {}
    explicit StringFormat(const std::string &&fmt) : m_text(fmt), m_counter(1)
        {}

    operator std::string() const
        {   return m_text;   }

    std::string string() const
        {   return m_text;   }

    template<class T>
    StringFormat& arg(T val)
    {
        std::string mod = modificator();
        std::string::size_type pos = 0;

        std::stringstream stream;
        stream <<val;

        std::string value = stream.str();

        do {
            pos = m_text.find(mod, pos);
            if(pos == std::string::npos)
                break;
            if(pos != 0 && m_text[pos - 1] == '%')
            {
                m_text.erase(pos, 1);
                ++pos;
                continue;
            }
            m_text.replace(pos, mod.size(), value);
        } while(true);

        ++m_counter;
        return *this;
    }


private:
    std::string modificator()
    {
        std::stringstream ss;
        ss <<"%"<<m_counter;
        return ss.str();
    }

private:
    std::string m_text;
    int m_counter;
};

///////////////////////////////////////////////////////////////////////////////
inline StringFormat format(std::string &&str)
{
    return StringFormat(str);
}

///////////////////////////////////////////////////////////////////////////////
inline StringFormat format(const char *str)
{
    return StringFormat(str);
}

///////////////////////////////////////////////////////////////////////////////
inline std::string hex(int value)
{
    std::stringstream str;
    str <<std::hex <<value;
    return str.str();
}

}
}

#endif // STRING_FORMAT_HPP
