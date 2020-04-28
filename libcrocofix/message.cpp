#include "message.hpp"
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

        std::string_view value(equals + 1, std::distance(equals, delimiter));

        m_fields.emplace_back(tag, value);

        current = delimiter + 1;

        if (tag == 10) {
            complete = true;
            break;
        }
    }

    return { static_cast<size_t>(std::distance(data.begin(), current)), complete };
}

}


