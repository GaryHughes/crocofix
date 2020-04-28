#ifndef crocofix_libcrocofix_message_hpp
#define crocofix_libcrocofix_message_hpp

#include <vector>
#include <optional>
#include "field.hpp"

namespace crocofix
{

class message
{
public:

    using field_collection = std::vector<field>;

    field_collection& fields() { return m_fields; }
    const field_collection& fields() const { return m_fields; }

    const std::string& MsgType() const;

    bool is_admin() const;


    // Decode FIX tag/value pairs and store them in this message. This does no validation of
    // the field content and does not validate the BodyLength or CheckSum. This supports
    // decoding fragmentary messages. This method is restartable, it can be called multiple
    // times with separate pieces of the same message until completion. This method does not
    // track completeness on subsequent calls so you can call it again after it has returned
    // complete=true and it will decode and store any fields it reads which may result in an
    // invalid message.
    struct decode_result
    {
        size_t consumed;
        bool complete;
    };

    decode_result decode(std::string_view data);

private:

    field_collection m_fields;

};

}

#endif