#ifndef crocofix_libcrocofix_test_reader_hpp
#define crocofix_libcrocofix_test_reader_hpp

import crocofix;

namespace crocofix
{

class test_reader : public reader
{
public:

    test_reader(blocking_queue<message>& messages)
    :   m_messages(messages)
    {
    }

    void deliver(message& message)
    {
        if (!m_closed) {
            m_messages.enqueue(message);
            message_read(message);
        }
    }

    void open() override
    {
        if (m_closed)
        {
            m_closed = false;
            opened();
        }    
    }

    void close() override
    {
        if (!m_closed) {
            m_closed = true;
            closed();
        }
    }

private:

    blocking_queue<message>& m_messages;
    bool m_closed = false;

};

}

#endif