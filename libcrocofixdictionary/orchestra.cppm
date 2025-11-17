module;

#include <vector>

export module crocofix.dictionary:orchestra;

import :orchestration;

export namespace crocofix::dictionary
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
