#include "stdafx.h"
#include "widget_factory.h"
#include <nlohmann/json.hpp>

std::shared_ptr<WidgetInterface_I> WidgetFactory_C::CreateWidget(std::shared_ptr<AddWidgetInfo_T> info, ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue)
{
    std::shared_ptr<WidgetInterface_I> widget;
    switch (info->type)
    {
        case WidgetTypes_E::Text:
            widget = std::make_shared<WidgetText_C>();
            break;
        case WidgetTypes_E::Button:
            widget = std::make_shared<WidgetButton_C>(eventQueue);
            break;
        case WidgetTypes_E::Slider:
            if (const auto* sliderInfo = dynamic_cast<const AddSliderWidgetInfo_T*>(info.get())) 
            {
                widget = std::make_shared<WidgetSlider_C>(eventQueue, sliderInfo->sliderMin, sliderInfo->sliderMax);
            }
            else
            {
                throw std::invalid_argument("Slider widget info not provided");
            }
            break;
        case WidgetTypes_E::Checkbox:
            widget = std::make_shared<WidgetCheckbox_C>(eventQueue);
            break;
        case WidgetTypes_E::Radio:
            if (const auto* radioInfo = dynamic_cast<const AddRadioWidgetInfo_T*>(info.get())) 
            {
                widget = std::make_shared<WidgetRadio_C>(eventQueue, radioInfo->radioWidgetOptionsList);
            }
            else
            {
                throw std::invalid_argument("Radio widget info not provided");
            }
            break;
        case WidgetTypes_E::Menu:
            if (const auto* menuInfo = dynamic_cast<const AddMenuWidgetInfo_T*>(info.get())) 
            {
                std::vector<std::shared_ptr<WidgetInterface_I>> menuItems;
                uint8_t widgetIdOffset = 1; // Each Menu Item widget ID after the menu widget is incremented by 1
                for (const auto& menuItem : menuInfo->menuItems)
                {   
                    auto menuItemPtr = std::make_shared<AddWidgetInfo_T>(menuItem);
                    menuItemPtr->windowId = menuInfo->windowId;
                    menuItemPtr->widgetId = menuInfo->widgetId + widgetIdOffset;
                    widgetIdOffset++;
                    menuItems.push_back(CreateWidget(menuItemPtr, eventQueue));
                }
                widget = std::make_shared<WidgetMenu_C>(eventQueue, menuItems);
            }
            else
            {
                throw std::invalid_argument("Radio widget info not provided");
            }
            break;
        case WidgetTypes_E::MenuItem:
            widget = std::make_shared<WidgetMenuItem_C>(eventQueue);
            break;
        default:
            throw std::invalid_argument("Unknown widget type");
    }
    widget->SetWindowId(info->windowId);
    widget->SetWidgetId(info->widgetId);
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
    retVal->dataType = WidgetDataTypes_E::String;
    retVal->flags |= WidgetFlags_E::Readable;
    retVal->flags |= WidgetFlags_E::Writeable;
    return retVal;
}

std::shared_ptr<AddWidgetInfo_T> WidgetFactory_C::ParseButtonWidgetData(const nlohmann::json& widgetData)
{
    auto retVal = std::make_unique<AddWidgetInfo_T>();
    retVal->dataType = WidgetDataTypes_E::Bool;
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
        retVal->dataType = WidgetDataTypes_E::Int;
    }
    else if (std::holds_alternative<float>(retVal->sliderMin) && std::holds_alternative<float>(retVal->sliderMax))
    {
        retVal->dataType = WidgetDataTypes_E::Float;
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
    retVal->dataType = WidgetDataTypes_E::Bool;
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
    retVal->dataType = WidgetDataTypes_E::Int;
    retVal->flags |= WidgetFlags_E::Readable;
    retVal->flags |= WidgetFlags_E::Writeable;
    retVal->flags |= WidgetFlags_E::Interactable;
    return retVal;
}

std::shared_ptr<AddWidgetInfo_T> WidgetFactory_C::ParseMenuWidgetData(const nlohmann::json& widgetData)
{
    auto retVal = std::make_unique<AddMenuWidgetInfo_T>();
    if (widgetData.find("Options") == widgetData.end())
    {
        throw std::invalid_argument("Menu Widget data missing required fields.");
    }
    auto items = widgetData["Options"].get<std::vector<std::string>>();
    for (const auto& item : items)
    {
        AddMenuItemWidgetInfo_T menuItem;
        menuItem.widgetName = item;
        menuItem.type = WidgetTypes_E::MenuItem;
        menuItem.flags |= WidgetFlags_E::Readable;
        menuItem.flags |= WidgetFlags_E::Writeable;
        menuItem.dataType = WidgetDataTypes_E::None;
        retVal->menuItems.push_back(menuItem);
    }
    retVal->dataType = WidgetDataTypes_E::None;
    return retVal;
}
