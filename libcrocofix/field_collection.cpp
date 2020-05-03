#include "field_collection.hpp"
#include <algorithm>

namespace crocofix
{

bool field_collection::set(const field& field)
{
    auto existing = std::find_if(begin(), end(), [&](const auto& f) { return f.tag() == field.tag(); });

    if (existing == end()) {
        return false;
    }

    *existing = field;

    return true; 
}

bool field_collection::set(int tag, std::string value)
{
    return set(field(tag, value));
}

bool field_collection::set(int tag, uint64_t value)
{
    return set(tag, std::to_string(value));
}

}
