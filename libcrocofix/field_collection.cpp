#include "field_collection.hpp"
#include <algorithm>

namespace crocofix
{

field_collection::field_collection(std::initializer_list<field> fields)
:   std::vector<field>(fields)
{
}

bool field_collection::set(const field& field, bool add_if_missing)
{
    auto existing = std::find_if(begin(), end(), [&](const auto& f) { return f.tag() == field.tag(); });

    if (existing == end()) {
        if (add_if_missing) {
            push_back(field);
            return true;
        }
        return false;
    }

    *existing = field;

    return true; 
}

bool field_collection::set(int tag, std::string value, bool add_if_missing)
{
    return set(field(tag, value), add_if_missing);
}

bool field_collection::set(int tag, uint32_t value, bool add_if_missing)
{
    return set(tag, std::to_string(value), add_if_missing);
}

bool field_collection::set(int tag, uint64_t value, bool add_if_missing)
{
    return set(tag, std::to_string(value), add_if_missing);
}

bool field_collection::set(int tag, bool value, bool add_if_missing)
{
    return set(tag, value ? std::string("Y") : std::string("N"), add_if_missing);
}

std::optional<field> field_collection::try_get(int tag) const noexcept
{
    auto field = std::find_if(begin(), end(), [&](const auto& field) { return field.tag() == tag; });

    if (field == end()) {
        return std::nullopt;
    }

    return *field;
}

void field_collection::remove(int tag)
{
    erase(std::remove_if(begin(), end(), [=](const auto& field) { return field.tag() == tag; }));
}

}
