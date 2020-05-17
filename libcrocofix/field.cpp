#include "field.hpp"

namespace crocofix
{

field::field(int tag, const dictionary::field_value& value)
: m_tag(tag), m_value(value.value())
{
}

field::field(int tag, std::string_view value)
: m_tag(tag), m_value(value)
{
}

field::field(int tag, size_t value)
:   m_tag(tag), m_value(std::to_string(value))
{
}

}