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
#include <functional>

/* Shared Includes */
#include "thread_safe_queue.h"
#include "custom_types.h"

/* Project Includes */
#include "property_gatherer_messages.h"

namespace PropertyGatherer
{
    enum class PropertyConsumerState_E
    {
        INITIALIZED,
        PROPERTY_LIST_REQUESTED,
        PROPERTY_LIST_RECEIVED,
        READY,
        PROPERTY_VALUE_REQUESTED,
        PROPERTY_VALUE_RECEIVED,
    };
    enum class PropertyConsumerReqStatus_E
    {
        PROPERTY_CONSUMER_STATUS_SUCCESS,
        PROPERTY_CONSUMER_STATUS_FAILED_TO_SEND_MSG,
        PROPERTY_CONSUMER_STATUS_ERROR,
    };
    struct PropertyConsumerCallbacks_T 
    {
        std::function<int32_t(const std::vector<uint8_t>&)> sendMessage;
        std::function<void(PropertyReplyStatus_E, std::vector<PropertyDescriptor_T>&)> onPropertyListReplyReceived;
        std::function<void(PropertyReplyStatus_E, std::vector<PropertyStorageVariant>&)> onPropertyGetValueReplyRecieved;
    };

    class PropertyConsumer_C
    {
        public:
            PropertyConsumer_C();
            ~PropertyConsumer_C();

            void ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size);
            void ProcessTimedActivities();
            void SetCallbacks(const PropertyConsumerCallbacks_T& callbacks);

            PropertyConsumerReqStatus_E SendPropertyListRequest();
            PropertyConsumerReqStatus_E SendGetValueRequest(uint16_t maxResponseLength, std::vector<uint16_t> propertyIds);

        private:
            uint64_t GetCurrentTimeMs();
            void ProcessStateMachine();
            void ProcessReceivedMessageQueue();
            void ProcessReceivedPropertyListReply(Message_T& msg);
            void ProcessReceivedPropertyGetValueReply(Message_T& msg);

            /* Callbacks */
            std::function<int32_t (const std::vector<uint8_t>&)> SendMessage;
            std::function<void(PropertyReplyStatus_E, std::vector<PropertyDescriptor_T>&)> OnPropertyListReplyReceived;
            std::function<void(PropertyReplyStatus_E, std::vector<PropertyStorageVariant>&)> OnPropertyGetValueReplyRecieved;

            /* Member Variables */
            PropertyConsumerState_E _state = PropertyConsumerState_E::INITIALIZED;
            ThreadSafeQueue_C<Message_T> _msgQueue;
            PropertyGathererMessageSerializer _msgSerializer;
            bool _propertyListRequested = false;
            bool _propertyListReceived = false;

    };
}

#endif // PROPERTY_CONSUMER_API_H