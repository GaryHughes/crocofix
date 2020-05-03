#include "message.hpp"
#include <libcrocofixdictionary/fix50SP2_messages.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include <libcrocofixutility/number_of_digits.hpp>
#include <algorithm>
#include <charconv>
#include <numeric>
#include <cstring>
#include <iomanip>

namespace crocofix
{

static constexpr const char value_separator = '=';
static constexpr const char field_separator = '\01';

message::decode_result message::decode(std::string_view buffer)
{
    auto current = buffer.data();
    auto end = buffer.data() + buffer.size();
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

    return { static_cast<size_t>(std::distance(&*buffer.begin(), current)), complete };
}

gsl::span<char>::pointer message::encode(gsl::span<char>::pointer current, gsl::span<char>::pointer end, const field& field)
{
    auto [ptr, ec] = std::to_chars(current, end, field.tag());

    if (ec != std::errc()) {
        return nullptr;
    }

    current = ptr;

    if (current >= end) {
        return nullptr;
    }

    *current++ = value_separator;

    if (current >= end) {
        return nullptr;
    }

    auto value_length = field.value().length();

    if (current + value_length >= end) {
        return nullptr;
    }

    std::memcpy(current, field.value().data(), value_length);

    current += value_length;

    *current++ = field_separator;

    return current;
}

size_t message::encode(gsl::span<char> buffer)
{
    // TODO - make this optional
    auto body_length = calculate_body_length();

    fields().set(crocofix::FIX_5_0SP2::field::BodyLength::Tag, body_length);

    auto current = buffer.data();
    auto end = buffer.data() + buffer.size();

    for (const auto& field : fields())
    {
        if (field.tag() == FIX_5_0SP2::field::CheckSum::Tag) 
        {
            auto checksum = calculate_checksum(std::string_view(buffer.data(), current - buffer.data()));
            auto checksum_field = crocofix::field(FIX_5_0SP2::field::CheckSum::Tag, format_checksum(checksum)); 
                        
            // If there is no BodyLength CheckSim will be 0 so leave the existing value in place. This will be
            // an invalid message but we need this facility for testing.
            if (checksum > 0 && fields().set(checksum_field)) 
            {
                current = encode(current, end, checksum_field);

                if (current == nullptr) {
                    return 0;
                }

                break;
            }
        }

        current = encode(current, end, field);

        if (current == nullptr) {
            return 0;
        }
    }

    return current - buffer.data();
}

uint32_t message::calculate_body_length() const
{
    bool passed_body_length {false};
    int32_t length {0};

    for (const auto& field : fields())
    {
        if (field.tag() == FIX_5_0SP2::field::CheckSum::Tag) {
            break;
        }

        if (!passed_body_length) {
            if (field.tag() == FIX_5_0SP2::field::BodyLength::Tag) {
                passed_body_length = true;
            }
            continue;
        }

        length += number_of_digits(field.tag());
        length += field.value().length();
        length += 2; // field && value separators 
    }

    return length;
}

std::string message::format_checksum(uint32_t checksum)
{
    auto buffer = std::to_string(checksum);

    if (buffer.length() > 3) {
        throw std::runtime_error("Cannot format CheckSum that is greater than 3 digits '" + buffer + "'");
    }

    static const char* padding = "000";

    return padding + buffer.length() + buffer;
}

uint32_t message::calculate_checksum(std::string_view buffer)
{
    return std::reduce(buffer.begin(), buffer.end(), 0) % 256;
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

void message::pretty_print(std::ostream& os) const
{
    size_t widest_field_name {0};
    size_t widest_tag {0}; 

    for (const auto& field : fields())
    {
        if (field.tag() < FIX_5_0SP2::fields().size()) 
        {
            auto name = FIX_5_0SP2::fields()[field.tag()].name();
            
            if (name.length() > widest_field_name) {
                widest_field_name = name.length();
            }
            
            auto digits = number_of_digits(field.tag()); 

            if (digits > widest_tag) {
                widest_tag = digits;
            }
        }
    }

    os << MsgType() << " {\n";

    for (const auto& field : fields())
    {
        // BodyLength  (9) 145
        //    MsgType (35) A
        std::string name {""};

        if (field.tag() < FIX_5_0SP2::fields().size()) {
            name = FIX_5_0SP2::fields()[field.tag()].name();
        }

        os << std::setw(widest_field_name) << std::right << name 
           << std::setw(widest_tag + 4) << std::right << " (" + std::to_string(field.tag()) + ") "
           << field.value() << '\n'; 
    }

    os << "}\n";
}

}


