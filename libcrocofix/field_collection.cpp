#include "field_collection.hpp"
#include <algorithm>

namespace crocofix
{

field_collection::field_collection(std::initializer_list<field> fields)
:   std::vector<field>(fields)
{
}

bool field_collection::set(const field& field, set_operation operation)
{
    if (operation == set_operation::append) {
        push_back(field);
        return true;
    }

    auto existing = std::find_if(begin(), end(), [&](const auto& f) { return f.tag() == field.tag(); });

    if (existing == end()) {
        if (operation == set_operation::replace_first_or_append) {
            push_back(field);
            return true;
        }
        return false;
    }

    *existing = field;

    return true; 
}

bool field_collection::set(int tag, const char* value, set_operation operation)
{
    return set(tag, std::string(value), operation);
}

bool field_collection::set(int tag, std::string value, set_operation operation)
{
    return set(field(tag, value), operation);
}

bool field_collection::set(int tag, uint32_t value, set_operation operation)
{
    return set(tag, std::to_string(value), operation);
}

bool field_collection::set(int tag, uint64_t value, set_operation operation)
{
    return set(tag, std::to_string(value), operation);
}

bool field_collection::set(int tag, bool value, set_operation operation)
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

bool field_collection::remove(int tag, remove_operation operation)
{
    auto predicate = [=](const auto& field) { return field.tag() == tag; };

    if (operation == remove_operation::remove_first) {
        auto first = std::find_if(begin(), end(), predicate);
        if (first == end()) {
            return false;
        }
        erase(first);
        return true;
    }

    if (operation == remove_operation::remove_all) {
        auto first = std::remove_if(begin(), end(), predicate);
        if (first == end()) {
            return false;
        }
        erase(first, end());
        return true;
    }

    return false;
}

/*
void field_collection::remove_at(size_t index)
{

}

void field::collection::remove_range(size_t first_index, size_t last_index)
{

}
*/

}
