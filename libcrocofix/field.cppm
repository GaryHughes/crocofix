module;

#include <string>
#include <cstdint>

export module crocofix:field;

import crocofix.dictionary;

export namespace crocofix
{

enum class timestamp_format : u_int8_t
{
    seconds,
    milliseconds
};

std::string timestamp_string(timestamp_format format);

class field
{
public:

    explicit field(int tag, const dictionary::field_value& value);
  
    explicit field(int tag, std::string value);
    explicit field(int tag, std::string_view value);
    explicit field(int tag, const char* value);
    explicit field(int tag, bool value);
  
    explicit field(int tag, int8_t value);
    explicit field(int tag, int16_t value);
    explicit field(int tag, int32_t value);
    explicit field(int tag, int64_t value);

    explicit field(int tag, uint8_t value);
    explicit field(int tag, uint16_t value);
    explicit field(int tag, uint32_t value);
    explicit field(int tag, uint64_t value);

    explicit field(int tag, double value);

    [[nodiscard]] constexpr int tag() const { return m_tag; }
    [[nodiscard]] constexpr const std::string& value() const { return m_value; }

    bool operator==(const dictionary::field_value& other) const {
        return m_value == other.value();
    }

   
    // TODO: These are placeholders to satisfy sol. Either figure out how to get rid of them or make them real.
    bool operator<(const field& other) const {
        return m_value < other.m_value;
    }

    bool operator==(const field& other) const {
        return m_value == other.m_value;
    }

    // TODO - spaceship?

private:

    int m_tag;
    std::string m_value;

};

}
