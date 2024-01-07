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

    boost::signals2::signal<void()> closed;

    using message_callback = std::function<void(message&)>;

    virtual void read_async(message_callback callback) = 0;

    virtual void close() = 0;

};

}

#endif