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
        INITIALIZED,
        PROPERTY_LIST_POPULATED,
        PROPERTY_LIST_REPLY_SENT,
    };
    enum class PropertyProducerRepStatus_E
    {
        SUCCESS,
        FAILED_TO_SEND_MSG,
        ERROR,
    };

    class PropertyProducer_C
    {
        public:
            PropertyProducer_C();
            ~PropertyProducer_C();

            void PropertyProducer_ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size);
            void PropertyProducer_ProcessTimedActivities();

            bool SetPropertyList(std::vector<PropertyDescriptor_T>& descList, std::vector<PropertyStorageVariant>& values);

            /* Callbacks */
            std::function<int32_t(const std::vector<uint8_t>&)> SendMessage;
            std::function<void(PropertyReplyStatus_E, std::vector<PropertyDescriptor_T>&)> OnPropertyListRequestReceived;
            std::function<PropertyReplyStatus_E(std::vector<PropertyStorageVariant>&)> OnPropertyGetValueRequestRecieved;

        private:
            uint64_t GetCurrentTimeMs();
            void ProcessStateMachine();
            void ProcessReceivedMessageQueue();
            void ProcessReceivedPropertyListRequest();
            void ProcessReceivedPropertyGetValueRequest(Message_T& msg);


            /* Member variables */
            PropertyProducerState_E _state = PropertyProducerState_E::INITIALIZED;
            ThreadSafeQueue_C<Message_T> _msgQueue;
            PropertyGathererMessageSerializer _msgSerializer;
            std::map<uint16_t, PropertyDescriptor_T> _propertyMap;
            std::map<uint16_t, PropertyStorageVariant> _propertyValues;
            bool _propertListPopulated = false;
            bool _propertyListReplySent = false;
            bool _propertyGetValueReplySent = false;

    };
}

#endif // PROPERTY_PRODUCER_API_H