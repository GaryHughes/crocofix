#include "boost_asio_scheduler.hpp"
#include <boost/bind.hpp>

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
    auto token = m_next_cancellation_token++;

    auto [timer, inserted] = m_timers.emplace(token, boost::asio::deadline_timer(m_io_context, boost::posix_time::milliseconds(when.count()))); 
    
    if (!inserted) {
        throw std::runtime_error("failed to insert timer with cancellation token " + std::to_string(token));
    }
    
    timer->second.async_wait([&, callback](boost::system::error_code error) {
        
        if (!error) {
            m_io_context.post([=]() {
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
                                   scheduled_callback callback)
{
    if (error) {
        m_timers.erase(token);
        return;
    }

    m_io_context.post([&, callback]() {
        callback();
    });

    auto timer = m_timers.find(token);

    if (timer == m_timers.end()) {
        throw std::runtime_error("unable to find timer with cancellation token " + std::to_string(token));
    }

    timer->second.expires_from_now(boost::posix_time::milliseconds(interval.count()));
    timer->second.async_wait(boost::bind(&boost_asio_scheduler::handler, this, boost::asio::placeholders::error, token, interval, callback));
}

scheduler::cancellation_token boost_asio_scheduler::schedule_repeating_callback(std::chrono::milliseconds interval, scheduled_callback callback)
{
    auto token = m_next_cancellation_token++;
    
    auto [timer, inserted] = m_timers.emplace(token, boost::asio::deadline_timer(m_io_context, boost::posix_time::milliseconds(interval.count()))); 
    
    if (!inserted) {
        throw std::runtime_error("failed to insert timer with cancellation token " + std::to_string(token));
    }
    
    timer->second.async_wait(boost::bind(&boost_asio_scheduler::handler, 
                                         this, 
                                         boost::asio::placeholders::error, 
                                         token, 
                                         interval, 
                                         callback));
    
    return token;
}

void boost_asio_scheduler::cancel_callback(cancellation_token token)
{
   auto timer = m_timers.find(token);

   if (timer != m_timers.end()) {
       timer->second.cancel();
   }
}

}
