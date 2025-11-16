module;

#include <ctime>
#include <iomanip>
#include <chrono>
#include <boost/lexical_cast.hpp>
#include <libcrocofixdictionary/field_value.hpp>

module crocofix;

namespace crocofix
{

char const * const seconds_format =  "%Y%m%d-%H:%M:%S";

std::string timestamp_string(timestamp_format format)
{
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);

    std::tm time {};
    gmtime_r(&now_time_t, &time);

    auto value = boost::lexical_cast<std::string>(std::put_time(&time, seconds_format));

    switch (format)
    {
        case timestamp_format::seconds:
            break;

        case timestamp_format::milliseconds:
        {
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
            auto buffer = std::to_string(milliseconds.count());
            static const char* padding = "000";
            value += "." + ((padding + buffer.length()) + buffer); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            break;
        }

        default:
            throw "unsupported timestamp format " + std::to_string(static_cast<int>(format));
    };
  
    return value;
}

field::field(int tag, const dictionary::field_value& value)
: m_tag(tag), m_value(value.value())
{
}

field::field(int tag, std::string value)
: m_tag(tag), m_value(std::move(value))
{
}

field::field(int tag, std::string_view value)
: m_tag(tag), m_value(value)
{
}

field::field(int tag, const char* value)
: m_tag(tag), m_value(value)
{
}

field::field(int tag, bool value)
:   m_tag(tag), m_value(value ? std::string("Y") : std::string("N"))
{
}

field::field(int tag, int8_t value) // NOLINT(bugprone-easily-swappable-parameters)
:   m_tag(tag), m_value(std::to_string(value))
{
}

field::field(int tag, int16_t value) // NOLINT(bugprone-easily-swappable-parameters)
:   m_tag(tag), m_value(std::to_string(value))
{
}

field::field(int tag, int32_t value) // NOLINT(bugprone-easily-swappable-parameters)
:   m_tag(tag), m_value(std::to_string(value))
{
}

field::field(int tag, int64_t value) // NOLINT(bugprone-easily-swappable-parameters)
:   m_tag(tag), m_value(std::to_string(value))
{
}


field::field(int tag, uint8_t value) // NOLINT(bugprone-easily-swappable-parameters)
:   m_tag(tag), m_value(std::to_string(value))
{
}

field::field(int tag, uint16_t value) // NOLINT(bugprone-easily-swappable-parameters)
:   m_tag(tag), m_value(std::to_string(value))
{
}

field::field(int tag, uint32_t value) // NOLINT(bugprone-easily-swappable-parameters)
:   m_tag(tag), m_value(std::to_string(value))
{
}

field::field(int tag, uint64_t value) // NOLINT(bugprone-easily-swappable-parameters)
:   m_tag(tag), m_value(std::to_string(value))
{
}

field::field(int tag, double value) // NOLINT(bugprone-easily-swappable-parameters)
:   m_tag(tag), m_value(std::to_string(value)) // TODO - fix the formatting, it adds trailing zeros.
{
}

}