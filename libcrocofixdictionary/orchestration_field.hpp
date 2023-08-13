#ifndef crocofix_libcrocofixdictionary_orchestration_field_hpp
#define crocofix_libcrocofixdictionary_orchestration_field_hpp

#include <string>
#include <vector>
#include <functional>
#include <boost/algorithm/string.hpp>
#include "field_value.hpp"
#include "pedigree.hpp"

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
      std::string_view synopsis, 
      const pedigree& pedigree, 
      std::initializer_list<std::reference_wrapper<const field_value>> values = {})
   :  m_tag(tag), 
      m_is_data(is_data),
      m_name(name), 
      m_type(type), 
      m_synopsis(synopsis), 
      m_pedigree(pedigree), 
      m_values(values)
   {
   }

   constexpr int tag() const noexcept { return m_tag; }
   constexpr bool is_data() const noexcept { return m_is_data; } 
   constexpr const std::string_view& name() const noexcept { return m_name; }
   constexpr const std::string_view& type() const noexcept { return m_type; }
   constexpr const value_collection& values() const noexcept { return m_values; }
   constexpr const dictionary::pedigree& pedigree() const noexcept { return m_pedigree; }
   constexpr const std::string_view& synopsis() const noexcept { return m_synopsis; }

   bool is_numeric() const noexcept
   {
      // TODO - this properly with base_type when we fix the orchestra.
      auto lower_name = std::string(type());
      boost::algorithm::to_lower(lower_name);

      if (lower_name == "int" ||
          lower_name == "length" ||
          lower_name == "tagnum" ||
          lower_name == "seqnum" ||
          lower_name == "numingroup" ||
          lower_name == "float" ||
          lower_name == "qty" ||
          lower_name == "price" ||
          lower_name == "priceoffset" ||
          lower_name == "amt" ||
          lower_name == "percentage")
      {
         return true;
      }

      return false;
   }

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
   std::string_view m_base_type;
   std::string_view m_synopsis;
   dictionary::pedigree m_pedigree;
   value_collection m_values;

};

}

#endif