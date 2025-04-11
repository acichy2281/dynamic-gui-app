/* Project includes */
#include "stdafx.h"
#include "widget_menu.h"

WidgetMenu_C::WidgetMenu_C(const std::shared_ptr<const AddMenuWidgetInfo_T>& info) : WidgetInterface_I(info) 
{
    _isWritable = false;
    _isReadable = false;
    _isInteractable = true;
    _isStatic = false;
}

WidgetMenu_C::~WidgetMenu_C()
{

}

void WidgetMenu_C::ShowWidget()
{
    // Create a dropdown menu
    if (ImGui::BeginMenu(_widgetName.c_str()))
    {
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