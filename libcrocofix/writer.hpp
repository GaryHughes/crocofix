#ifndef crocofix_libcrocofix_writer_hpp
#define crocofix_libcrocofix_writer_hpp

#include "message.hpp"

namespace crocofix
{

class writer
{
public:

    virtual void write(message& message) = 0;         

};

}

#endif