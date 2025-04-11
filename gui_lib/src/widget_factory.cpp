#include "stdafx.h"
#include "widget_factory.h"
#include <nlohmann/json.hpp>

std::shared_ptr<WidgetInterface_I> WidgetFactory_C::CreateWidget(std::shared_ptr<AddWidgetInfo_T> info)
{
    std::shared_ptr<WidgetInterface_I> widget;
    switch (info->widgetType)
    {
        case WidgetTypes_E::Text:
            widget = std::make_shared<WidgetText_C>(info);
            break;
        case WidgetTypes_E::Button:
            widget = std::make_shared<WidgetButton_C>(info);
            break;
        case WidgetTypes_E::Slider:
        {
            auto sliderInfo = std::dynamic_pointer_cast<const AddSliderWidgetInfo_T>(info);
            if (sliderInfo) 
            {
                widget = std::make_shared<WidgetSlider_C>(sliderInfo);
            }
            else
            {
                throw std::invalid_argument("Slider widget info not provided");
            }
            break;
        }
        case WidgetTypes_E::Checkbox:
            widget = std::make_shared<WidgetCheckbox_C>(info);
            break;
        case WidgetTypes_E::Radio:
        {
            auto radioInfo = std::dynamic_pointer_cast<const AddRadioWidgetInfo_T>(info);
            if (radioInfo) 
            {
                widget = std::make_shared<WidgetRadio_C>(radioInfo);
            }
            else
            {
                throw std::invalid_argument("Radio widget info not provided");
            }
            break;
        }
        case WidgetTypes_E::Menu:
        {
            auto menuInfo = std::dynamic_pointer_cast<const AddMenuWidgetInfo_T>(info);
            if (menuInfo) 
            {
                auto menuWidget = std::make_shared<WidgetMenu_C>(menuInfo);
                uint8_t widgetIdOffset = 1; // Each Menu Item widget ID after the menu widget is incremented by 1
                for (const auto& menuItem : menuInfo->menuItems)
                {   
                    auto menuItemPtr = std::make_shared<AddMenuItemWidgetInfo_T>(menuItem);
                    menuItemPtr->windowId = menuInfo->windowId;
                    menuItemPtr->widgetId = menuInfo->widgetId + widgetIdOffset;
                    menuItemPtr->eventQueue = menuInfo->eventQueue;
                    widgetIdOffset++;
                    menuWidget->GetMenuItems().push_back(CreateWidget(menuItemPtr));
                }
                widget = menuWidget;
            }
            else
            {
                throw std::invalid_argument("Radio widget info not provided");
            }
            break;
        }
        case WidgetTypes_E::MenuItem:
            widget = std::make_shared<WidgetMenuItem_C>(
                        std::dynamic_pointer_cast<const AddMenuItemWidgetInfo_T>(info)
                     );
            break;
        case WidgetTypes_E::Image:
        {
            auto imageInfo = std::dynamic_pointer_cast<const AddImageWidgetInfo_T>(info);
            if (imageInfo) 
            {
                widget = std::make_shared<WidgetImage_C>(imageInfo);
            }
            else
            {
                throw std::invalid_argument("Radio widget info not provided");
            }
            break;
        }
        case WidgetTypes_E::InputText:
            widget = std::make_shared<WidgetInputText_C>(info);
            break;
        default:
            throw std::invalid_argument("Unknown widget type");
    }

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
    retVal->widgetName = widgetData["Name"].get<std::string>();
    retVal->widgetType = widgetData["Type"].get<WidgetTypes_E>();
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
    return retVal;
}

std::shared_ptr<AddWidgetInfo_T> WidgetFactory_C::ParseTextWidgetData(const nlohmann::json& widgetData)
{
    auto retVal = std::make_unique<AddWidgetInfo_T>();
    return retVal;
}

std::shared_ptr<AddWidgetInfo_T> WidgetFactory_C::ParseButtonWidgetData(const nlohmann::json& widgetData)
{
    auto retVal = std::make_unique<AddWidgetInfo_T>();
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
    if (false == (std::holds_alternative<int>(retVal->sliderMin) && std::holds_alternative<int>(retVal->sliderMax)) ||
        false == (std::holds_alternative<float>(retVal->sliderMin) && std::holds_alternative<float>(retVal->sliderMax)))
    {
        throw std::invalid_argument("Slider Widget data type mismatch.");
    }
    return retVal;
}

std::shared_ptr<AddWidgetInfo_T> WidgetFactory_C::ParseCheckboxWidgetData(const nlohmann::json& widgetData)
{
    auto retVal = std::make_unique<AddWidgetInfo_T>();
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
        menuItem.widgetType = WidgetTypes_E::MenuItem;
        retVal->menuItems.push_back(menuItem);
    }
    return retVal;
}

std::shared_ptr<AddWidgetInfo_T> WidgetFactory_C::ParseImageWidgetData(const nlohmann::json& widgetData)
{
    auto retVal = std::make_unique<AddImageWidgetInfo_T>();
    if (widgetData.find("ImagePath") == widgetData.end())
    {
        throw std::invalid_argument("Image Widget data missing required fields.");
    }
    retVal->imagePath = widgetData["ImagePath"].get<std::string>();
    return retVal;
}

std::shared_ptr<AddWidgetInfo_T> WidgetFactory_C::ParseInputTextWidgetData(const nlohmann::json& widgetData)
{
    auto retVal = std::make_unique<AddWidgetInfo_T>();
    return retVal;
}