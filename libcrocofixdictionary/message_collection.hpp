#ifndef crocofix_libcrocofixdictionary_message_collection_hpp
#define crocofix_libcrocofixdictionary_message_collection_hpp

#include <initializer_list>
#include <vector>
#include <map>
#include "message.hpp"

namespace crocofix::dictionary
{

class message_collection
{
public:

    using collection = std::vector<message>;
    using msg_type_map = std::map<std::string, const message*>;

    message_collection(std::initializer_list<message> messages)
    {
        m_messages = messages;
    
        for (const auto& message : m_messages)
        {
            m_messages_by_msg_type[std::string(message.msg_type())] = &message;    
        }
    }

    collection::const_iterator begin() const { return m_messages.begin(); }
    collection::const_iterator end() const { return m_messages.end(); }
    collection::size_type size() const { return m_messages.size(); }
    const message& operator[](size_t index) const { return m_messages[index]; }
    const message& operator[](const std::string& msg_type) const 
    { 
        auto message = m_messages_by_msg_type.find(msg_type);

        if (message == m_messages_by_msg_type.end()) {
            throw std::out_of_range("unknown msg_type " + msg_type);
        }   

        return *message->second; 
    }

    const std::string_view& name_of_message(const std::string& msg_type) const noexcept
    {
        auto message = m_messages_by_msg_type.find(msg_type);

        if (message == m_messages_by_msg_type.end()) {
            static const std::string_view empty_string {""};
            return empty_string;
        }

        return message->second->name();
    }

private:

    collection m_messages;
    msg_type_map m_messages_by_msg_type;

};

}

#endif