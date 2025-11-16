module;

#include <boost/signals2.hpp>

export module crocofix:writer;

import :message;

export namespace crocofix
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
