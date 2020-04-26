#ifndef crocofix_libcrocofixdictionary_message_hpp
#define crocofix_libcrocofixdictionary_message_hpp

#include <string>

namespace crocofix::dictionary
{

class message
{
public:

    message(std::string_view name, std::string_view msg_type, std::string_view category, std::string_view added, std::string_view synopsis)
    : m_name(name), m_msg_type(msg_type), m_category(category), m_added(added), m_synopsis(synopsis) 
    {
    }

    constexpr const std::string_view& name() const { return m_name; }
    constexpr const std::string_view& msg_type() const { return m_msg_type; }
    constexpr const std::string_view& category() const { return m_category; }
    constexpr const std::string_view& added() const { return m_added; }
    constexpr const std::string_view& synopsis() const { return m_synopsis; }

private:

    std::string_view m_name;
    std::string_view m_msg_type;
    std::string_view m_category;
    std::string_view m_added;
    std::string_view m_synopsis;

};

}

#endif

