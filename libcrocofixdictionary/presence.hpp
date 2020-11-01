#ifndef crocofix_libcrocofixdictionary_presence_hpp
#define crocofix_libcrocofixdictionary_presence_hpp

#include <string>

namespace crocofix::dictionary
{

enum class presence
{
    required,
    optional,
    forbidden,
    ignored,
    constant
};

}

#endif

