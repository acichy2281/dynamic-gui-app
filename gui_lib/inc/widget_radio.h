#ifndef WIDGET_RADIO_H
#define WIDGET_RADIO_H

#include "stdafx.h"
#include "widget_interface.h"
#include "event_radio_selected.h"
#include "thread_safe_queue.h"

struct AddRadioWidgetInfo_T : public AddWidgetInfo_T {
    std::vector<std::string> radioWidgetOptionsList;
};

class WidgetRadio_C : public WidgetInterface_I
{
    public:
        WidgetRadio_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, std::vector<std::string> optionsList);
        ~WidgetRadio_C();
        void ShowWidget() override;
        
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;
        void AddOption(std::string& newOption);
        WidgetTypes_E GetType() override;
        void SetFlags(uint8_t flags) override;
        WidgetDescriptor_T GetDescriptor() override;

    private:
        uint16_t _id;
        std::vector<std::string> _optionsList;
        ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& _eventQueue;
        int _selectedOptionIndex = 0;
};
#endif // WIDGET_RADIO_H