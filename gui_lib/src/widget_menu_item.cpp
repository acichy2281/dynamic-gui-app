/* Project includes */
#include "stdafx.h"
#include "widget_menu_item.h"

WidgetMenuItem_C::WidgetMenuItem_C(const std::shared_ptr<const AddMenuItemWidgetInfo_T>& info) : WidgetInterface_I(info) 
{

}

WidgetMenuItem_C::~WidgetMenuItem_C()
{

}

void WidgetMenuItem_C::ShowWidget()
{
    if (ImGui::MenuItem(_widgetName.c_str())) 
    {
        std::cout << "Menu Item Selected! Window ID: " << _windowId << " Widget ID: " << _widgetId << "\n";
        auto event = std::make_shared<EventButtonPress_C>(_windowId, _widgetId);
        EventQueue()->Enqueue(std::move(event));
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