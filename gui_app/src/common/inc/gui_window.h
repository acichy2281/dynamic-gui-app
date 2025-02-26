#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

/* Project Includes */
#include "stdafx.h"
#include "widget_interface.h"

class GuiWindow_C
{
    public:
        GuiWindow_C(std::string windowName, uint16_t windowId);
        ~GuiWindow_C();

        /**
         * @brief Adds a widget to this window to be drawn each frame
         * 
         * @param widget shared pointer to the widget 
         * @return uint16_t 
         * @retval returns a key value to access this widget via GetWidgetAt()
         */
        uint16_t AddWidget(const std::shared_ptr<WidgetInterface_I>& widget);

        /**
         * @brief Loop through all widgets in the widget list to call the widgets ShowWidget function  
         * 
         * @return true 
         * @return false 
         */
        bool ShowWindow();

        /**
         * @brief Get the Widget At object
         * 
         * @param[in] key (In) Looks up the widget in the map
         * @param[out] outWidget (Out) reference to the widget pointer
         * @return true 
         * @return false 
         * @retval returns false if the widget is not in the map 
         */
        bool GetWidgetAt(uint16_t key, std::shared_ptr<WidgetInterface_I>& outWidget);

        const uint16_t GetWindowId() { return _windowId; }

    private:
        std::string                                             _windowName;
        std::map<uint16_t, std::shared_ptr<WidgetInterface_I>>  _widgetList;
        uint16_t                                                _widgetKeyCount                 = 0;
        uint16_t                                                _windowId;
};
#endif // GUI_WINDOW_H