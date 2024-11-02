#include <system/async_pool.h>

#include <chrono>
#include <mutex>
#include <thread>

namespace msgrpc::system {

async_pool::async_pool(std::chrono::milliseconds polling_resolution)
    : m_polling_thread_attached(false),
      m_polling_is_running(false),
      m_queued_is_running(false),
      m_polling_resolution(polling_resolution) {}

async_pool::~async_pool() {}

void async_pool::start(std::size_t thread_count) {}

void async_pool::stop() {
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

void async_pool::submit(async_pool::job_func&& job) {
    {
        std::unique_lock lck(m_queued_mutex);
        m_queued_jobs.push_back(job);
    }

    m_queued_signal.notify_one();
}

void async_pool::attach_polling_job(async_pool::polling_job_func&& job,
                                    std::chrono::milliseconds timeout,
                                    bool waiting) {
    bool expected = false;

    if (m_polling_thread_attached.compare_exchange_strong(expected, true)) {
        m_threads.push_back(std::jthread([this]() { polling_work(); }));
    }

    {
        std::unique_lock lck(m_polling_mutex);
        m_polling_jobs.emplace_back(job, timeout, waiting);
    }

    m_polling_signal.notify_one();
}

void async_pool::polling_work() {
    auto t = std::chrono::high_resolution_clock::now();

    using clock = std::chrono::high_resolution_clock;

    while (m_polling_is_running) {
        std::unique_lock lck(m_polling_mutex);
        m_polling_signal.wait(lck, [this] {
            return !m_polling_jobs.empty() && !m_polling_is_running;
        });

        if (m_polling_is_running) {
            return;
        }

        for (auto& job : m_polling_jobs) {
            auto dt = (clock::now() - job.last_started);

            if (dt >= job.timeout && (job.waiting && !job.running->load())) {
                submit([&] {
                    job.running->store(true);
                    job.last_started = clock::now();
                    job.job([&]() { job.waiting = false; });
                    job.running->store(false);
                });
            }
        }
        auto dt = (clock::now() - t);
        // this should handle if we have dt bigger then resolution
        // possibly start another timer thread and stuff
        // also warn
        auto should_wait = m_polling_resolution - dt;
        std::this_thread::sleep_for(should_wait);
    }
}

void async_pool::queued_work() {
    bool running = true;

    while (m_queued_is_running) {
        job_func func;

        {
            std::unique_lock lck(m_queued_mutex);

            m_queued_signal.wait(lck, [&] {
                return !m_queued_jobs.empty() && !m_queued_is_running;
            });

            if (!m_queued_is_running) {
                return;
            }

            func = m_queued_jobs.back();
            m_queued_jobs.pop_back();
        }

        try {
            func();
        } catch (...) {
            // TODO: notfiy about this, warn
        }
    }
}
}  // namespace msgrpc::system
