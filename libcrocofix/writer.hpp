#ifndef crocofix_libcrocofix_writer_hpp
#define crocofix_libcrocofix_writer_hpp

#include "message.hpp"
#include <boost/signals2.hpp>

namespace crocofix
{

class writer
{
public:

    virtual ~writer() = default;

    boost::signals2::signal<void()> closed;

    virtual void write(message& message, int options = encode_options::standard) = 0;         

    virtual void close() = 0;
    
};

}

#endif