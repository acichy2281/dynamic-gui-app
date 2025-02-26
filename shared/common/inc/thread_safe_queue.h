#ifndef THREAD_SAFE_QUEUE_H_
#define THREAD_SAFE_QUEUE_H_

#include <mutex>
#include <queue>
#include <memory>
#include <iostream>

struct Message_T
{
    std::unique_ptr<char[]> data;
    uint16_t size;
};

template <typename QueueType>
class ThreadSafeQueue_C
{
    public:
        // Add a message to the queue in a thread-safe manner.
        void AddMessageToQueue(QueueType&& message)
        {
            std::unique_lock<std::mutex> lock(_queueMutex);
            _messageQueue.push(std::move(message));
        }

        // Retrieve and remove a message from the queue in a thread-safe manner.
        QueueType GetMessageFromQueue()
        {
            QueueType retVal{};
            std::unique_lock<std::mutex> lock(_queueMutex);
            if (!_messageQueue.empty()) 
            {
                retVal = std::move(_messageQueue.front());
                _messageQueue.pop();
            }
            else
            {
                std::cout << "Error message queue is empty!\n";
            }
            return retVal;
        }

        // Check if queue is empty
        bool IsQueueEmpty()
        {
            std::unique_lock<std::mutex> lock(_queueMutex);
            return _messageQueue.empty();
        }

        size_t Size()
        {
            return _messageQueue.size();
        }
    
    private:
        // Message queue for thread-safe communication.
        std::queue<QueueType> _messageQueue;

        // Mutex for synchronizing access to the message queue.
        std::mutex _queueMutex;

};

#endif // THREAD_SAFE_QUEUE_H_