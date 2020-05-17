#ifndef crocofix_libcrocofix_field_hpp
#define crocofix_libcrocofix_field_hpp

#include <string>
#include <libcrocofixdictionary/field_value.hpp>

namespace crocofix
{

class field
{
public:

    field(int tag, const dictionary::field_value& value);
    field(int tag, std::string_view value);
    field(int tag, size_t value);

    constexpr int tag() const { return m_tag; }
    constexpr const std::string& value() const { return m_value; }

   
private:

    int m_tag;
    std::string m_value;

};

}

#endif