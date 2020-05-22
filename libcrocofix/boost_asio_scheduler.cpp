#include "boost_asio_scheduler.hpp"

namespace crocofix
{

boost_asio_scheduler::boost_asio_scheduler(boost::asio::io_context& io_context)
: m_io_context(io_context)
{
}

void boost_asio_scheduler::run()
{
    m_io_context.run();
}

}
