/* Project includes */
#include "stdafx.h"
#include "gui_window.h"

GuiWindow_C::GuiWindow_C(std::string windowName, uint16_t windowId) : _windowName(windowName), _windowId(windowId)
{

}

GuiWindow_C::~GuiWindow_C()
{

}

std::shared_ptr<WidgetInterface_I> GuiWindow_C::AddWidget(std::shared_ptr<AddWidgetInfo_T> widgetInfo, ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue)
{
    widgetInfo->windowId = _windowId;
    widgetInfo->widgetId = _generalWidgetCount;
    auto widget = _widgetFactory.CreateWidget(widgetInfo, eventQueue);

    _generalWidgetList[_generalWidgetCount] = widget;

    if (WidgetTypes_E::Menu == widget->GetType())
    {
        _menuList[_menuCount] = widget;
        _menuCount++;

        // Add all menu items to the general widget list
        for (const auto& menuItem : std::dynamic_pointer_cast<WidgetMenu_C>(widget)->GetMenuItems())
        {
            _generalWidgetList[_generalWidgetCount] = menuItem;
            _generalWidgetCount++;
        }
    }
    else
    {
        _widgetList[_widgetCount] = widget;
        _widgetCount++;
    }
    
    _generalWidgetCount++;
    return widget;
}

bool GuiWindow_C::ShowWindow() 
{
    bool retVal = false;
    ImGuiWindowFlags flags = 0;
    if (0 != _menuCount)
    {
        flags |= ImGuiWindowFlags_MenuBar;
    }
    if (true == ImGui::Begin(_windowName.c_str(), nullptr, flags))
    {
        // Create a Menu bar if this window has a Menu
        if (0 != _menuCount && ImGui::BeginMenuBar())
        {
            for (const auto& [key, menu] : _menuList)
            {
                menu->ShowWidget();
            }
            ImGui::EndMenuBar();
        }
        for (const auto& [key, widget] : _widgetList)
        {
            widget->ShowWidget();
        }
        ImGui::End();
        retVal = true;
    }
    else {
        ImGui::End();
    }
    return retVal;
}

bool GuiWindow_C::GetWidgetAt(uint16_t key, std::shared_ptr<WidgetInterface_I>& outWidget)
{
    bool retVal = false;
    auto it = _generalWidgetList.find(key);

    if (it != _generalWidgetList.end())
    {
        outWidget = it->second;
        retVal = true;
    }
    else
    {
        outWidget = nullptr;
    }
    return retVal;
}