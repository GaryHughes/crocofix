#ifndef crocofix_libcrocofixdictionary_message_hpp
#define crocofix_libcrocofixdictionary_message_hpp

#include <string>
#include "message_field_collection.hpp"

namespace crocofix::dictionary
{

class message
{
public:

    message(std::string_view name, std::string_view msg_type, std::string_view category, std::string_view added, std::string_view synopsis, std::initializer_list<message_field> fields)
    : m_name(name), m_msg_type(msg_type), m_category(category), m_added(added), m_synopsis(synopsis), m_fields(fields)  
    {
    }

    constexpr const std::string_view& name() const noexcept { return m_name; }
    constexpr const std::string_view& msg_type() const noexcept { return m_msg_type; }
    constexpr const std::string_view& category() const noexcept { return m_category; }
    constexpr const std::string_view& added() const noexcept { return m_added; }
    constexpr const std::string_view& synopsis() const noexcept { return m_synopsis; }

    const crocofix::dictionary::message_field_collection& fields() const noexcept { return m_fields; }

private:

    std::string_view m_name;
    std::string_view m_msg_type;
    std::string_view m_category;
    std::string_view m_added;
    std::string_view m_synopsis;
    message_field_collection m_fields;

};

}

#endif

