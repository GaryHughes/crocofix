#ifndef crocofix_libcrocofix_boost_asio_scheduler_hpp
#define crocofix_libcrocofix_boost_asio_scheduler_hpp

#include "scheduler.hpp"
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace crocofix
{

class boost_asio_scheduler : public scheduler
{
public:

    boost_asio_scheduler(boost::asio::io_context& io_context);

    void run() override;
    
    void schedule(task_type task) override;
    cancellation_token schedule_relative_callback(std::chrono::milliseconds when, const scheduled_callback& callback) override;
    cancellation_token schedule_repeating_callback(std::chrono::milliseconds interval, const scheduled_callback& callback) override;
    void cancel_callback(cancellation_token token) override;
 
private:

    boost::asio::io_context& m_io_context;

    cancellation_token m_next_cancellation_token {0};
    std::unordered_map<cancellation_token, boost::asio::deadline_timer> m_timers;

    void handler(const boost::system::error_code& error,
                 cancellation_token token,
                 std::chrono::milliseconds interval,
                 const scheduled_callback& callback);

};

}

#endif