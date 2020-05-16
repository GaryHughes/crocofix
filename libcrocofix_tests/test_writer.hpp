#ifndef crocofix_libcrocofix_test_writer_hpp
#define crocofix_libcrocofix_test_writer_hpp

#include <libcrocofix/writer.hpp>

namespace crocofix
{

class test_reader;

class test_writer : public writer
{
public:

    test_writer(test_reader& reader, blocking_queue<message>& messages)
    :   m_reader(reader),
        m_messages(messages)
    {
    }

    void write(message& message) override
    {
        m_messages.enqueue(message);
        m_reader.deliver(message);
    }         

private:

    test_reader& m_reader;
    blocking_queue<message>& m_messages;

};

}

#endif