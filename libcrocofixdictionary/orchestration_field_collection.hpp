#ifndef crocofix_libcrocofixdictionary_orchestration_field_collection_hpp
#define crocofix_libcrocofixdictionary_orchestration_field_collection_hpp

#include <initializer_list>
#include <vector>
#include <stdexcept>
#include <limits>
#include <map>
#include "orchestration_field.hpp"

namespace crocofix::dictionary
{

// This is the set of all fields for a specific FIX orchestration. It contains the generic definition of
// the field. It does not contain message specific definitions.
class orchestration_field_collection
{
public:

    using collection = std::vector<orchestration_field>;
    using offset_type = uint16_t;

    // This assumes that fields are sorted in ascending order.
    orchestration_field_collection(std::initializer_list<offset_type> field_offsets, std::initializer_list<orchestration_field> fields)
    :   m_field_offsets(field_offsets),
        m_fields(fields)
    {
        if (m_field_offsets.size() < m_fields.size()) {
            throw std::invalid_argument("field_offsets.size() " + std::to_string(field_offsets.size()) +
                                        " must be >= fields.size()" + std::to_string(fields.size()));
        }

        auto max_tag = m_fields.rbegin()->tag();
        
        if (max_tag > std::numeric_limits<offset_type>::max()) {
            throw std::invalid_argument("the maximum field tag " + std::to_string(max_tag) +
                                        " is too large for the field offset type to represent");
        }

        for (const auto& field : m_fields) {
            m_fields_by_name.emplace(field.name(), field);
        }
    }

    collection::const_iterator begin() const { return m_fields.begin(); }
    collection::const_iterator end() const { return m_fields.end(); }
    collection::size_type size() const { return m_fields.size(); }
    
    const orchestration_field& operator[](size_t tag) const 
    { 
        if (tag < m_field_offsets.size()) {
            auto offset = m_field_offsets[tag];
            if (offset < m_fields.size()) {
                return m_fields[offset];
            }
        }
        throw std::out_of_range(std::to_string(tag) + " exceeds the highest tag number " + std::to_string(m_fields.rbegin()->tag()));
    }

    const orchestration_field& operator[](const std::string& name) const
    {
        auto field = m_fields_by_name.find(name);

        if (field == m_fields_by_name.end()) {
            throw std::out_of_range("no field with name '" + name + "'");
        }

        return field->second;
    }

    const std::string_view& name_of_field(size_t tag) const noexcept
    {
        if (tag < m_field_offsets.size()) {
            auto offset = m_field_offsets[tag];
            if (offset < m_fields.size()) {
                return m_fields[offset].name();
            }
        }

        static const std::string_view empty_string {""};
        return empty_string;
    }

    const std::string_view& name_of_value(size_t tag, const std::string& value) const noexcept
    {
        if (tag < m_field_offsets.size()) {
            auto offset = m_field_offsets[tag];
            if (offset < m_fields.size()) {
                return m_fields[offset].name_of_value(value);
            }
        }

        static const std::string_view empty_string {""};
        return empty_string;
    }

private:

    orchestration_field_collection(const orchestration_field_collection&) = delete;
    orchestration_field_collection operator=(const orchestration_field_collection&) = delete;
    orchestration_field_collection(orchestration_field_collection&&) = delete;
    orchestration_field_collection operator=(orchestration_field_collection&&) = delete;

    using offset_collection = std::vector<offset_type>;
    using name_collection = std::map<std::string, const orchestration_field&>;
    
    offset_collection m_field_offsets;
    collection m_fields;
    name_collection m_fields_by_name;

};

}

#endif
