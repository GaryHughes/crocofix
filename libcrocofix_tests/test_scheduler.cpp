#include "test_scheduler.hpp"
#include <boost/date_time.hpp>

namespace crocofix
{

test_scheduler::test_scheduler()
{
    m_task_thread = std::thread([&]()
    { 
        while (!m_stopping)
        {
            auto task = m_tasks.dequeue();
            task();
        }
    });
}

test_scheduler::~test_scheduler()
{
    stop();
}

void test_scheduler::run()
{
}

void test_scheduler::stop()
{
    m_tasks.enqueue([&](){ m_stopping = true; });
    m_task_thread.join();

    for (auto& context : m_timer_contexts) {
        context->stop();
    }

    for (auto& thread : m_timer_threads) {
        thread.join();
    }
}

void test_scheduler::schedule(task_type task)
{
    m_tasks.enqueue(task);
}

scheduler::cancellation_token test_scheduler::schedule_relative_callback(std::chrono::milliseconds when, scheduled_callback callback)
{
    auto context = m_timer_contexts.emplace_back(std::make_shared<boost::asio::io_context>());

    m_timer_threads.emplace_back([=]() {
        boost::asio::deadline_timer timer(*context);
        timer.expires_from_now(boost::posix_time::milliseconds(when.count()));
        timer.async_wait([=](boost::system::error_code) {
            m_tasks.enqueue([=]() {
                callback();
            });
        });
        context->run_one();
    });

    return reinterpret_cast<cancellation_token>(context.get());
}

scheduler::cancellation_token test_scheduler::schedule_repeating_callback(std::chrono::milliseconds interval, scheduled_callback callback)
{
    auto context = m_timer_contexts.emplace_back(std::make_shared<boost::asio::io_context>());

    // We don't actually need these timers for the unit tests, just implement enough to satisfy the interface.

    return reinterpret_cast<cancellation_token>(context.get());
}

void test_scheduler::cancel_callback(cancellation_token token)
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
