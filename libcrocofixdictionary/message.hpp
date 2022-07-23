#ifndef crocofix_libcrocofixdictionary_message_hpp
#define crocofix_libcrocofixdictionary_message_hpp

#include <string>
#include "message_field_collection.hpp"
#include "pedigree.hpp"

namespace crocofix::dictionary
{

class message
{
public:

    message(std::string_view name, std::string_view msg_type, std::string_view category, std::string_view synopsis, const pedigree& pedigree, std::initializer_list<message_field> fields)
    : m_name(name), m_msg_type(msg_type), m_category(category), m_synopsis(synopsis), m_pedigree(pedigree), m_fields(fields)  
    {
    }

    constexpr const std::string_view& name() const noexcept { return m_name; }
    constexpr const std::string_view& msg_type() const noexcept { return m_msg_type; }
    constexpr const std::string_view& category() const noexcept { return m_category; }
    constexpr const std::string_view& synopsis() const noexcept { return m_synopsis; }
    constexpr const dictionary::pedigree& pedigree() const noexcept { return m_pedigree; }

    const crocofix::dictionary::message_field_collection& fields() const noexcept { return m_fields; }

private:

    std::string_view m_name;
    std::string_view m_msg_type;
    std::string_view m_category;
    std::string_view m_synopsis;
    dictionary::pedigree m_pedigree;
    message_field_collection m_fields;

};

}

#endif

