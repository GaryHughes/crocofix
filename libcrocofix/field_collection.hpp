#ifndef crocofix_libcrocofix_field_collection_hpp
#define crocofix_libcrocofix_field_collection_hpp

#include <vector>
#include <string>
#include <optional>
#include "field.hpp"

namespace crocofix
{

class field_collection : public std::vector<field>
{
public:

    field_collection() {}
    field_collection(std::initializer_list<field> fields);

    bool set(int tag, std::string value, bool add_if_missing = false);
    bool set(int tag, uint64_t value, bool add_if_missing = false);
    bool set(const field& field, bool add_if_missing = false);

    std::optional<field> try_get(int tag) const noexcept;

    void remove(int tag);

};

}

#endif