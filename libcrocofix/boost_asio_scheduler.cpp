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
    auto context = m_timer_contexts.emplace_back(std::make_shared<boost::asio::io_context>());

    m_timer_threads.emplace_back([=]() {
        boost::asio::deadline_timer timer(*context);
        timer.expires_from_now(boost::posix_time::milliseconds(when.count()));
        timer.async_wait([=](boost::system::error_code) {
            callback();
        });
        context->run_one();
    });

    return reinterpret_cast<cancellation_token>(context.get());
}

void boost_asio_scheduler::cancel_callback(cancellation_token token)
{
    auto context = std::find_if(m_timer_contexts.begin(), 
                                m_timer_contexts.end(),
                                [=](const auto& context) { 
                                    return context.get() == reinterpret_cast<boost::asio::io_context*>(token); 
                                });

    if (context != m_timer_contexts.end()) {
        (*context)->stop();
        m_timer_contexts.erase(context);
    }
}

}
