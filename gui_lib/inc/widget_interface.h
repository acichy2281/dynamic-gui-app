#ifndef WIDGET_INTERFACE_H
#define WIDGET_INTERFACE_H

#include "stdafx.h"

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
        bool GetIsStatic() { return _isStatic; };
        void SetIsStatic(bool isStatic) { _isStatic = isStatic; };

    private:
        uint16_t _windowId; // Assigned by the window that contains this widget
        uint16_t _widgetId; // Assigned by the window that contains this widget
        std::string _widgetName;
        bool _isStatic; // Field assigned at construction
};
#endif // WIDGET_INTERFACE_H