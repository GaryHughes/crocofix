#ifndef crocofix_libcrocofixdictionary_orchestrata_hpp
#define crocofix_libcrocofixdictionary_orchestrata_hpp

#include "orchestration.hpp"
#include <vector>

namespace crocofix::dictionary
{

class orchestra
{
public:

    orchestra(std::initializer_list<orchestration> orchestrations)
    : m_orchestrations(orchestrations)
    {
    }

private:

    std::vector<orchestration> m_orchestrations;

};

}

#endif