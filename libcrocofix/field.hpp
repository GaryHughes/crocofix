#ifndef crocofix_libcrocofix_field_hpp
#define crocofix_libcrocofix_field_hpp

#include <string>
#include <libcrocofixdictionary/field_value.hpp>

namespace crocofix
{

enum class timestamp_format
{
    seconds,
    milliseconds
};

std::string timestamp_string(timestamp_format format);

class field
{
public:

    // TODO - complete these for integral types and add tests
    field(int tag, const dictionary::field_value& value);
    field(int tag, std::string value);
    field(int tag, std::string_view value);
    field(int tag, const char* value);
    field(int tag, size_t value);
    field(int tag, bool value);
    field(int tag, uint32_t value);
    field(int tag, int value);

    constexpr int tag() const { return m_tag; }
    constexpr const std::string& value() const { return m_value; }

private:

    int m_tag;
    std::string m_value;

};

}

#endif