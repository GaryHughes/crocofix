module;

#include <functional>
#include <chrono>

export module crocofix:scheduler;

export namespace crocofix
{

class scheduler
{
public:

    virtual ~scheduler() = default;

    virtual void run() = 0;
   
    using task_type = std::function<void()>;
    using time_point = std::chrono::time_point<std::chrono::system_clock>;
    using scheduled_callback = std::function<void()>;
    using cancellation_token = uint64_t;
  
    virtual void schedule(task_type task) = 0;
    // TODO - think up better names
    virtual cancellation_token schedule_relative_callback(std::chrono::milliseconds when, const scheduled_callback& callback) = 0;
    virtual cancellation_token schedule_repeating_callback(std::chrono::milliseconds interval, const scheduled_callback& callback) = 0;
    virtual void cancel_callback(cancellation_token token) = 0;
    
 
};

}
