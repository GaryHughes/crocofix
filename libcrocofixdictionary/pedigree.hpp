#ifndef crocofix_libcrocofixdictionary_pedigree_hpp
#define crocofix_libcrocofixdictionary_pedigree_hpp

#include <string>

namespace crocofix::dictionary
{

class pedigree
{
public:

    pedigree(std::string_view added, std::string_view addedEP, std::string_view updated, std::string_view updatedEP, std::string_view deprecated, std::string_view deprecatedEP)
    : m_added(added), m_addedEP(addedEP), m_updated(updated), m_updatedEP(updatedEP), m_deprecated(deprecated), m_deprecatedEP(deprecatedEP)  
    {
    }

    constexpr const std::string_view& added() const noexcept { return m_added; }
    constexpr const std::string_view& addedEP() const noexcept { return m_addedEP; }
    constexpr const std::string_view& updated() const noexcept { return m_updated; }
    constexpr const std::string_view& updatedEP() const noexcept { return m_updatedEP; }
    constexpr const std::string_view& deprecated() const noexcept { return m_deprecated; }
    constexpr const std::string_view& deprecatedEP() const noexcept { return m_deprecatedEP; }

private:

    std::string_view m_added;
    std::string_view m_addedEP;
    std::string_view m_updated;
    std::string_view m_updatedEP;
    std::string_view m_deprecated;
    std::string_view m_deprecatedEP;

};

}

#endif

