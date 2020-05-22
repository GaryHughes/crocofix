#include "test_scheduler.hpp"

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
}

void test_scheduler::schedule(task_type task)
{
    m_tasks.enqueue(task);
}

}
