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
#include "property_gatherer_messages.h"

namespace PropertyGatherer
{
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

            PropertyConsumerReqStatus_E PropertyConsumer_SendPropertyListRequest();
            PropertyConsumerReqStatus_E PropertyConsumer_SendGetValueRequest();
        private:
            PropertyGathererMessageSerializer _msgSerializer; 

            /* Callbacks */
            virtual void PropertyConsumer_OnPropertyListReplyReceived() = 0;
            virtual void PropertyConsumer_OnPropertyGetValueReplyRecieved() = 0;
    };
}

#endif // PROPERTY_CONSUMER_API_H