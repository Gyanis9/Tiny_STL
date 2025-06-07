#include "utility/ThreadPool.h"
#include <iostream>

namespace Tiny
{
    ThreadPool::ThreadPool(std::string name)
        : m_name(std::move(name))
    {
    }

    ThreadPool::~ThreadPool()
    {
        if (m_isRunning)
        {
            stop();
        }
    }

    void ThreadPool::setTaskMaxSize(const uint32_t maxSize)
    {
        std::lock_guard lock(m_mutex);
        if (m_isRunning)
        {
            throw std::runtime_error("Cannot change task size while pool is running");
        }
        m_maxTaskSize = maxSize;
    }

    size_t ThreadPool::queueSize() const
    {
        std::lock_guard lock(m_mutex);
        return m_tasks.size();
    }

    uint32_t ThreadPool::threadCount() const
    {
        return static_cast<uint32_t>(m_threads.size());
    }

    void ThreadPool::start(const uint32_t numThreads)
    {
        if (m_isRunning)
        {
            return;
        }

        {
            std::lock_guard lock(m_mutex);
            m_isRunning = true;
            m_threads.reserve(numThreads);

            for (uint32_t i = 0; i < numThreads; ++i)
            {
                m_threads.emplace_back(std::make_unique<std::thread>(
                    [this]() { ThreadFunc(); }
                ));
            }
        }
    }

    void ThreadPool::stop()
    {
        if (!m_isRunning.exchange(false, std::memory_order_release))
        {
            return;
        }

        {
            std::lock_guard lock(m_mutex);
            m_notEmpty.notify_all();
            m_notFull.notify_all();
        }

        for (const auto& thread : m_threads)
        {
            thread->join();
        }

        std::lock_guard lock(m_mutex);
        m_threads.clear();
    }

    void ThreadPool::drain()
    {
        std::lock_guard lock(m_mutex);
        m_tasks.clear();
    }

    void ThreadPool::run(Task task)
    {
        if (m_threads.empty())
        {
            if (!m_isRunning)
            {
                throw std::runtime_error("ThreadPool is stopped");
            }
            task();
            return;
        }

        {
            std::unique_lock lock(m_mutex);
            m_notFull.wait(lock, [this]
            {
                return !m_isRunning || !isFull();
            });

            if (!m_isRunning)
            {
                throw std::runtime_error("ThreadPool is stopped");
            }

            m_tasks.emplace_back(std::move(task));
        }
        m_notEmpty.notify_one();
    }

    ThreadPool::Task ThreadPool::take()
    {
        std::unique_lock lock(m_mutex);
        m_notEmpty.wait(lock, [this]
        {
            return !m_tasks.empty() || !m_isRunning;
        });

        if (!m_tasks.empty())
        {
            Task task = std::move(m_tasks.front());
            m_tasks.pop_front();
            m_notFull.notify_one();
            return task;
        }
        return nullptr;
    }

    void ThreadPool::ThreadFunc()
    {
        try
        {
            if (m_threadInitCallback)
            {
                m_threadInitCallback();
            }

            while (m_isRunning)
            {
                if (auto task = take())
                {
                    task();
                }
                else if (!m_isRunning)
                {
                    break;
                }
            }

            // 处理剩余任务
            while (true)
            {
                auto task = take();
                if (!task)
                {
                    break;
                }
                task();
            }
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Exception in thread pool " << m_name
                << ": " << ex.what() << std::endl;
            std::terminate();
        }
        catch (...)
        {
            std::cerr << "Unknown exception in thread pool " << m_name << std::endl;
            std::terminate();
        }
    }

    bool ThreadPool::isFull() const
    {
        return m_maxTaskSize > 0 && m_tasks.size() >= m_maxTaskSize;
    }
}
