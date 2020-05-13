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

        if (tag < FIX_5_0SP2::fields().size() && FIX_5_0SP2::fields()[tag].is_data()) 
        {
            if (m_fields.empty()) {
                throw std::runtime_error("parsed a data field with tag=" + std::to_string(tag) + " that was not preceeded by a length field");                
            }

            int length;

            try {
                length = std::stoi(m_fields.rbegin()->value());
            }
            catch (std::exception&) {
                throw std::runtime_error("parsed a data field with tag=" + std::to_string(tag) + " but the preceeding field value was not a valid numeric length");
            }

            if (equals + length + 1 >= end) {
                break;
            }

            std::string_view value(equals + 1, length);

            if (*(equals + length + 1) != field_separator) {
                throw std::runtime_error("parsed a data field wtih tag=" + std::to_string(tag) + " but the field did not have a trailing field separator");
            }

            m_fields.emplace_back(tag, value);
            // Only update current when we have a complete field so the return value is correct.
            // +1 for the field separator, +1 to move to the first character of the next tag.
            current = equals + length + 2; 
        }
        else
        {
            auto delimiter = std::find(equals + 1, end, field_separator);

            if (delimiter == end) {
                break;
            }

            std::string_view value(equals + 1, std::distance(equals, delimiter) - 1);
            m_fields.emplace_back(tag, value);
            // Only update current when we have a complete field so the return value is correct.
            // +1 to move past the delimiter to the start of the next tag.
            current = delimiter + 1;
        }

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
    // TODO - add an interface to support custom dictionaries.
    size_t widest_field_name {0};
    size_t widest_tag {0};

    for (const auto& field : fields())
    {
        auto name {FIX_5_0SP2::fields().name_of_field(field.tag())};
            
        if (name.length() > widest_field_name) {
            widest_field_name = name.length();
        }
        
        auto digits = number_of_digits(field.tag()); 

        if (digits > widest_tag) {
            widest_tag = digits;
        }
    }

    os << FIX_5_0SP2::messages().name_of_message(MsgType()) << "\n{\n";

    for (const auto& field : fields())
    {
        // BodyLength  (9) 145
        //    MsgType (35) A
        auto name {FIX_5_0SP2::fields().name_of_field(field.tag())};

        os << std::setw(widest_field_name) << std::right << name 
           << std::setw(widest_tag + 4) << std::right << " (" + std::to_string(field.tag()) + ") "
           << field.value();
           
        auto name_of_value = FIX_5_0SP2::fields().name_of_value(field.tag(), field.value());

        if (!name_of_value.empty()) {
            os << " - " << name_of_value;
        }

        os << '\n'; 
    }

    os << "}\n";
}

}


