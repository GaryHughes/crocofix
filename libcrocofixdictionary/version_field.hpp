#ifndef crocofix_libcrocofixdictionary_version_field_hpp
#define crocofix_libcrocofixdictionary_version_field_hpp

#include <string>

namespace crocofix::dictionary
{

// This is the generic definition of a field for a specific FIX version. It does not contain message
// specific properties.
class version_field
{
public:

   version_field(int tag, std::string_view name, std::string_view type, std::string_view added, std::string_view synopsis)
   : m_tag(tag), m_name(name), m_type(type), m_added(added), m_synopsis(synopsis)
   {
   }

   constexpr int tag() const noexcept { return m_tag; }
   constexpr const std::string_view& name() const noexcept { return m_name; }
   constexpr const std::string_view& type() const noexcept { return m_type; }
   constexpr const std::string_view& added() const noexcept { return m_added; }
   constexpr const std::string_view& synopsis() const noexcept { return m_synopsis; }

   // We store field definititions in version_field_collection in an array indexed by the tag
   // number. There are gaps in the sequence and we insert dummy values with a tag of -1 in the
   // gaps, this method lets us test for dummies.
   constexpr bool is_valid() const { return tag() > 0; }

private:

   int m_tag;
   std::string_view m_name;
   std::string_view m_type;
   std::string_view m_added;
   std::string_view m_synopsis;

};

}

#endif