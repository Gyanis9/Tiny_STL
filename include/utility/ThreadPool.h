#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <mutex>
#include <memory>
#include <functional>
#include <condition_variable>
#include <future>
#include <thread>
#include <atomic>
#include <vector>
#include <deque>
#include <string>
#include <stdexcept>

namespace Tiny
{
    /**
     * @brief 线程池类，支持任务异步提交和线程复用。
     *
     * 通过线程池可以有效地复用线程资源、限制并发数量，并异步执行任务，
     * 适用于高并发、高性能的场景。
     */
    class ThreadPool
    {
    private:
        using Task = std::function<void()>;

    public:
        /**
         * @brief 构造线程池
         * @param name 线程池名称（可选，便于日志定位）
         */
        explicit ThreadPool(std::string name = "ThreadPool");

        /**
         * @brief 析构函数，自动停止所有线程
         */
        ~ThreadPool();

        /**
         * @brief 禁止拷贝构造
         */
        ThreadPool(const ThreadPool&) = delete;

        /**
         * @brief 禁止拷贝赋值
         */
        ThreadPool& operator=(const ThreadPool&) = delete;

        /**
         * @brief 禁止移动构造
         */
        ThreadPool(ThreadPool&&) = delete;

        /**
         * @brief 禁止移动赋值
         */
        ThreadPool& operator=(ThreadPool&&) = delete;

        /**
         * @brief 设置任务队列的最大长度
         * @param maxSize 队列最大任务数
         */
        void setTaskMaxSize(uint32_t maxSize);

        /**
         * @brief 当前队列中的任务数量
         * @return 队列长度
         */
        size_t queueSize() const;

        /**
         * @brief 当前线程池的线程数
         * @return 线程数量
         */
        uint32_t threadCount() const;

        /**
         * @brief 获取线程池名称
         * @return 线程池名称
         */
        const std::string& name() const { return m_name; }

        /**
         * @brief 启动线程池
         * @param numThreads 启动线程数（默认自动检测硬件并发数）
         */
        void start(uint32_t numThreads = std::thread::hardware_concurrency());

        /**
         * @brief 停止线程池，等待所有任务完成
         */
        void stop();

        /**
         * @brief 等待所有任务执行完成，不再接收新任务
         */
        void drain();

        /**
         * @brief 直接投递一个任务到线程池（无返回值）
         * @param task 可调用对象（如lambda、函数、绑定对象等）
         */
        void run(Task task);

        /**
         * @brief 设置线程初始化回调，在每个工作线程开始时调用一次
         * @param cb 回调函数
         */
        void setThreadInitCallback(Task cb) { m_threadInitCallback = std::move(cb); }

        /**
         * @brief 异步提交任务，获取future结果
         * @tparam Func 函数类型
         * @tparam Args 参数类型
         * @param func 可调用对象
         * @param args 可调用对象参数
         * @return future<Rtype> 用于获取结果
         * @throws std::runtime_error 若线程池已停止，提交任务失败
         */
        template <typename Func, typename... Args>
        auto submitTask(Func&& func, Args&&... args)
            -> std::future<std::invoke_result_t<Func, Args...>>
        {
            using Rtype = std::invoke_result_t<Func, Args...>;

            // 用bind提前绑定好参数
            auto boundTask = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
            auto task = std::make_shared<std::packaged_task<Rtype()>>(boundTask);

            std::future<Rtype> result = task->get_future();

            {
                std::unique_lock lock(m_mutex);

                if (!m_isRunning)
                {
                    throw std::runtime_error("ThreadPool is stopped, cannot submit new tasks.");
                }

                m_notFull.wait(lock, [this]
                {
                    return !m_isRunning || !isFull();
                });

                if (!m_isRunning)
                {
                    throw std::runtime_error("ThreadPool is stopped during task submission.");
                }

                m_tasks.emplace_back([task] { (*task)(); });
            }

            m_notEmpty.notify_one();
            return result;
        }

    private:
        /**
         * @brief 工作线程主函数
         */
        void ThreadFunc();

        /**
         * @brief 从任务队列中取出一个任务（线程安全）
         * @return 任务对象
         */
        Task take();

        /**
         * @brief 判断任务队列是否已满
         * @return true 满，false 未满
         */
        [[nodiscard]] bool isFull() const;

    private:
        Task m_threadInitCallback; ///< 线程初始化回调
        std::vector<std::unique_ptr<std::thread>> m_threads; ///< 工作线程集合
        std::deque<Task> m_tasks; ///< 任务队列
        std::atomic<bool> m_isRunning{false}; ///< 线程池运行状态
        size_t m_maxTaskSize{0}; ///< 任务队列最大长度
        mutable std::mutex m_mutex; ///< 互斥锁，保护任务队列
        std::condition_variable m_notEmpty; ///< 任务队列非空条件变量
        std::condition_variable m_notFull; ///< 任务队列未满条件变量
        std::string m_name; ///< 线程池名称
    };
}

#endif
