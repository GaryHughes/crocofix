#include "field_collection.hpp"
#include <algorithm>

namespace crocofix
{

void field_collection::set(const field& field)
{
    auto existing = std::find_if(begin(), end(), [&](const auto& f) { return f.tag() == field.tag(); });

    if (existing == end()) {
        return;
    }

    *existing = field; 
}

void field_collection::set(int tag, std::string value)
{
    set(field(tag, value));
}

void field_collection::set(int tag, uint64_t value)
{
    set(tag, std::to_string(value));
}

}
