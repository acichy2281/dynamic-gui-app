/* Project includes */
#include "stdafx.h"
#include "widget_menu_item.h"

WidgetMenuItem_C::WidgetMenuItem_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue) : _eventQueue(eventQueue)
{
    _isReadable = false;
    _isWritable = false;
    _isStatic = true;
    _isInteractable = true;
}

WidgetMenuItem_C::~WidgetMenuItem_C()
{

}

void WidgetMenuItem_C::ShowWidget()
{
    if (ImGui::MenuItem(GetWidgetName().c_str())) 
    {
        std::cout << "Menu Item Selected! Window ID: " << GetWindowId() << " Widget ID: " << GetWidgetId() << "\n";
        auto event = std::make_shared<EventButtonPress_C>(GetWindowId(), GetWidgetId());
        _eventQueue.Enqueue(std::move(event));
    }
}

WidgetValueVariant_T WidgetMenuItem_C::GetWidgetValue()
{
    // Menu item cannot have a value
    return false;
}

bool WidgetMenuItem_C::SetWidgetValue(WidgetValueVariant_T val)
{
    // Menu item cannot have a value
    return false;
}

void WidgetMenuItem_C::SetFlags(uint8_t flags)
{
    // Empty as menu flags are not changeable. 
}

WidgetDescriptor_T WidgetMenuItem_C::GetDescriptor()
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
                           WidgetTypes_E::MenuItem,
                           WidgetDataTypes_E::None,
                           GetWidgetName());
}

WidgetTypes_E WidgetMenuItem_C::GetType()
{
    return WidgetTypes_E::MenuItem;
}