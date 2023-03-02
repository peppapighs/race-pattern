#pragma once

#include <atomic>
#include <condition_variable>
#include <optional>
#include <queue>

template <class T> class concurrent_queue {
  private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable cond;
    std::atomic<int> task_count{0};
    std::atomic<bool> terminated{0};

  public:
    void push(const T &item) {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(item);
        task_count++;
        cond.notify_one();
    }

    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mutex);
        while (queue.empty() && !terminated)
            cond.wait(lock);
        if (terminated)
            return std::nullopt;
        T item = queue.front();
        queue.pop();
        return item;
    }

    void done() {
        --task_count;
        if (task_count == 0)
            cond.notify_all();
    }

    void join() {
        std::unique_lock<std::mutex> lock(mutex);
        while (task_count > 0)
            cond.wait(lock);
    }

    void terminate() {
        terminated = 1;
        cond.notify_all();
    }
};