#ifndef crocofix_libcrocofix_test_writer_hpp
#define crocofix_libcrocofix_test_writer_hpp

#include <libcrocofix/writer.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include "test_scheduler.hpp"

#include <iostream>

namespace crocofix
{

class test_reader;

class test_writer : public writer
{
public:

    test_writer(test_reader& reader, 
                blocking_queue<message>& messages, 
                test_scheduler& scheduler)
    :   m_reader(reader),
        m_messages(messages),
        m_scheduler(scheduler)
    {
    }

    void write(message& outgoing, int options = encode_options::standard) override
    {
        if (m_closed) {
            return;
        }

        // TODO message::encode doesn't add this yet
        if (options & encode_options::set_checksum) {
            outgoing.fields().set(FIX_5_0SP2::field::CheckSum::Tag, 0, true);
        }

        auto encode_result = outgoing.encode(m_buffer, options);
        
        if (encode_result == 0) {
            throw std::runtime_error("failed to encode message");
        }

        message decoded;    

        auto decode_result = decoded.decode(std::string_view(&m_buffer[0], encode_result));

        if (!decode_result.complete) {
            throw std::runtime_error("failed to decode message");
        }
            
        m_messages.enqueue(decoded);
        m_scheduler.schedule([=]() {
            message copy{decoded};
            this->m_reader.deliver(copy);
        });
    }

    void close() override
    {
        m_closed = true;
    }         

private:

    test_reader& m_reader;
    blocking_queue<message>& m_messages;
    test_scheduler& m_scheduler;
    bool m_closed = false;
    std::array<char, 8192> m_buffer;

};

}

#endif