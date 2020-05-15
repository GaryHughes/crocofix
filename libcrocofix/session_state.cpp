#include "session_state.hpp"

namespace crocofix
{

const std::string to_string(session_state state)
{
    switch (state)
    {
        case session_state::connected:
            return "connected";

        case session_state::logging_on:
            return "loggin_on";

        case session_state::resending:
            return "resending";

        case session_state::logged_on:
            return "logged_on";

        case session_state::resetting:
            return "resetting";

        default:
            return "unknown";
    }    
}

}