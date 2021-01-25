#include "message_type.hpp"
#include <libcrocofix/message.hpp>

void initialise_message_type(sol::state& lua)
{
    lua.new_usertype<crocofix::message>(
        "message",
        "reset", &crocofix::message::reset
    );
}