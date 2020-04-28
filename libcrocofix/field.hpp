#ifndef crocofix_libcrocofix_field_hpp
#define crocofix_libcrocofix_field_hpp

#include <string>

namespace crocofix
{

class field
{
public:

    field(int tag, std::string_view value);

    constexpr int tag() const { return m_tag; }
    constexpr const std::string& value() const { return m_value; } 

private:

    int m_tag;
    std::string m_value;

};

}

#endif