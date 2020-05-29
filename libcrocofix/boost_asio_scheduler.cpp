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

scheduler::cancellation_token boost_asio_scheduler::schedule_relative_callback(std::chrono::milliseconds when, scheduled_callback callback)
{
    return 0;
}

void boost_asio_scheduler::cancel_callback(cancellation_token token)
{

}

}
