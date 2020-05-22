#ifndef crocofix_libcrocofix_tests_scheduler_hpp
#define crocofix_libcrocofix_tests_scheduler_hpp

#include <libcrocofix/scheduler.hpp>
#include <libcrocofixutility/blocking_queue.hpp>
#include <atomic>
#include <thread>
#include <functional>

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

    void schedule(task_type task);

    void stop();

private:

    using task_queue = blocking_queue<task_type>;

    task_queue m_tasks;
    std::atomic_bool m_stopping = false;
    std::thread m_task_thread;

};

}

#endif