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

void boost_asio_scheduler::schedule(task_type task)
{
    boost::asio::post(m_io_context, task);
}

scheduler::cancellation_token boost_asio_scheduler::schedule_relative_callback(std::chrono::milliseconds when, const scheduled_callback& callback)
{
    auto token = m_next_cancellation_token++;

    auto [timer, inserted] = m_timers.emplace(token, boost::asio::deadline_timer(m_io_context, boost::posix_time::milliseconds(when.count()))); 
    
    if (!inserted) {
        throw std::runtime_error("failed to insert timer with cancellation token " + std::to_string(token));
    }
    
    timer->second.async_wait([&, callback](boost::system::error_code error) {
        
        if (!error) {
            boost::asio::post(m_io_context, [=]() {
                callback();
            });
        }

        m_timers.erase(token);
    });

    return token;
}

void boost_asio_scheduler::handler(const boost::system::error_code& error, 
                                   cancellation_token token,
                                   std::chrono::milliseconds interval,
                                   const scheduled_callback& callback)
{
    if (error) {
        m_timers.erase(token);
        return;
    }

    boost::asio::post(m_io_context, [&, callback]() {
        callback();
    });

    auto timer = m_timers.find(token);

    if (timer == m_timers.end()) {
        throw std::runtime_error("unable to find timer with cancellation token " + std::to_string(token));
    }

    timer->second.expires_from_now(boost::posix_time::milliseconds(interval.count()));
    
    timer->second.async_wait([=, this](const auto error) { // NOLINT(cppcoreguidelines-misleading-capture-default-by-value) adding *this segfaults clang-tidy
        handler(error, token, interval, callback);
    });
}

scheduler::cancellation_token boost_asio_scheduler::schedule_repeating_callback(std::chrono::milliseconds interval, const scheduled_callback& callback)
{
    auto token = m_next_cancellation_token++;
    
    auto [timer, inserted] = m_timers.emplace(token, boost::asio::deadline_timer(m_io_context, boost::posix_time::milliseconds(interval.count()))); 
    
    if (!inserted) {
        throw std::runtime_error("failed to insert timer with cancellation token " + std::to_string(token));
    }
    
    timer->second.async_wait([=, this](const auto& error) { // NOLINT(cppcoreguidelines-misleading-capture-default-by-value) adding *this segfaults clang-tidy
        handler(error, token, interval, callback);
    });

    return token;
}

void boost_asio_scheduler::cancel_callback(cancellation_token token)
{
   auto timer = m_timers.find(token);

   if (timer != m_timers.end()) {
       timer->second.cancel();
       // TODO - fix this, single shot timers clean themselves up, repeaters do not.
   }
}

}
