#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace msgrpc::system {
class async_pool {
   public:
    using job_func = std::function<void()>;
    using polling_job_func = std::function<void(job_func&&)>;

   public:
    async_pool(std::chrono::milliseconds polling_resolution);
    ~async_pool();

    void start(std::size_t thread_count);
    void stop();

    void submit(job_func&& job);
    void attach_polling_job(polling_job_func&& job,
                            std::chrono::milliseconds timeout, bool waiting);

   private:
    struct polling_job {
        polling_job_func job;
        std::chrono::milliseconds timeout;
        bool waiting{false};
        std::unique_ptr<std::atomic_bool> running{
            std::make_unique<std::atomic_bool>(false)};
        std::chrono::high_resolution_clock::time_point last_started{
            std::chrono::high_resolution_clock::now()};
    };

   private:
    void polling_work();
    void queued_work();

   private:
    std::chrono::milliseconds m_polling_resolution;

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
}  // namespace msgrpc::system
