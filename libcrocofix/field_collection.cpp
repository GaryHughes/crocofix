module;

#include <algorithm>
#include <stdexcept>
#include <ranges>
#include <vector>
#include <cstdint>

module crocofix;

import crocofix.dictionary;

namespace crocofix
{

field_collection::field_collection(std::initializer_list<field> fields)
:   std::vector<field>(fields)
{
}

bool field_collection::set(const field& field, set_operation operation) // NOLINT(readability-convert-member-functions-to-static)
{
    if (operation == set_operation::append) {
        push_back(field);
        return true;
    }

    auto existing = std::ranges::find_if(begin(), end(), [&](const auto& current_field) { return current_field.tag() == field.tag(); });

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
    return set(field(tag, std::move(value)), operation);
}

bool field_collection::set(int tag, uint32_t value, set_operation operation)
{
    return set(tag, std::to_string(value), operation);
}

bool field_collection::set(int tag, uint64_t value, set_operation operation)
{
    return set(tag, std::to_string(value), operation);
}

bool field_collection::set(int tag, int64_t value, set_operation operation)
{
    return set(tag, std::to_string(value), operation);
}

bool field_collection::set(int tag, bool value, set_operation operation)
{
    return set(tag, value ? std::string("Y") : std::string("N"), operation);
}

bool field_collection::set(int tag, double value, set_operation operation)
{
    return set(tag, std::to_string(value), operation);
}

bool field_collection::set(int tag, const dictionary::field_value& value, set_operation operation)
{
    return set(tag, std::string(value.value()), operation);
}

field field_collection::get(int tag) const
{
    auto result = try_get(tag);

    if (!result.has_value()) {
        throw std::out_of_range("field collection does not contain a field with Tag " + std::to_string(tag));
    }

    return result.value();
}


std::optional<field> field_collection::try_get(int tag) const noexcept
{
    auto field = std::ranges::find_if(begin(), end(), [&](const auto& field) { return field.tag() == tag; });

    if (field == end()) {
        return std::nullopt;
    }

    return *field;
}

field field_collection::try_get_or_default(int tag, const dictionary::field_value& default_value) const
{
    auto value = try_get(tag);

    if (value.has_value()) {
        return value.value();
    }

    return field(tag, default_value);
}

field field_collection::try_get_or_default(int tag, const field& default_value) const
{
    auto value = try_get(tag);

    if (value.has_value()) {
        return value.value();
    }

    return default_value;
}

field field_collection::try_get_or_default(int tag, uint32_t default_value) const
{
    auto value = try_get(tag);

    if (value.has_value()) {
        return value.value();
    }

    return field(tag, default_value);
}

field field_collection::try_get_or_default(int tag, const std::string& default_value) const
{
    auto value = try_get(tag);

    if (value.has_value()) {
        return value.value();
    }

    return field(tag, default_value);
}

bool field_collection::remove(int tag, remove_operation operation)
{
    auto predicate = [=](const auto& field) { return field.tag() == tag; };

    if (operation == remove_operation::remove_first) {
        auto first = std::ranges::find_if(begin(), end(), predicate);
        if (first == end()) {
            return false;
        }
        erase(first);
        return true;
    }

    if (operation == remove_operation::remove_all) {
        auto range = std::ranges::remove_if(begin(), end(), predicate);
        if (range.begin() == end()) {
            return false;
        }
        erase(range.begin(), end());
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
