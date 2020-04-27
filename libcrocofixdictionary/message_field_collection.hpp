#ifndef crocofix_libcrocofixdictionary_message_field_collection_hpp
#define crocofix_libcrocofixdictionary_message_field_collection_hpp

#include <initializer_list>
#include <vector>
#include "message_field.hpp"

namespace crocofix::dictionary
{

// This is the set of all fields for a specific FIX message.
class message_field_collection
{
public:

    using collection = std::vector<message_field>;

    message_field_collection(std::initializer_list<message_field> fields)
    : m_fields(fields)
    {
    }

    collection::const_iterator begin() const { return m_fields.begin(); }
    collection::const_iterator end() const { return m_fields.end(); }
    collection::size_type size() const { return m_fields.size(); }
    const message_field& operator[](size_t tag) const { return m_fields[tag]; }

private:

    collection m_fields;

};

}

#endif