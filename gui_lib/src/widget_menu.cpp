/* Project includes */
#include "stdafx.h"
#include "widget_menu.h"

WidgetMenu_C::WidgetMenu_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, 
                               std::vector<std::shared_ptr<WidgetInterface_I>> menuItems) : 
    _eventQueue(eventQueue),
    _menuItems(menuItems)
{
    _isReadable = false;
    _isWritable = false;
    _isStatic = true;
    _isInteractable = true;
}

WidgetMenu_C::~WidgetMenu_C()
{

}

void WidgetMenu_C::ShowWidget()
{
    // Create a dropdown menu
    if (ImGui::BeginMenu(GetWidgetName().c_str()))
    {
        // for (const auto& [key, menuItem] :  _menuItemList)
        // {
        //     menuItem->ShowWidget();
        // }
        for (const auto& menuItem :  _menuItems)
        {
            menuItem->ShowWidget();
        }
        
        // End the dropdown menu
        ImGui::EndMenu();
    }
}

WidgetValueVariant_T WidgetMenu_C::GetWidgetValue()
{
    // Menu cannot have a value
    return false;
}

bool WidgetMenu_C::SetWidgetValue(WidgetValueVariant_T val)
{
    // Menu cannot have a value
    return false;
}

std::vector<std::shared_ptr<WidgetInterface_I>>& WidgetMenu_C::GetMenuItems()
{
    return _menuItems;
}

void WidgetMenu_C::SetFlags(uint8_t flags)
{
    // Empty as menu flags are not changeable. 
}

WidgetDescriptor_T WidgetMenu_C::GetDescriptor()
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
                           WidgetTypes_E::Menu,
                           WidgetDataTypes_E::None,
                           GetWidgetName());
}

WidgetTypes_E WidgetMenu_C::GetType()
{
    return WidgetTypes_E::Menu;
}