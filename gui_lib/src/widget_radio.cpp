/* Project includes */
#include "stdafx.h"
#include "widget_radio.h"

WidgetRadio_C::WidgetRadio_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, 
                             uint16_t windowId, 
                             std::vector<std::string> optionList) : 
    _eventQueue(eventQueue), 
    _optionsList(optionList)
{
    SetWindowId(windowId);
    SetIsStatic(false);
}
WidgetRadio_C::~WidgetRadio_C()
{

}

void WidgetRadio_C::ShowWidget()
{
    // ShowWidget override
    for (int i = 0; i < _optionsList.size(); i++){
        if(ImGui::RadioButton(_optionsList[i].c_str(), &_selectedOptionIndex, i))
        {
            std::cout << "Radio button selected! Window ID: " << GetWindowId() << " Widget ID: " << GetWidgetId() << " Option: " << i << "\n";
            auto event = std::make_shared<EventRadioSelected_C>(GetWindowId(), GetWidgetId(), i);
            _eventQueue.Enqueue(std::move(event));
        }
    }     
}

bool WidgetRadio_C::SetWidgetValue(WidgetValueVariant_T val)
{
    if (GetIsStatic()) 
    {
        return false;
    }
    else if (false == std::holds_alternative<int>(val))
    {
        return false;
    }
    else if (std::get<int>(val) < 0 || std::get<int>(val) >= _optionsList.size())
    {
        return false;
    }
    _selectedOptionIndex = std::get<int>(val);
    return true;
}

void WidgetRadio_C::AddOption(std::string& newOption)
{
    _optionsList.push_back(newOption);
}

WidgetTypes_E WidgetRadio_C::GetType()
{
    return WidgetTypes_E::RADIO;
}