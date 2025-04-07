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
        Uninitialized,
        Initialized,
        PropertyListRequested,
        Ready,
        PropertyGetValReqSent,
        PropertySetValReqSent,
        Error,
    };
    enum class PropertyConsumerStatus_E
    {
        Success,
        NotInitialized,
        PropertyListNotReceived,
        FailedToSendMsg,
        RequestInProgress,
        Error,
    };
    struct PropertyConsumerCallbacks_T 
    {
        std::function<int32_t(const std::vector<uint8_t>&)> sendMessage;
        std::function<void(PropertyGathererReplyStatus_E, std::vector<PropertyDescriptor_T>&)> onPropertyListReplyReceived;
        std::function<void(PropertyGathererReplyStatus_E, std::vector<PropertyStorageVariant>&)> onPropertyGetValueReplyRecieved;
        std::function<void(PropertyGathererReplyStatus_E, PropertyStorageVariant&)> onPropertySetValueReplyRecieved;
    };
    struct PropertyConsumerInitParams_T
    {
        const PropertyConsumerCallbacks_T& callbacks;
    };

    class PropertyConsumer_C
    {
        public:
            PropertyConsumer_C();
            ~PropertyConsumer_C();

            PropertyConsumerStatus_E PropertyConsumer_Initialize(PropertyConsumerInitParams_T& initParams);
            PropertyConsumerStatus_E ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size);
            void ProcessTimedActivities();

            PropertyConsumerStatus_E SendPropertyListRequest();
            PropertyConsumerStatus_E SendGetValueRequest(uint16_t maxResponseLength, std::vector<uint16_t> propertyIds);
            PropertyConsumerStatus_E SendSetValueRequest(uint16_t propertyId, PropertyStorageVariant& value);

        private:
            bool SetCallbacks(const PropertyConsumerCallbacks_T& callbacks);
            uint64_t GetCurrentTimeMs();
            void ProcessStateMachine();
            void ProcessReceivedMessageQueue();
            void ProcessReceivedPropertyListReply(Message_T& msg);
            void ProcessReceivedPropertyGetValueReply(Message_T& msg);
            void ProcessReceivedPropertySetValueReply(Message_T& msg);

            /* Callbacks */
            std::function<int32_t (const std::vector<uint8_t>&)> SendMessage;
            std::function<void(PropertyGathererReplyStatus_E, std::vector<PropertyDescriptor_T>&)> OnPropertyListReplyReceived;
            std::function<void(PropertyGathererReplyStatus_E, std::vector<PropertyStorageVariant>&)> OnPropertyGetValueReplyRecieved;
            std::function<void(PropertyGathererReplyStatus_E, PropertyStorageVariant&)> OnPropertySetValueReplyRecieved;

            /* Member Variables */
            PropertyConsumerState_E _state = PropertyConsumerState_E::Uninitialized;
            ThreadSafeQueue_C<Message_T> _msgQueue;
            PropertyGathererMessageSerializer _msgSerializer;
            std::vector<PropertyDescriptor_T> _propertyDescriptorList;
            bool _initialized = false;
            bool _propertyListRequested = false;
            bool _propertyListReceived = false;
            bool _propertyGetValueReqSent = false;
            bool _propertyGetValueReplyReceived = false;
            bool _propertySetValueReqSent = false;
            bool _propertySetValueReplyReceived = false;
            bool _errorOccured = false;
    };
}

#endif // PROPERTY_CONSUMER_API_H