#ifndef crocofix_libcrocofix_reader_hpp
#define crocofix_libcrocofix_reader_hpp

#include "message.hpp"
#include <functional>

namespace crocofix
{

class reader
{
public:

    using message_callback = std::function<void(message&)>;

    virtual void read_async(message_callback callback) = 0;

};

}

#endif