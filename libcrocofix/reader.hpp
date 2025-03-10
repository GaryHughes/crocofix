#ifndef crocofix_libcrocofix_reader_hpp
#define crocofix_libcrocofix_reader_hpp

#include "message.hpp"
#include <functional>
#include <boost/signals2.hpp>

namespace crocofix
{

class reader
{
public:

    virtual ~reader() = default;

    boost::signals2::signal<void()> opened;
    boost::signals2::signal<void()> closed;
    boost::signals2::signal<void(message&)> message_read;

    virtual void open() = 0;
    virtual void close() = 0;

//protected:

    // This is a helper primarily for the Python binding so we can conveniently fire
    // the boost signal via a regular method call.
    void dispatch_message_read(message& message)
    {
        message_read(message);
    }

};

}

#endif