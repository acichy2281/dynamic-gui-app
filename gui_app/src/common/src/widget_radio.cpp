/* Project includes */
#include "stdafx.h"
#include "widget_radio.h"

WidgetRadio_C::WidgetRadio_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, uint16_t windowId) : _eventQueue(eventQueue)
{
    SetWindowId(windowId);
    this->_selected = 0;
}

WidgetRadio_C::~WidgetRadio_C()
{

}

void WidgetRadio_C::ShowWidget()
{
    // ShowWidget override
    for (int i = 0; i < _options.size(); i++){
        if(ImGui::RadioButton(_options[i].c_str(), &_selected, i))
        {
            std::cout << "Radio button selected! Window ID: " << GetWindowId() << " Widget ID: " << GetWidgetId() << " Option: " << i << "\n";
            auto event = std::make_shared<EventRadioSelected_C>(GetWindowId(), GetWidgetId(), i);
            _eventQueue.Enqueue(std::move(event));
        }
    }     
}

bool WidgetRadio_C::SetWidgetValue(std::vector<std::string> options, int selected)
{
    if (options.empty() || selected < 0 || selected >= static_cast<int>(options.size()))
    {
        _options.clear();
        _selected = 0;
        return false;
    }
    _options = options;
    _selected = selected;
    return true;
}

WidgetTypes_E WidgetRadio_C::GetType()
{
    return WidgetTypes_E::RADIO;
}