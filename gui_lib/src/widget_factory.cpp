#include "stdafx.h"
#include "widget_factory.h"
#include <nlohmann/json.hpp>

std::shared_ptr<WidgetInterface_I> WidgetFactory_C::CreateWidget(std::shared_ptr<AddWidgetInfo_T> info, ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue)
{
    std::shared_ptr<WidgetInterface_I> widget;
    switch (info->type)
    {
        case WidgetTypes_E::Text:
            std::cout << "Type of info: " << typeid(info).name() << std::endl;
            widget = std::make_shared<WidgetText_C>(info->windowId);
            break;
        case WidgetTypes_E::Button:
            widget = std::make_shared<WidgetButton_C>(eventQueue, info->windowId);
            break;
        case WidgetTypes_E::Slider:
            std::cout << "Type of info: " << typeid(info).name() << std::endl;
            if (const auto* sliderInfo = dynamic_cast<const AddSliderWidgetInfo_T*>(info.get())) 
            {
                widget = std::make_shared<WidgetSlider_C>(eventQueue, info->windowId, sliderInfo->sliderMin, sliderInfo->sliderMax);
            }
            else
            {
                throw std::invalid_argument("Slider widget info not provided");
            }
            break;
        case WidgetTypes_E::Checkbox:
            widget = std::make_shared<WidgetCheckbox_C>(eventQueue, info->windowId);
            break;
        case WidgetTypes_E::Radio:
            std::cout << "Type of info: " << typeid(info).name() << std::endl;
            if (const auto* radioInfo = dynamic_cast<const AddRadioWidgetInfo_T*>(info.get())) 
            {
                widget = std::make_shared<WidgetRadio_C>(eventQueue, info->windowId, radioInfo->radioWidgetOptionsList);
            }
            else
            {
                throw std::invalid_argument("Radio widget info not provided");
            }
            break;
        default:
            throw std::invalid_argument("Unknown widget type");
    }
    widget->SetWidgetName(info->widgetName);
    widget->SetFlags(info->flags);
    widget->SetWidgetValue(info->defaultValue);

    return widget;
}

std::shared_ptr<AddWidgetInfo_T> WidgetFactory_C::ParseWidgetData(const nlohmann::json& widgetData)
{
    std::shared_ptr<AddWidgetInfo_T> retVal;
    if (widgetData.find("Type") == widgetData.end() || widgetData.find("Name") == widgetData.end())
    {
        throw std::invalid_argument("Widget data missing required fields.");
    }
    auto type = widgetData["Type"].get<WidgetTypes_E>();
    
    /* Call factory function to parse widget specific data */
    auto it = _factoryMap.find(type);
    if (it != _factoryMap.end()) 
    {
        retVal = it->second(widgetData);
    } 
    else 
    {
        throw std::invalid_argument("Unknown object type in JSON.");
    }

    /* Parse Common Widget Fields */
    retVal->type = widgetData["Type"].get<WidgetTypes_E>();
    retVal->widgetName = widgetData["Name"].get<std::string>();
    if (widgetData.find("Value") != widgetData.end())
    {
        if (widgetData["Value"].is_string())
            retVal->defaultValue = widgetData["Value"].get<std::string>();
        else if (widgetData["Value"].is_boolean())
            retVal->defaultValue = widgetData["Value"].get<bool>();
        else if (widgetData["Value"].is_number_integer())
            retVal->defaultValue = widgetData["Value"].get<int>();
        else if (widgetData["Value"].is_number_float())
            retVal->defaultValue = widgetData["Value"].get<float>();
    }
    if (widgetData.find("Static") != widgetData.end() && widgetData["Static"].get<bool>())
    {
        retVal->flags |= WidgetFlags_E::Static;
    }

    return retVal;
}

std::shared_ptr<AddWidgetInfo_T> WidgetFactory_C::ParseTextWidgetData(const nlohmann::json& widgetData)
{
    auto retVal = std::make_unique<AddWidgetInfo_T>();
    retVal->dataType = GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_STRING;
    retVal->flags |= WidgetFlags_E::Readable;
    retVal->flags |= WidgetFlags_E::Writeable;
    return retVal;
}

std::shared_ptr<AddWidgetInfo_T> WidgetFactory_C::ParseButtonWidgetData(const nlohmann::json& widgetData)
{
    auto retVal = std::make_unique<AddWidgetInfo_T>();
    retVal->dataType = GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_BOOL;
    retVal->flags |= WidgetFlags_E::Readable;
    return retVal;
}

std::shared_ptr<AddWidgetInfo_T> WidgetFactory_C::ParseSliderWidgetData(const nlohmann::json& widgetData)
{
    auto retVal = std::make_unique<AddSliderWidgetInfo_T>();
    if (widgetData.find("MinValue") == widgetData.end() || widgetData.find("MaxValue") == widgetData.end())
    {
        throw std::invalid_argument("Slider Widget data missing required fields.");
    }

    /* Parse Min and Max values as either an int or float */
    if (widgetData["MinValue"].is_number_integer()) {
        retVal->sliderMin = widgetData["MinValue"].get<int>();
    } else {
        retVal->sliderMin = widgetData["MinValue"].get<float>();
    }
    
    if (widgetData["MaxValue"].is_number_integer()) {
        retVal->sliderMax = widgetData["MaxValue"].get<int>();
    } else {
        retVal->sliderMax = widgetData["MaxValue"].get<float>();
    }

    /* Verify both are float or int */
    if (std::holds_alternative<int>(retVal->sliderMin) && std::holds_alternative<int>(retVal->sliderMax))
    {
        retVal->dataType = GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_INT;
    }
    else if (std::holds_alternative<float>(retVal->sliderMin) && std::holds_alternative<float>(retVal->sliderMax))
    {
        retVal->dataType = GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_FLOAT;
    }
    else
    {
        throw std::invalid_argument("Slider Widget data type mismatch.");
    }
    retVal->flags |= WidgetFlags_E::Readable;
    retVal->flags |= WidgetFlags_E::Writeable;
    retVal->flags |= WidgetFlags_E::Interactable;
    return retVal;
}

std::shared_ptr<AddWidgetInfo_T> WidgetFactory_C::ParseCheckboxWidgetData(const nlohmann::json& widgetData)
{
    auto retVal = std::make_unique<AddWidgetInfo_T>();
    retVal->dataType = GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_BOOL;
    retVal->flags |= WidgetFlags_E::Readable;
    retVal->flags |= WidgetFlags_E::Writeable;
    retVal->flags |= WidgetFlags_E::Interactable;
    return retVal;
}

std::shared_ptr<AddWidgetInfo_T> WidgetFactory_C::ParseRadioWidgetData(const nlohmann::json& widgetData)
{
    auto retVal = std::make_unique<AddRadioWidgetInfo_T>();
    if (widgetData.find("Options") == widgetData.end())
    {
        throw std::invalid_argument("Radio Widget data missing required fields.");
    }
    retVal->radioWidgetOptionsList = widgetData["Options"].get<std::vector<std::string>>();
    retVal->dataType = GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_INT;
    retVal->flags |= WidgetFlags_E::Readable;
    retVal->flags |= WidgetFlags_E::Writeable;
    retVal->flags |= WidgetFlags_E::Interactable;
    return retVal;
}
