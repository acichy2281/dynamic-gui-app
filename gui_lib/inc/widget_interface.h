#ifndef WIDGET_INTERFACE_H
#define WIDGET_INTERFACE_H

#include "stdafx.h"

struct AddWidgetInfo_T
{
    uint16_t windowId;
    uint16_t widgetId;
    uint8_t flags;
    WidgetTypes_E type;
    WidgetDataTypes_E dataType;
    std::string widgetName;
    WidgetValueVariant_T defaultValue;
    virtual ~AddWidgetInfo_T() = default;
};

class WidgetInterface_I 
{
    public:
        virtual ~WidgetInterface_I() {}
        
        virtual void ShowWidget() = 0;

        virtual WidgetTypes_E GetType() = 0;
        void SetWindowId(uint16_t windowId) { _windowId = windowId; };
        void SetWidgetId(uint16_t widgetId) { _widgetId = widgetId; };
        uint16_t GetWindowId() const { return _windowId; };
        uint16_t GetWidgetId() const { return _widgetId; };
        std::string GetWidgetName() { return _widgetName; };
        void SetWidgetName(std::string widgetName) { _widgetName = widgetName; };
        virtual WidgetValueVariant_T GetWidgetValue() = 0; 
        virtual bool SetWidgetValue(WidgetValueVariant_T val) = 0;
        virtual void SetFlags(uint8_t flags) = 0;
        const nlohmann::json& GetJsonData() const { return _jsonData; }; // Get JSON data for the widget if needed
        void SetJsonData(const nlohmann::json& jsonData) { _jsonData = jsonData; }; // Set JSON data for the widget if needed
        virtual WidgetDescriptor_T GetDescriptor() = 0;

    protected:
        bool _isReadable = false;
        bool _isWritable = false;
        bool _isStatic = false;
        bool _isInteractable = false;

    private:
        uint16_t _windowId = 0; // Assigned by the window that contains this widget
        uint16_t _widgetId = 0; // Assigned by the window that contains this widget
        std::string _widgetName;
        nlohmann::json _jsonData;
};
#endif // WIDGET_INTERFACE_H