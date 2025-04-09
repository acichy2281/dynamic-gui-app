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
        WidgetRadio_C(const std::shared_ptr<const AddRadioWidgetInfo_T>& info);
        ~WidgetRadio_C();

        void ShowWidget() override;
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;
        WidgetTypes_E GetWidgetType() override { return WidgetTypes_E::Radio; }
        WidgetDataTypes_E GetDataType() override { return WidgetDataTypes_E::Int; } // Radio button returns an integer index of the selected option

        void AddOption(const std::string& newOption);
        
    private:
        uint16_t _id;
        std::vector<std::string> _optionsList;
        int32_t _selectedOptionIndex = 0;
};
#endif // WIDGET_RADIO_H