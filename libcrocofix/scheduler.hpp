#ifndef crocofix_libcrocofix_scheduler_hpp
#define crocofix_libcrocofix_scheduler_hpp

#include <functional>
#include <chrono>

namespace crocofix
{

class scheduler
{
public:

    virtual void run() = 0;
  
    using time_point = std::chrono::time_point<std::chrono::system_clock>;
    using scheduled_callback = std::function<void()>;
    using cancellation_token = uint64_t;
  
    // TODO - think up better names
    virtual cancellation_token schedule_relative_callback(std::chrono::milliseconds when, scheduled_callback callback) = 0;
    virtual cancellation_token schedule_repeating_callback(std::chrono::milliseconds interval, scheduled_callback callback) = 0;
    virtual void cancel_callback(cancellation_token token) = 0;
 
};

}

#endif