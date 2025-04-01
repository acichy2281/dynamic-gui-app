/* Project includes */
#include "stdafx.h"
#include "widget_ribbon.h"

WidgetRibbon_C::WidgetRibbon_C()
{
    this->_id = -1;
}

WidgetRibbon_C::~WidgetRibbon_C()
{

}

void WidgetRibbon_C::ShowWidget(){
    ImGui::Begin("Ribbon Toolbar", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_NoCollapse);

    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 30));

    bool firstButton = true;
    for (const auto& button : _buttons) {
        if (!firstButton) {
            ImGui::SameLine();
        }
        if (ImGui::Button(button.c_str())) 
        {
            std::cout << "Button Pressed: " << button << std::endl;
            // TODO: How do I send the event to the main window?
        }
        firstButton = false;
    }

    ImGui::End();
}

bool WidgetRibbon_C::SetRibbonButtons(const std::vector<std::string>& buttonNames)
{
    if (buttonNames.empty())
    {
        _buttons.clear();
        return false;
    }
    _buttons = buttonNames;
    return true;
}

WidgetTypes_E WidgetRibbon_C::GetType()
{
    return WidgetTypes_E::RIBBON;
}

void WidgetRibbon_C::AssignId(uint16_t widgetId)
{
    this->_id = widgetId;
}