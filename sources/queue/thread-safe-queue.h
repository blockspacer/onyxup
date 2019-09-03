#pragma once

#include <mutex>
#include <memory>
#include <condition_variable>
#include <queue>

namespace onyxup {

    template <typename T>
    class ThreadSafeQueue {
    public:
        ThreadSafeQueue(){}
        ThreadSafeQueue(const ThreadSafeQueue& other) : std::lock_guard<std::mutex>(other.mutex), queue(other.queue){
        }

        ThreadSafeQueue&operator=(const ThreadSafeQueue& other) = delete;

        void push(T new_value) {
            std::lock_guard<std::mutex> lock{mutex};
            queue.push(new_value);
            cond.notify_one();
        }

        void wait_and_pop(T& value) {
            std::unique_lock<std::mutex> lock{mutex};
            cond.wait(lock, [this]{ return !queue.empty(); });
            value = queue.front();
            queue.pop();
        }

        std::shared_ptr<T> wait_and_pop() {
            std::unique_lock<std::mutex> lock{mutex};
            cond.wait(lock, [this]{ return !queue.empty(); });
            std::shared_ptr<T> res{std::make_shared<T>(queue.front())};
            queue.pop();
            return res;
        }

        bool try_pop(T& value) {
            std::lock_guard<std::mutex> lock{mutex};
            if (queue.empty())
                return false;
            value = queue.front();
            queue.pop();
            return true;
        }

        std::shared_ptr<T> try_pop() {
            std::lock_guard<std::mutex> lock{mutex};
            if (queue.empty())
                return std::shared_ptr<T>{};
            std::shared_ptr<std::mutex> res{std::make_shared<T>(queue.front())};
            queue.pop();
            return res;
        }

        bool empty() const {
            std::lock_guard<std::mutex> lock{mutex};
            return queue.empty();
        }

        size_t size() const {
            std::lock_guard<std::mutex> lock{mutex};
            return queue.size();
        }

    private:
        mutable std::mutex mutex;
        std::condition_variable cond;
        std::queue<T> queue;
    };

}