#include "types.hpp"
#include <libcrocofix/message.hpp>

// This didn't seem to work - I've added == and < to field so we don't need it for now
// namespace sol 
// {
//     template <>
//     struct is_container<crocofix::field_collection> : std::false_type {};
// }

namespace crocofix
{

void define_lua_types(sol::state& lua)
{
    lua.new_enum(
        "set_operation",
		"replace_first", crocofix::set_operation::replace_first,
		"replace_first_or_append", crocofix::set_operation::replace_first_or_append,
		"append", crocofix::set_operation::append
    );

    lua.new_enum(
        "remove_operation",
        "remove_first", crocofix::remove_operation::remove_first,
        "remove_all", crocofix::remove_operation::remove_all
    );

    auto field_collection_type = lua.new_usertype<crocofix::field_collection>(
        "field_collection",
        "size", &crocofix::field_collection::size,
        "try_get", &crocofix::field_collection::try_get,
        // There are various overloads of set for different value types, they all defer to string to just make that available in lua.
        "set", static_cast<bool(crocofix::field_collection::*)(int, std::string, crocofix::set_operation)>(&crocofix::field_collection::set),
        "remove", &crocofix::field_collection::remove
    );

    auto message_type = lua.new_usertype<crocofix::message>(
        "message",
        "reset", &crocofix::message::reset,
        "MsgType", &crocofix::message::MsgType,
        "MsgSeqNum", &crocofix::message::MsgSeqNum,
        "PossDupFlag", &crocofix::message::PossDupFlag,
        "GapFillFlag", &crocofix::message::GapFillFlag,
        "ResetSeqNumFlag", &crocofix::message::ResetSeqNumFlag,
        "is_admin", &crocofix::message::is_admin,
        // We need the static_cast because there are const and non const versions of fields()
        "fields", static_cast<crocofix::field_collection&(crocofix::message::*)()>(&crocofix::message::fields)
    );
}

}