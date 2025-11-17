module;

#include <string>

export module crocofix:session_state;

export namespace crocofix
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
