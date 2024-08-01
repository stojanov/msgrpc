#pragma once

#include <atomic>
#include <cstddef>
#include <functional>
#include <chrono>
#include <deque>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace msgrpc::system
{
    class async_pool
    {
    public:
        using job_func = std::function<void()>;

    public:

        async_pool();
        ~async_pool();

        void start(std::size_t thread_count);
        void stop();

        void submit(job_func&& job);
        void attach_polling_job(job_func&& job, std::chrono::milliseconds timeout);

    private:
        struct polling_job 
        {
            job_func job;
            std::chrono::milliseconds timeout;
            std::chrono::high_resolution_clock::time_point last_started;
        };

    private:
        void polling_work();
        void queued_work();

    private:
        std::atomic_bool m_polling_is_running;
        std::atomic_bool m_queued_is_running;

        std::atomic_bool m_polling_thread_attached;
        std::vector<polling_job> m_polling_jobs;
        std::deque<job_func> m_queued_jobs;

        std::mutex m_polling_mutex;
        std::condition_variable m_polling_signal;

        std::mutex m_queued_mutex;
        std::condition_variable m_queued_signal;

        std::vector<std::jthread> m_threads;
    };
}
