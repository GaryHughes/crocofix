#ifndef crocofix_libcrocofixdictionary_message_field_hpp
#define crocofix_libcrocofixdictionary_message_field_hpp

#include "orchestration_field.hpp"
#include <string>

namespace crocofix::dictionary
{

// This is the definition of a field for a specific FIX message.
class message_field
{
public:

    message_field(const orchestration_field& field, size_t depth)
    : m_field(field),
      m_depth(depth)
    {
    }

    constexpr int tag() const noexcept { return m_field.tag(); }
    constexpr const std::string_view& name() const noexcept { return m_field.name(); }
    constexpr const std::string_view& type() const noexcept { return m_field.type(); }
    constexpr const std::string_view& synopsis() const noexcept { return m_field.synopsis(); }
    constexpr const pedigree& pedigree() const noexcept { return m_field.pedigree(); }
  
    // Nested groups are indicated using this field.
    constexpr size_t depth() const noexcept { return m_depth; }

private:

    orchestration_field m_field;
    size_t m_depth;

};

}

#endif