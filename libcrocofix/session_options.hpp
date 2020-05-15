#ifndef crocofix_libcrocofix_session_options_hpp
#define crocofix_libcrocofix_session_options_hpp

#include "behaviour.hpp"

namespace crocofix
{

class session_options
{
public:

    behaviour logon_behaviour() const noexcept { return m_logon_behaviour; }
    void logon_behaviour(behaviour behaviour) noexcept { m_logon_behaviour = behaviour; }    

    

private:

    behaviour m_logon_behaviour = behaviour::initiator;

};

}

#endif