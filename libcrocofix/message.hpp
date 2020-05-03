#ifndef crocofix_libcrocofix_message_hpp
#define crocofix_libcrocofix_message_hpp

#include <optional>
#include <gsl/span>
#include "field_collection.hpp"

namespace crocofix
{

class message
{
public:

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

    decode_result decode(std::string_view buffer);

    // Encode this FIX message into the supplied buffer. This method calculates 
    // and rewrites the BodyLength and CheckSum, these fields must already be present, they
    // will not be added. It does no validation of the message content/structure. 
    // Returns 0 if the buffer is not big enough.
    size_t encode(gsl::span<char> buffer);

    uint32_t calculate_body_length() const;
    
    static uint32_t calculate_checksum(std::string_view buffer);
    static std::string format_checksum(uint32_t checksum);

private:

    field_collection m_fields;

    gsl::span<char>::pointer encode(gsl::span<char>::pointer current, 
                                    gsl::span<char>::pointer end, 
                                    const field& field);

};

}

#endif