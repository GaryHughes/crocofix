module;

#include <string>

export module crocofix.dictionary:presence;

export namespace crocofix::dictionary
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


