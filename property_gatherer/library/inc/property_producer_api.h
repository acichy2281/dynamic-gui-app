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

    class PropertyProducer_C
    {
        public:
            PropertyProducer_C();
            ~PropertyProducer_C();

            void PropertyProducer_ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size);
            void PropertyProducer_ProcessTimedActivities();

            /* Callbacks */
            virtual void PropertyProducer_OnPropertyListRequestRecieved() = 0;
            virtual void PropertyProducer_OnPropertyGetValueRequestRecieved() = 0;

        private:
            uint16_t GetCurrentTimeMs();
            void ProcessStateMachine();
            void ProcessReceivedMessageQueue();
            void ProcessReceivedPropertyListRequest();
            void ProcessReceivedPropertyGetValueRequest(Message_T& msg);


            /* Member variables */
            PropertyProducerState_E _state = PropertyProducerState_E::INITIALIZED;
            ThreadSafeQueue_C<Message_T> _msgQueue;
            PropertyGathererMessageSerializer _msgSerializer;
            std::map<uint16_t, PropertyDescriptor_T> _propertyMap;

    };
}

#endif // PROPERTY_PRODUCER_API_H