/* Project includes */
#include "stdafx.h"
#include "gui_window.h"

GuiWindow_C::GuiWindow_C(std::string windowName, uint16_t windowId) : _windowName(windowName), _windowId(windowId)
{

}

GuiWindow_C::~GuiWindow_C()
{

}

uint16_t GuiWindow_C::AddWidget(const std::shared_ptr<WidgetInterface_I>& widget)
{
    uint16_t retVal = _widgetKeyCount;
    _widgetList[_widgetKeyCount] = widget;
    _widgetKeyCount++;
    return retVal;
}

bool GuiWindow_C::ShowWindow() 
{
    bool retVal = false;
    if (true == ImGui::Begin(_windowName.c_str()))
    {
        for (const auto& [key, widget] : _widgetList)
        {
            widget->ShowWidget();
        }
        ImGui::End();
        retVal = true;
    }
    return retVal;
}

bool GuiWindow_C::GetWidgetAt(uint16_t key, std::shared_ptr<WidgetInterface_I>& outWidget)
{
    bool retVal = false;
    auto it = _widgetList.find(key);
    if (it != _widgetList.end())
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