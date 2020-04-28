#include "field.hpp"

namespace crocofix
{

field::field(int tag, std::string_view value)
: m_tag(tag), m_value(value)
{
}

}