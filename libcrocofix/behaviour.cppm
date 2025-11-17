module;

#include <cstdint>

export module crocofix:behaviour;

export namespace crocofix
{

enum class behaviour : uint8_t
{
    initiator,
    acceptor
};

}
