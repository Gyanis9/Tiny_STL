#include "utility/ThreadPool.h"
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <future>
#include <vector>

using namespace std::chrono_literals;

namespace Tiny
{
    class ThreadPoolTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            pool = std::make_unique<ThreadPool>("TestPool");
        }

        void TearDown() override
        {
            if (pool)
            {
                pool->stop();
            }
        }

        std::unique_ptr<ThreadPool> pool;
    };

    // 基本功能测试
    TEST_F(ThreadPoolTest, BasicFunctionality)
    {
        pool->start(2);

        std::atomic<int> counter{0};
        auto task = [&counter] { ++counter; };

        pool->run(task);
        pool->run(task);

        // 等待任务完成
        std::this_thread::sleep_for(50ms);
        EXPECT_EQ(counter, 2);
    }

    // 任务提交和结果获取
    TEST_F(ThreadPoolTest, SubmitTaskWithResult)
    {
        pool->start(2);

        auto add = [](const int a, const int b) { return a + b; };

        auto f1 = pool->submitTask(add, 2, 3);
        auto f2 = pool->submitTask([] { return 42; });

        EXPECT_EQ(f1.get(), 5);
        EXPECT_EQ(f2.get(), 42);
    }

    // 队列大小限制
    TEST_F(ThreadPoolTest, TaskQueueLimit)
    {
        pool->setTaskMaxSize(2);
        pool->start(1); // 单线程处理

        std::atomic<int> processed{0};
        auto task = [&processed]
        {
            std::this_thread::sleep_for(100ms);
            ++processed;
        };

        // 提交3个任务 - 第三个应该阻塞直到有空间
        pool->submitTask(task);
        pool->submitTask(task);
        auto future = pool->submitTask(task);

        // 立即检查队列大小
        EXPECT_EQ(pool->queueSize(), 2);

        future.get(); // 等待所有任务完成
        EXPECT_EQ(processed, 3);
    }

    // 线程池停止行为
    TEST_F(ThreadPoolTest, StopBehavior)
    {
        pool->start(2);

        std::atomic<int> counter{0};
        auto long_task = [&counter]
        {
            std::this_thread::sleep_for(100ms);
            ++counter;
        };

        pool->run(long_task);
        pool->run(long_task);
        pool->run(long_task); // 第三个任务进入队列

        pool->stop();

        // 停止后不应执行新任务
        EXPECT_THROW(pool->run([]{}), std::runtime_error);
        EXPECT_THROW(pool->submitTask([] { return 1; }), std::runtime_error);

        // 已入队但未执行的任务不应执行
        EXPECT_EQ(counter, 3);
    }

    // 异常处理
    TEST_F(ThreadPoolTest, ExceptionHandling)
    {
        pool->start(1);

        auto thrower = []
        {
            throw std::runtime_error("Test error");
        };

        auto future = pool->submitTask(thrower);

        EXPECT_THROW(future.get(), std::runtime_error);
    }

    // 线程初始化回调
    TEST_F(ThreadPoolTest, ThreadInitCallback)
    {
        std::atomic<int> init_count{0};
        pool->setThreadInitCallback([&init_count]
        {
            ++init_count;
        });

        pool->start(3);

        // 等待线程初始化
        std::this_thread::sleep_for(50ms);
        EXPECT_EQ(init_count, 3);
    }

    // 并发任务提交
    TEST_F(ThreadPoolTest, ConcurrentSubmission)
    {
        constexpr int THREADS = 10;
        constexpr int TASKS_PER_THREAD = 100;
        pool->start(4);

        std::vector<std::thread> threads;
        std::atomic<int> counter{0};

        for (int i = 0; i < THREADS; ++i)
        {
            threads.emplace_back([this, &counter]
            {
                for (int j = 0; j < TASKS_PER_THREAD; ++j)
                {
                    pool->submitTask([&counter] { ++counter; });
                }
            });
        }

        for (auto& t : threads)
        {
            t.join();
        }

        // 等待所有任务完成
        while (pool->queueSize() > 0)
        {
            std::this_thread::sleep_for(10ms);
        }

        EXPECT_EQ(counter, THREADS * TASKS_PER_THREAD);
    }

    // 清空任务队列
    TEST_F(ThreadPoolTest, DrainQueue)
    {
        pool->start(1);

        std::atomic<int> executed{0};
        auto task = [&executed] { ++executed; };

        // 填充队列
        for (int i = 0; i < 5; ++i)
        {
            pool->run(task);
        }

        EXPECT_EQ(pool->queueSize(), 5);

        pool->drain();
        EXPECT_EQ(pool->queueSize(), 0);

        // 已清空的任务不应执行
        std::this_thread::sleep_for(50ms);
        EXPECT_EQ(executed, 0);
    }

    // 线程数量查询
    TEST_F(ThreadPoolTest, ThreadCount)
    {
        EXPECT_EQ(pool->threadCount(), 0);
        pool->start(3);
        EXPECT_EQ(pool->threadCount(), 3);
    }

    // 边界条件测试
    TEST_F(ThreadPoolTest, EdgeCases)
    {
        // 零线程
        pool->start(0);
        std::atomic<int> counter{0};
        pool->run([&counter] { ++counter; });
        EXPECT_EQ(counter, 1);

        // 停止后重启
        pool->stop();
        pool->start(2);
        pool->run([&counter] { ++counter; });
        std::this_thread::sleep_for(50ms);
        EXPECT_EQ(counter, 2);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
