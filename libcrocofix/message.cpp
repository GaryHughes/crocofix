#include "message.hpp"
#include <libcrocofixdictionary/fix50SP2_messages.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include <algorithm>
#include <charconv>

namespace crocofix
{

static constexpr const char value_separator = '=';
static constexpr const char field_separator = '\01';

message::decode_result message::decode(std::string_view data)
{
    auto current = data.begin();
    auto end = data.end();
    auto complete = false;

    while (current != end)
    {
        auto equals = std::find(current, end, value_separator);

        if (equals == end) {
            break;
        }

        int tag;
        
        auto [ptr, ec] = std::from_chars(current, equals, tag);
        
        if (ec != std::errc()) {
            throw std::out_of_range(std::string(current, equals) + " is not a valid field tag");
        }

        auto delimiter = std::find(equals + 1, end, field_separator);

        if (delimiter == end) {
            break;
        }

        std::string_view value(equals + 1, std::distance(equals, delimiter) - 1);

        m_fields.emplace_back(tag, value);

        current = delimiter + 1;

        if (tag == FIX_5_0SP2::field::CheckSum::Tag) {
            complete = true;
            break;
        }
    }

    return { static_cast<size_t>(std::distance(data.begin(), current)), complete };
}

const std::string& message::MsgType() const
{
    // TODO - maybe cache this but wait until we figure out an invalidation policy for message reuse etc
    auto field = std::find_if(m_fields.begin(), 
                              m_fields.end(), 
                              [](const auto& field) { return field.tag() == FIX_5_0SP2::field::MsgType::Tag; });

    if (field == m_fields.end()) {
        throw std::runtime_error("message does not have a MsgType field");
    }

    return field->value();
}

bool message::is_admin() const
{
    auto type = MsgType();

    if (type == FIX_5_0SP2::message::Logon::MsgType ||
        type == FIX_5_0SP2::message::Heartbeat::MsgType ||
        type == FIX_5_0SP2::message::TestRequest::MsgType ||
        type == FIX_5_0SP2::message::ResendRequest::MsgType ||
        type == FIX_5_0SP2::message::Reject::MsgType ||
        type == FIX_5_0SP2::message::SequenceReset::MsgType ||
        type == FIX_5_0SP2::message::Logout::MsgType ||
        type == FIX_5_0SP2::message::Logon::MsgType ||
        type == FIX_5_0SP2::message::XMLnonFIX::MsgType)
    {
        return true;
    }

    return false;
}

}


