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

/* Project Includes */
#include "property_gatherer_messages.h"

namespace PropertyGatherer
{
    class PropertyProducer_C
    {
        public:
            PropertyProducer_C();
            ~PropertyProducer_C();

            virtual void PropertyProducer_OnPropertyListRequestRecieved() = 0;
            virtual void PropertyProducer_OnPropertyGetValueRequestRecieved() = 0;

        private:

    };
}

#endif // PROPERTY_PRODUCER_API_H