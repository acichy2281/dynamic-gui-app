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

    private:
        uint16_t _windowId; // Assigned by the window that contains this widget
        uint16_t _widgetId; // Assigned by the window that contains this widget
};
#endif // WIDGET_INTERFACE_H