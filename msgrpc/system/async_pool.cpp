#include <system/async_pool.h>
#include <mutex>

namespace msgrpc::system
{

    async_pool::async_pool()
        : m_polling_thread_attached(false)
        , m_polling_is_running(false)
        , m_queued_is_running(false)
    {
    }

    async_pool::~async_pool()
    {

    }

    void async_pool::start(std::size_t thread_count)
    {
    }

    void async_pool::stop()
    {
        {
            std::unique_lock lck(m_queued_mutex);
            m_queued_jobs.clear();
        }

        {
            std::unique_lock lck(m_polling_mutex);
            m_polling_jobs.clear();
        }

        m_queued_is_running = false;
        m_polling_is_running = false;

        m_queued_signal.notify_all();
        m_polling_signal.notify_all();
    }

    void async_pool::submit(async_pool::job_func&& job)
    {
        {
            std::unique_lock lck(m_queued_mutex);
            m_queued_jobs.push_back(job);
        }

        m_queued_signal.notify_one();
    }
    
    void async_pool::attach_polling_job(async_pool::job_func&& job, std::chrono::milliseconds timeout)
    {
        bool expected = false; 

        if (m_polling_thread_attached.compare_exchange_strong(expected, true))
        {
            m_threads.push_back(std::jthread([this]()
                        {
                            polling_work();
                        }));

        }

        {
            std::unique_lock lck(m_polling_mutex);
            m_polling_jobs.emplace_back(job, timeout);
        }

        m_polling_signal.notify_one();
    }
    
}
