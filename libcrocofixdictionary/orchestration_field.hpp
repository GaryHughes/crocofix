#ifndef crocofix_libcrocofixdictionary_orchestration_field_hpp
#define crocofix_libcrocofixdictionary_orchestration_field_hpp

#include <string>
#include <vector>
#include <functional>
#include "field_value.hpp"

namespace crocofix::dictionary
{

// This is the generic definition of a field for a specific FIX orchestration. It does not contain message
// specific properties.
class orchestration_field
{
public:

   using value_collection = std::vector<std::reference_wrapper<const field_value>>;

   orchestration_field(
      int tag, 
      bool is_data,
      std::string_view name, 
      std::string_view type, 
      std::string_view added, 
      std::string_view synopsis, 
      std::initializer_list<std::reference_wrapper<const field_value>> values = {})
   :  m_tag(tag), 
      m_is_data(is_data),
      m_name(name), 
      m_type(type), 
      m_added(added), 
      m_synopsis(synopsis), 
      m_values(values)
   {
   }

   constexpr int tag() const noexcept { return m_tag; }
   constexpr bool is_data() const noexcept { return m_is_data; } 
   constexpr const std::string_view& name() const noexcept { return m_name; }
   constexpr const std::string_view& type() const noexcept { return m_type; }
   constexpr const std::string_view& added() const noexcept { return m_added; }
   constexpr const std::string_view& synopsis() const noexcept { return m_synopsis; }
   constexpr const value_collection& values() const noexcept { return m_values; }

   // We store field definititions in orchestration_field_collection in an array indexed by the tag
   // number. There are gaps in the sequence and we insert dummy values with a tag of -1 in the
   // gaps, this method lets us test for dummies.
   constexpr bool is_valid() const noexcept { return tag() > 0; }

   // Return the name of an enumerated value if it is defined for this field e.g. 1 -> 'Buy' for Side.
   // Returns an empty string if no such value is defined.
   const std::string_view& name_of_value(const std::string& value) const noexcept
   {
      auto definition = std::find_if(m_values.begin(), m_values.end(), [&](const auto& v) { return v.get().value() == value; });

      if (definition == m_values.end()) {
         static std::string_view empty_string {""};
         return empty_string;
      }   

      return definition->get().name();
   }

private:

   int m_tag;
   bool m_is_data;
   std::string_view m_name;
   std::string_view m_type;
   std::string_view m_added;
   std::string_view m_synopsis;
   value_collection m_values;

};

}

#endif