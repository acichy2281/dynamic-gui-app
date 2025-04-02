/* Project includes */
#include "stdafx.h"
#include "widget_radio.h"

WidgetRadio_C::WidgetRadio_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, 
                             std::vector<std::string> optionList) : 
    _eventQueue(eventQueue), 
    _optionsList(optionList) {}
    
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

WidgetValueVariant_T WidgetRadio_C::GetWidgetValue()
{
    // Return the currently selected option index
    return _selectedOptionIndex;
}

bool WidgetRadio_C::SetWidgetValue(WidgetValueVariant_T val)
{
    if (false == _isWritable) 
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

void WidgetRadio_C::SetFlags(uint8_t flags)
{
    if (flags & WidgetFlags_E::Readable)
    {
        _isReadable = true;
    }
    if (flags & WidgetFlags_E::Writeable)
    {
        _isWritable = true;
    }
    if (flags & WidgetFlags_E::Interactable)
    {
        _isInteractable = true;
    }
    if (flags & WidgetFlags_E::Static)
    {
        _isStatic = true;
    }
}

WidgetDescriptor_T WidgetRadio_C::GetDescriptor() 
{
    uint8_t flags = 0;
    if (_isReadable)
    {
        flags |= WidgetFlags_E::Readable;
    }
    if (_isWritable)
    {
        flags |= WidgetFlags_E::Writeable;
    }
    if (_isInteractable)
    {
        flags |= WidgetFlags_E::Interactable;
    }
    if (_isStatic)
    {
        flags |= WidgetFlags_E::Static;
    }
    
    return GuiProtocol::GetWidgetDescriptor(
                           GetWindowId(), 
                           GetWidgetId(), 
                           flags,
                           WidgetTypes_E::Radio,
                           WidgetDataTypes_E::Int,
                           GetWidgetName());
}

void WidgetRadio_C::AddOption(std::string& newOption)
{
    _optionsList.push_back(newOption);
}

WidgetTypes_E WidgetRadio_C::GetType()
{
    return WidgetTypes_E::Radio;
}