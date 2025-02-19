/* Project includes */
#include "stdafx.h"
#include "button_widget.h"

ButtonWidget_C::ButtonWidget_C()
{

}

ButtonWidget_C::~ButtonWidget_C()
{

}

void ButtonWidget_C::ShowWidget()
{
    // ShowWidget override
    if (ImGui::Button(_buttonLabel.c_str())) 
    {
        // Do button stuff 
        // I am not 100% sure how to go about this at the moment, but when the button is clicked it returns true, so button suff goes here
        
    }
}

bool ButtonWidget_C::SetWidgetValue(const char* label)
{
    if (!label)
    {
        _buttonLabel.clear();
        return false;
    }
    _buttonLabel = label;
    return true;
}
