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

message::message(bool populate_header, std::initializer_list<field> fields)
{
    if (populate_header) {
        m_fields = {
            field( FIX_5_0SP2::field::BeginString::Tag,    std::string("") ),
            field( FIX_5_0SP2::field::BodyLength::Tag,     std::string("") ),
            field( FIX_5_0SP2::field::MsgType::Tag,        std::string("") ),
            field( FIX_5_0SP2::field::SenderCompID::Tag,   std::string("") ),
            field( FIX_5_0SP2::field::TargetCompID::Tag,   std::string("") ),
            field( FIX_5_0SP2::field::MsgSeqNum::Tag,      std::string("") ),
            field( FIX_5_0SP2::field::SendingTime::Tag,    std::string("") )
        };
    }

    for (const auto& field : fields) {
        m_fields.set(field, set_operation::replace_first_or_append);
    }
}

void message::reset()
{
    m_fields.clear();
    m_decode_checksum = 0;
    m_decode_checksum_valid = false;
}

message::decode_result message::decode(std::string_view buffer) // NOLINT(readability-function-cognitive-complexity, cppcoreguidelines-pro-bounds-pointer-arithmetic)
{
    const auto* current = buffer.data();
    const auto* checksum_current = buffer.data();
    const auto* end = buffer.data() + buffer.size(); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    auto complete = false;

    while (current != end)
    {
        const auto* equals = std::find(current, end, value_separator);

        if (equals == end) {
            break;
        }

        int tag = 0;
        
        auto [ptr, ec] = std::from_chars(current, equals, tag);
        
        if (ec != std::errc()) {
            throw std::out_of_range(std::string(current, equals) + " is not a valid field tag");
        }

        if (FIX_5_0SP2::fields()[tag].is_data()) 
        {
            if (m_fields.empty()) {
                throw std::runtime_error("parsed a data field with tag=" + std::to_string(tag) + " that was not preceeded by a length field");                
            }

            int length = 0;

            try {
                length = std::stoi(m_fields.rbegin()->value());
            }
            catch (std::exception&) {
                throw std::runtime_error("parsed a data field with tag=" + std::to_string(tag) + " but the preceeding field value was not a valid numeric length");
            }

            if (equals + length + 1 >= end) { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                break;
            }

            const std::string_view value(equals + 1, length); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

            if (*(equals + length + 1) != field_separator) { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                throw std::runtime_error("parsed a data field wtih tag=" + std::to_string(tag) + " but the field did not have a trailing field separator");
            }

            m_fields.emplace_back(tag, value);
            // Only update current when we have a complete field so the return value is correct.
            // +1 for the field separator, +1 to move to the first character of the next tag.
            current = equals + length + 2; // // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }
        else
        {
            const auto* delimiter = std::find(equals + 1, end, field_separator); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

            if (delimiter == end) {
                break;
            }

            const std::string_view value(equals + 1, std::distance(equals, delimiter) - 1); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            m_fields.emplace_back(tag, value);
            // Only update current when we have a complete field so the return value is correct.
            // +1 to move past the delimiter to the start of the next tag.
            current = delimiter + 1; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }

        if (tag == FIX_5_0SP2::field::CheckSum::Tag) {
            complete = true;
            break;
        }

        // We only calculate the checksum up to the checksum field itself.
        checksum_current = current;
    }

    m_decode_checksum += std::reduce(buffer.begin(), checksum_current);

    if (complete) {
        m_decode_checksum %= 256;
        m_decode_checksum_valid = true;
    }

    return { .consumed = static_cast<size_t>(std::distance(&*buffer.begin(), current)), .complete = complete };
}

std::span<char>::pointer message::encode(std::span<char>::pointer current, 
                                         std::span<char>::pointer end, 
                                         const field& field)
{
    auto [ptr, ec] = std::to_chars(current, end, field.tag());

    if (ec != std::errc()) {
        return nullptr;
    }

    current = ptr;

    if (current >= end) {
        return nullptr;
    }

    *current++ = value_separator; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    if (current >= end) {
        return nullptr;
    }

    auto value_length = field.value().length();

    if (current + value_length >= end) { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return nullptr;
    }

    std::memcpy(current, field.value().data(), value_length);

    current += value_length; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    *current++ = field_separator; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    return current;
}

size_t message::encode(std::span<char> buffer, int options)
{
    if ((options & encode_options::set_body_length) != 0) {    
        fields().set(crocofix::FIX_5_0SP2::field::BodyLength::Tag, calculate_body_length());
    }

    auto* current = buffer.data();
    auto* end = buffer.data() + buffer.size(); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    for (const auto& field : fields())
    {
        if (field.tag() == FIX_5_0SP2::field::CheckSum::Tag && ((options & encode_options::set_checksum) != 0)) 
        {
            auto checksum = calculate_checksum(std::string_view(buffer.data(), current - buffer.data()));
            auto checksum_field = crocofix::field(FIX_5_0SP2::field::CheckSum::Tag, format_checksum(checksum)); 
                        
            if (fields().set(checksum_field)) 
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
    uint32_t length {0};

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

uint32_t message::calculate_checksum() const
{
    if (m_decode_checksum_valid) {
        return m_decode_checksum;
    }

    throw std::runtime_error("NO DECODE CHECKSUM");

    return 0;
}

std::string message::format_checksum(uint32_t checksum)
{
    auto buffer = std::to_string(checksum);

    if (buffer.length() > 3) {
        throw std::runtime_error("Cannot format CheckSum that is greater than 3 digits '" + buffer + "'");
    }

    static const char* padding = "000";

    return padding + buffer.length() + buffer; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

uint32_t message::calculate_checksum(std::string_view buffer)
{
    return std::reduce(buffer.begin(), buffer.end(), 0) % 256;
}

const std::string& message::BeginString() const
{
    // TODO - maybe cache this but wait until we figure out an invalidation policy for message reuse etc
    auto field = std::ranges::find_if(m_fields, [](const auto& field) { return field.tag() == FIX_5_0SP2::field::BeginString::Tag; });

    if (field == m_fields.end()) {
        throw std::runtime_error("message does not have a BeginString field");
    }

    return field->value();
}

const std::string& message::SenderCompID() const
{
    // TODO - maybe cache this but wait until we figure out an invalidation policy for message reuse etc
    auto field = std::ranges::find_if(m_fields, [](const auto& field) { return field.tag() == FIX_5_0SP2::field::SenderCompID::Tag; });

    if (field == m_fields.end()) {
        throw std::runtime_error("message does not have a SenderCompID field");
    }

    return field->value();
}

const std::string& message::TargetCompID() const
{
    // TODO - maybe cache this but wait until we figure out an invalidation policy for message reuse etc
    auto field = std::ranges::find_if(m_fields, [](const auto& field) { return field.tag() == FIX_5_0SP2::field::TargetCompID::Tag; });

    if (field == m_fields.end()) {
        throw std::runtime_error("message does not have a TargetCompID field");
    }

    return field->value();
}

const std::string& message::MsgType() const
{
    // TODO - maybe cache this but wait until we figure out an invalidation policy for message reuse etc
    auto field = std::ranges::find_if(m_fields, [](const auto& field) { return field.tag() == FIX_5_0SP2::field::MsgType::Tag; });

    if (field == m_fields.end()) {
        throw std::runtime_error("message does not have a MsgType field");
    }

    return field->value();
}

uint32_t message::MsgSeqNum() const
{
    auto field = std::ranges::find_if(m_fields, [](const auto& field) { return field.tag() == FIX_5_0SP2::field::MsgSeqNum::Tag; });

    if (field == m_fields.end()) {
        throw std::runtime_error("message does not have a MsgSeqNum field");
    }

    return std::stoi(field->value());
}

bool message::PossDupFlag() const
{
    auto field = std::ranges::find_if(m_fields, [](const auto& field) { return field.tag() == FIX_5_0SP2::field::PossDupFlag::Tag; });

    if (field == m_fields.end()) {
       return false;
    }

    // TODO - bust this out into a get boolean method probably on field collection

    if (field->value() == "N") {
        return false;
    }

    if (field->value() == "Y") {
        return true;
    }

    throw std::runtime_error("field does not contain a valid boolean value " + std::to_string(field->tag()) + "=" + field->value());
}

bool message::GapFillFlag() const
{
    auto field = std::ranges::find_if(m_fields, [](const auto& field) { return field.tag() == FIX_5_0SP2::field::GapFillFlag::Tag; });

    if (field == m_fields.end()) {
       return false;
    }

    // TODO - bust this out into a get boolean method probably on field collection

    if (field->value() == "N") {
        return false;
    }

    if (field->value() == "Y") {
        return true;
    }

    throw std::runtime_error("field does not contain a valid boolean value " + std::to_string(field->tag()) + "=" + field->value());
}

bool message::ResetSeqNumFlag() const
{
    auto field = std::ranges::find_if(m_fields, [](const auto& field) { return field.tag() == FIX_5_0SP2::field::ResetSeqNumFlag::Tag; });

    if (field == m_fields.end()) {
       return false;
    }

    // TODO - bust this out into a get boolean method probably on field collection

    if (field->value() == "N") {
        return false;
    }

    if (field->value() == "Y") {
        return true;
    }

    throw std::runtime_error("field does not contain a valid boolean value " + std::to_string(field->tag()) + "=" + field->value());
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
        return true; // NOLINT(readability-simplify-boolean-expr)
    }

    return false;
}

void message::pretty_print(std::ostream& stream) const
{
    // TODO - add an interface to support custom dictionaries.
    int widest_field_name {0};
    int widest_tag {0};

    for (const auto& field : fields())
    {
        auto name {FIX_5_0SP2::fields().name_of_field(field.tag())};
            
        if (name.length() > widest_field_name) {
            widest_field_name = int(name.length());
        }
        
        auto digits = int(number_of_digits(field.tag())); 

        widest_tag = std::max(widest_tag, digits);
    }

    stream << FIX_5_0SP2::messages().name_of_message(MsgType()) << "\n{\n";

    for (const auto& field : fields())
    {
        // BodyLength  (9) 145
        //    MsgType (35) A
        auto name {FIX_5_0SP2::fields().name_of_field(field.tag())};

        stream << std::setw(widest_field_name) << std::right << name 
               << std::setw(widest_tag + 4) << std::right << " (" + std::to_string(field.tag()) + ") "
               << field.value();
           
        auto name_of_value = FIX_5_0SP2::fields().name_of_value(field.tag(), field.value());

        if (!name_of_value.empty()) {
            stream << " - " << name_of_value;
        }

        stream << '\n'; 
    }

    stream << "}\n";
}

}


