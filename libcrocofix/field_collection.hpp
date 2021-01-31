#ifndef crocofix_libcrocofix_field_collection_hpp
#define crocofix_libcrocofix_field_collection_hpp

#include <vector>
#include <string>
#include <optional>
#include "field.hpp"

namespace crocofix
{

enum class field_operation
{
    // Replace the first occurrence of a field with this tag. 
    // If there is no field with this tag do nothing.
    replace_first,
    // Replace the first occurrence of a field with this tag. 
    // If there is no field with this tag add a field with this tag and value.
    replace_first_or_append,
    // Append a new field with this tag and value. 
    append
};

class field_collection : public std::vector<field>
{
public:

    field_collection() {}
    field_collection(std::initializer_list<field> fields);

    

    bool set(int tag, const char* value, field_operation operation = field_operation::replace_first); 
    bool set(int tag, std::string value, field_operation operation = field_operation::replace_first);
    bool set(int tag, uint32_t value, field_operation operation = field_operation::replace_first);
    bool set(int tag, uint64_t value, field_operation operation = field_operation::replace_first);
    bool set(int tag, bool value, field_operation operation = field_operation::replace_first);
    bool set(const field& field, field_operation operation = field_operation::replace_first);

    std::optional<field> try_get(int tag) const noexcept;

    void remove(int tag); // remove_first
    // void remove_at(int index);
    // void remove_range(int first_index, int last_index);

};

}

#endif