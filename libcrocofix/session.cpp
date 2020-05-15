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

void session::logon_behaviour(behaviour behaviour) noexcept
{
    ensure_options_are_mutable(); 
    m_options.logon_behaviour(behaviour); 
} 

const std::string& session::sender_comp_id() const noexcept
{
    return m_options.sender_comp_id();
}

void session::sender_comp_id(const std::string& sender_comp_id)
{
    ensure_options_are_mutable();
    m_options.sender_comp_id(sender_comp_id);
}    

const std::string& session::target_comp_id() const noexcept
{
    return m_options.target_comp_id();
}

void session::target_comp_id(const std::string& target_comp_id) noexcept
{
    ensure_options_are_mutable();
    m_options.target_comp_id(target_comp_id);
}

}