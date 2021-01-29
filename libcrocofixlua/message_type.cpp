#include "message_type.hpp"
#include <libcrocofix/message.hpp>

// This didn't seem to work - I've added == and < to field so we don't need it for now
// namespace sol 
// {
//     template <>
//     struct is_container<crocofix::field_collection> : std::false_type {};
// }

void initialise_message_type(sol::state& lua)
{
    // auto field_collection_type = lua.new_usertype<crocofix::field_collection>(
    //     "field_collection",
    //     "size", &crocofix::field_collection::size
    // );

    auto message_type = lua.new_usertype<crocofix::message>(
        "message",
        "reset", &crocofix::message::reset,
        "MsgType", &crocofix::message::MsgType,
        "MsgSeqNum", &crocofix::message::MsgSeqNum,
        "PossDupFlag", &crocofix::message::PossDupFlag,
        "GapFillFlag", &crocofix::message::GapFillFlag,
        "ResetSeqNumFlag", &crocofix::message::ResetSeqNumFlag,
        "is_admin", &crocofix::message::is_admin
    );

    // We need the static_cast because there are const and non const versions of fields()
    message_type["fields"] = static_cast<crocofix::field_collection&(crocofix::message::*)()>(&crocofix::message::fields);


  
}