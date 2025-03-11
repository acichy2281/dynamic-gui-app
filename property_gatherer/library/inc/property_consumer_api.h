#ifndef PROPERTY_CONSUMER_API_H
#define PROPERTY_CONSUMER_API_H

/* System Includes */
#include <string>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <memory>
#include <inttypes.h>

/* Project Includes */
#include "thread_safe_queue.h"
#include "property_gatherer_messages.h"

namespace PropertyGatherer
{
    enum class PropertyConsumerState_E
    {
        INITIALIZED,
        PROPERTY_LIST_REQUESTED,
        PROPERTY_LIST_RECEIVED,
        PROPERTY_VALUE_REQUESTED,
        PROPERTY_VALUE_RECEIVED,
    };

    enum class PropertyConsumerReqStatus_E
    {
        SUCCESS,
        FAILED_TO_SEND_MSG,
        ERROR,
    };

    class PropertyConsumer_C
    {
        public:
            PropertyConsumer_C();
            ~PropertyConsumer_C();

            void PropertyConsumer_ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size);
            void PropertyConsumer_ProcessTimedActivities();

            PropertyConsumerReqStatus_E PropertyConsumer_SendPropertyListRequest();
            PropertyConsumerReqStatus_E PropertyConsumer_SendGetValueRequest();
        private:
            uint16_t GetCurrentTimeMs();
            void ProcessStateMachine();
            void ProcessReceivedMessageQueue();
            void ProcessReceivedPropertyListReply(Message_T& msg);
            void ProcessReceivedPropertyGetValueReply(Message_T& msg);


            /* Callbacks */
            virtual void PropertyConsumer_OnPropertyListReplyReceived() = 0;
            virtual void PropertyConsumer_OnPropertyGetValueReplyRecieved() = 0;

            /* Member Variables */
            PropertyConsumerState_E _state = PropertyConsumerState_E::INITIALIZED;
            ThreadSafeQueue_C<Message_T> _msgQueue;
            PropertyGathererMessageSerializer _msgSerializer;
            bool _propertyListRequested = false;
            bool _propertyListReceived = false;

    };
}

#endif // PROPERTY_CONSUMER_API_H