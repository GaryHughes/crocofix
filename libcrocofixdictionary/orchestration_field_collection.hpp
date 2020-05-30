#ifndef crocofix_libcrocofixdictionary_orchestration_field_collection_hpp
#define crocofix_libcrocofixdictionary_orchestration_field_collection_hpp

#include <initializer_list>
#include <vector>
#include "orchestration_field.hpp"

namespace crocofix::dictionary
{

// This is the set of all fields for a specific FIX orchestration. It contains the generic definition of
// the field. It does not contain message specific definitions.
// To support constant time looks based on field tags we store the definititions in an array, because
// there are gaps in the field tag sequence there are gaps in this collection, this is represented
// by inserted dummy fields with a tag == 0.
class orchestration_field_collection
{
public:

    using collection = std::vector<orchestration_field>;

    orchestration_field_collection(std::initializer_list<orchestration_field> fields)
    {
        // This assumes fields are sorted in ascending order by orchestration_field::tag().
        if (fields.size() == 0)
        {
            return;
        }

        m_fields.reserve(std::rbegin(fields)->tag() + 1);

        size_t index{0};
        
        for (auto field : fields)
        {
            while (field.tag() > index)
            {
                m_fields.emplace_back(-1, false, "", "", "", "");
                ++index;
            }
         
            m_fields.emplace_back(field);
            ++index;
        }
    }

    collection::const_iterator begin() const { return m_fields.begin(); }
    collection::const_iterator end() const { return m_fields.end(); }
    collection::size_type size() const { return m_fields.size(); }
    const orchestration_field& operator[](size_t tag) const { return m_fields[tag]; }

    const std::string_view& name_of_field(size_t tag) const noexcept
    {
        if (tag < m_fields.size()) {
            return m_fields[tag].name();
        }

        static const std::string_view empty_string {""};
        return empty_string;
    }

    const std::string_view& name_of_value(size_t tag, const std::string& value) const noexcept
    {
        if (tag < m_fields.size()) {
            return m_fields[tag].name_of_value(value);
        }

        static const std::string_view empty_string {""};
        return empty_string;
    }

private:

    collection m_fields;

};

}

#endif