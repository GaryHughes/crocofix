#include "session.hpp"
#include <functional>
#include <stdexcept>

namespace crocofix
{

session::session(reader& reader, writer& writer)
:   m_reader(reader),
    m_writer(writer)
{
     m_reader.read_async([=](crocofix::message& message) { on_message_read(message); });
}    

void session::open()
{
}

void session::close()
{
}

void session::on_message_read(crocofix::message& message)
{

}

session_state session::state() const
{
    return m_state;
}
   
void session::state(session_state state)
{
    m_state = state;
}

void session::ensure_options_are_mutable()
{
    if (state() != session_state::connected) {
        throw std::logic_error("session configuration cannot be changed while state = " + to_string(state()));
    }
}

behaviour session::logon_behaviour() const noexcept 
{ 
    return m_options.logon_behaviour(); 
}

void session::logon_behaviour(behaviour behaviour) 
{
    ensure_options_are_mutable(); 
    m_options.logon_behaviour(behaviour); 
} 

}