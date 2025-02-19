/* Project includes */
#include "stdafx.h"
#include "widget_button.h"

WidgetButton_C::WidgetButton_C()
{

}

WidgetButton_C::~WidgetButton_C()
{

}

WidgetTypes_E WidgetButton_C::GetType()
{
    return WidgetTypes_E::BUTTON;
}

void WidgetButton_C::ShowWidget()
{
    // ShowWidget override
    if (ImGui::Button(_buttonLabel.c_str())) 
    {
        // Do button stuff 
        // I am not 100% sure how to go about this at the moment, but when the button is clicked it returns true, so button suff goes here
        
    }
}

bool WidgetButton_C::SetWidgetValue(const char* label)
{
    if (!label)
    {
        _buttonLabel.clear();
        return false;
    }
    _buttonLabel = label;
    return true;
}
