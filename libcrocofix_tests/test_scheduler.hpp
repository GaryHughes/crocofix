#ifndef crocofix_libcrocofix_tests_scheduler_hpp
#define crocofix_libcrocofix_tests_scheduler_hpp

#include <libcrocofix/scheduler.hpp>
#include <libcrocofixutility/blocking_queue.hpp>
#include <atomic>
#include <thread>
#include <functional>
#include <vector>
#include <boost/asio.hpp>

namespace crocofix
{

class test_scheduler : public scheduler
{
public:

    using task_type = std::function<void()>;
  
    test_scheduler();
    ~test_scheduler();

    test_scheduler(const test_scheduler&) = delete;
    test_scheduler(test_scheduler&&) = delete;
    test_scheduler operator=(const test_scheduler&) = delete;
    test_scheduler operator=(test_scheduler&&) = delete;

    void run() override;

    cancellation_token schedule_relative_callback(std::chrono::milliseconds when, scheduled_callback callback) override;
    cancellation_token schedule_repeating_callback(std::chrono::milliseconds interval, scheduled_callback callback) override;
    void cancel_callback(cancellation_token token) override;

    void schedule(task_type task);

    void stop();

private:

    using task_queue = blocking_queue<task_type>;

    task_queue m_tasks;
    std::atomic_bool m_stopping = false;
    std::thread m_task_thread;

    std::vector<std::shared_ptr<boost::asio::io_context>> m_timer_contexts;
    std::vector<std::thread> m_timer_threads;

};

}

#endif