module;

#include <mutex>
#include <condition_variable>
#include <deque>

export module crocofix.utility:blocking_queue;

export namespace crocofix
{

template<typename T>
class blocking_queue
{
public:

    void enqueue(T const& value) 
    {
        std::scoped_lock lock{m_mutex};
        m_queue.push_front(value);
        m_condition.notify_one();
    }

    T dequeue() 
    {
        std::unique_lock lock{m_mutex};
        m_condition.wait(lock, [this]{ return !m_queue.empty(); });
        T value{std::move(m_queue.back())};
        m_queue.pop_back();
        return value;
    }

    template<typename PeriodT>
    bool try_dequeue (T& value, PeriodT duration) 
    {
        std::unique_lock lock{m_mutex};
        if (!m_condition.wait_for(lock, duration, [this]{ return !m_queue.empty(); })) {
            return false;
        }
        value = std::move(m_queue.back());
        m_queue.pop_back();
        return true;
    }    

private:

    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::deque<T> m_queue;

};

}
