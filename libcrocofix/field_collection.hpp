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

    void set(int tag, std::string value);
    void set(int tag, uint64_t value);
    void set(const field& field);

};

}

#endif