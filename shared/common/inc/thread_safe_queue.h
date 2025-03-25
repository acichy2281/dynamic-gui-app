#ifndef THREAD_SAFE_QUEUE_H_
#define THREAD_SAFE_QUEUE_H_

#include <mutex>
#include <queue>
#include <memory>
#include <iostream>

template <typename QueueType>
class ThreadSafeQueue_C
{
    public:
        // Add a item to the queue in a thread-safe manner.
        void Enqueue(QueueType&& item)
        {
            std::unique_lock<std::mutex> lock(_queueMutex);
            _itemQueue.push(std::move(item));
        }

        // Retrieve and remove a item from the queue in a thread-safe manner.
        QueueType Dequeue()
        {
            QueueType retVal{};
            std::unique_lock<std::mutex> lock(_queueMutex);
            if (!_itemQueue.empty()) 
            {
                retVal = std::move(_itemQueue.front());
                _itemQueue.pop();
            }
            else
            {
                std::cout << "Error queue is empty!\n";
            }
            return retVal;
        }

        // Check if queue is empty
        bool IsQueueEmpty()
        {
            std::unique_lock<std::mutex> lock(_queueMutex);
            return _itemQueue.empty();
        }

        size_t Size()
        {
            return _itemQueue.size();
        }
    
    private:
        // item queue for thread-safe communication.
        std::queue<QueueType> _itemQueue;

        // Mutex for synchronizing access to the item queue.
        std::mutex _queueMutex;

};

#endif // THREAD_SAFE_QUEUE_H_