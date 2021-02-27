#ifndef crocofix_libcrocofix_session_state_hpp
#define crocofix_libcrocofix_session_state_hpp

#include "behaviour.hpp"
#include <string>

namespace crocofix
{

enum class session_state
{
    disconnected,
    connected,
    logging_on,
    resending,
    logged_on,
    resetting
};

std::string to_string(session_state state);

}

#endif