#ifndef crocofix_libcrocofixdictionary_orchestration_hpp
#define crocofix_libcrocofixdictionary_orchestration_hpp

#include "message_collection.hpp"
#include "orchestration_field_collection.hpp"

namespace crocofix::dictionary
{

class orchestration
{
public:

    orchestration(const message_collection& messages,
                  const orchestration_field_collection& fields)
    :   m_messages(messages),
        m_fields(fields)
    {
    }

    const message_collection& messages() const noexcept { return m_messages; }
    const orchestration_field_collection& fields() const noexcept { return m_fields; }

private:

    const message_collection& m_messages;
    const orchestration_field_collection& m_fields;

};

}

#endif