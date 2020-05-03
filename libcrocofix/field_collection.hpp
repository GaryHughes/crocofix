#ifndef crocofix_libcrocofix_field_collection_hpp
#define crocofix_libcrocofix_field_collection_hpp

#include <vector>
#include <string>
#include "field.hpp"

namespace crocofix
{

class field_collection : public std::vector<field>
{
public:

    bool set(int tag, std::string value);
    bool set(int tag, uint64_t value);
    bool set(const field& field);

};

}

#endif