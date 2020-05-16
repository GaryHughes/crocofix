#ifndef crocofix_libcrocofix_test_reader_hpp
#define crocofix_libcrocofix_test_reader_hpp

#include <libcrocofix/reader.hpp>

namespace crocofix
{

class test_reader : public reader
{
public:

    test_reader(blocking_queue<message>& messages)
    :   m_messages(messages)
    {
    }

    void read_async(reader::message_callback callback) override
    {
        m_callback = callback;
    }

    void deliver(message& message)
    {
        m_messages.enqueue(message);
        m_callback(message);
    }

private:

    reader::message_callback m_callback;
    blocking_queue<message>& m_messages;

};

}

#endif