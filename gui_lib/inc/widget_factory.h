#ifndef WIDGET_FACTORY_H
#define WIDGET_FACTORY_H

#include "stdafx.h"
#include "widget_button.h"
#include "widget_checkbox.h"
#include "widget_radio.h"
#include "widget_slider.h"
#include "widget_text.h"
#include "widget_menu.h"
#include "widget_menu_item.h"
#include "widget_image.h"
#include "widget_input_text.h"

using ParseFunc = std::function<std::shared_ptr<AddWidgetInfo_T>(const nlohmann::json&)>;

class WidgetFactory_C
{
    public:
        std::shared_ptr<WidgetInterface_I> CreateWidget(std::shared_ptr<AddWidgetInfo_T> info);
        std::shared_ptr<AddWidgetInfo_T> ParseWidgetData(const nlohmann::json& widgetData);

    private:
        std::shared_ptr<AddWidgetInfo_T> ParseTextWidgetData(const nlohmann::json& widgetData);
        std::shared_ptr<AddWidgetInfo_T> ParseButtonWidgetData(const nlohmann::json& widgetData);
        std::shared_ptr<AddWidgetInfo_T> ParseSliderWidgetData(const nlohmann::json& widgetData);
        std::shared_ptr<AddWidgetInfo_T> ParseCheckboxWidgetData(const nlohmann::json& widgetData);
        std::shared_ptr<AddWidgetInfo_T> ParseRadioWidgetData(const nlohmann::json& widgetData);
        std::shared_ptr<AddWidgetInfo_T> ParseMenuWidgetData(const nlohmann::json& widgetData);
        std::shared_ptr<AddWidgetInfo_T> ParseImageWidgetData(const nlohmann::json& widgetData);
        std::shared_ptr<AddWidgetInfo_T> ParseInputTextWidgetData(const nlohmann::json& widgetData);

        const std::unordered_map<WidgetTypes_E, ParseFunc> _factoryMap {
            {WidgetTypes_E::Text, std::bind(&WidgetFactory_C::ParseTextWidgetData, this, std::placeholders::_1)},
            {WidgetTypes_E::Button, std::bind(&WidgetFactory_C::ParseButtonWidgetData, this, std::placeholders::_1)},
            {WidgetTypes_E::Slider, std::bind(&WidgetFactory_C::ParseSliderWidgetData, this, std::placeholders::_1)},
            {WidgetTypes_E::Checkbox, std::bind(&WidgetFactory_C::ParseCheckboxWidgetData, this, std::placeholders::_1)},
            {WidgetTypes_E::Radio, std::bind(&WidgetFactory_C::ParseRadioWidgetData, this, std::placeholders::_1)},
            {WidgetTypes_E::Menu, std::bind(&WidgetFactory_C::ParseMenuWidgetData, this, std::placeholders::_1)},
            {WidgetTypes_E::Image, std::bind(&WidgetFactory_C::ParseImageWidgetData, this, std::placeholders::_1)},
            {WidgetTypes_E::InputText, std::bind(&WidgetFactory_C::ParseInputTextWidgetData, this, std::placeholders::_1)}
        };
};

#endif // WIDGET_FACTORY_H