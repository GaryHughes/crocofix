#ifndef crocofix_libcrocofixdictionary_field_value_hpp
#define crocofix_libcrocofixdictionary_field_value_hpp

#include <string>

namespace crocofix::dictionary
{

class field_value
{
public:

    constexpr field_value(std::string_view name, const char* value) noexcept
    :   m_name(name), m_value(value)
    {
    }

    constexpr const std::string_view& name() const noexcept { return m_name; }
    constexpr const std::string_view& value() const noexcept { return m_value; }

private:

    std::string_view m_name;
    std::string_view m_value;

};

}

#endif