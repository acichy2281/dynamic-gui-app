#ifndef PROPERTY_PRODUCER_API_H
#define PROPERTY_PRODUCER_API_H

/* System Includes */
#include <string>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <memory>
#include <inttypes.h>
#include <map>
#include <functional>

/* Shared Includes */
#include "thread_safe_queue.h"
#include "custom_types.h"

/* Project Includes */
#include "property_gatherer_messages.h"

namespace PropertyGatherer
{
    enum class PropertyProducerState_E
    {
        Uninitialized,
        Initialized,
        Ready,
        Error,
    };
    enum class PropertyProducerStatus_E
    {
        Success,
        FailedToSendMsg,
        Error,
    };
    struct PropertyProducerCallbacks_T 
    {
        std::function<int32_t(const std::vector<uint8_t>&)> sendMessage;
        std::function<void(const std::vector<PropertyDescriptor_T>&)> onPropertyListRequestReceived;
        std::function<PropertyGathererReplyStatus_E(std::vector<PropertyValueContainer_T>&)> onPropertyGetValueRequestRecieved;
        std::function<PropertyGathererReplyStatus_E(PropertyValueContainer_T&)> onPropertySetValueRequestRecieved;
    };
    struct PropertyProducerInitParams_T
    {
        const std::vector<PropertyDescriptor_T>& propertyList;
        const PropertyProducerCallbacks_T& callbacks;
    };

    class PropertyProducer_C
    {
        public:
            PropertyProducer_C();
            ~PropertyProducer_C();

            PropertyProducerStatus_E PropertyProducer_Initialize(PropertyProducerInitParams_T& initParams);
            PropertyProducerStatus_E PropertyProducer_ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size);
            void PropertyProducer_ProcessTimedActivities();

        private:
            bool SetCallbacks(const PropertyProducerCallbacks_T& callbacks);
            bool SetPropertyList(const std::vector<PropertyDescriptor_T>& descList);
            uint64_t GetCurrentTimeMs();
            void ProcessStateMachine();
            void ProcessReceivedMessageQueue();
            void ProcessReceivedPropertyListRequest();
            void ProcessReceivedPropertyGetValueRequest(Message_T& msg);
            void ProcessReceivedPropertySetValueRequest(Message_T& msg);

            /* Callbacks */
            std::function<int32_t(const std::vector<uint8_t>&)> SendMessage;
            std::function<void(const std::vector<PropertyDescriptor_T>&)> OnPropertyListRequestReceived;
            std::function<PropertyGathererReplyStatus_E(std::vector<PropertyValueContainer_T>&)> OnPropertyGetValueRequestRecieved;
            std::function<PropertyGathererReplyStatus_E(PropertyValueContainer_T&)> OnPropertySetValueRequestRecieved;


            /* Member variables */
            PropertyProducerState_E _state = PropertyProducerState_E::Uninitialized;
            ThreadSafeQueue_C<Message_T> _msgQueue;
            PropertyGathererMessageSerializer _msgSerializer;
            std::map<uint16_t, PropertyDescriptor_T> _propertyMap;
            bool _initialized = false;
            bool _propertyListReplySent = false;
            bool _propertyGetValueReplySent = false;
            bool _propertySetValueReplySent = false;
            bool _errorOccured = false;
    };
}

#endif // PROPERTY_PRODUCER_API_H