#ifndef crocofix_libcrocofixdictionary_message_collection_hpp
#define crocofix_libcrocofixdictionary_message_collection_hpp

#include <initializer_list>
#include <vector>
#include "message.hpp"


#include <iostream>

namespace crocofix::dictionary
{

class message_collection
{
public:

    using collection = std::vector<message>;

    message_collection(std::initializer_list<message> messages)
    {
        m_messages = messages;
    }

    collection::const_iterator begin() const { return m_messages.begin(); }
    collection::const_iterator end() const { return m_messages.end(); }
    collection::size_type size() const { return m_messages.size(); }
    const message& operator[](size_t index) const { return m_messages[index]; }

private:

    collection m_messages;

};

}

#endif