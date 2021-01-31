#include "field_collection.hpp"
#include <algorithm>

namespace crocofix
{

field_collection::field_collection(std::initializer_list<field> fields)
:   std::vector<field>(fields)
{
}

bool field_collection::set(const field& field, field_operation operation)
{
    if (operation == field_operation::append) {
        push_back(field);
        return true;
    }

    auto existing = std::find_if(begin(), end(), [&](const auto& f) { return f.tag() == field.tag(); });

    if (existing == end()) {
        if (operation == field_operation::replace_first_or_append) {
            push_back(field);
            return true;
        }
        return false;
    }

    *existing = field;

    return true; 
}

bool field_collection::set(int tag, const char* value, field_operation operation)
{
    return set(tag, std::string(value), operation);
}

bool field_collection::set(int tag, std::string value, field_operation operation)
{
    return set(field(tag, value), operation);
}

bool field_collection::set(int tag, uint32_t value, field_operation operation)
{
    return set(tag, std::to_string(value), operation);
}

bool field_collection::set(int tag, uint64_t value, field_operation operation)
{
    return set(tag, std::to_string(value), operation);
}

bool field_collection::set(int tag, bool value, field_operation operation)
{
    return set(tag, value ? std::string("Y") : std::string("N"), operation);
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

/*
void field_collection::remove_at(int index)
{

}

void field::collection::remove_range(int first_index, int last_index)
{

}
*/

}
