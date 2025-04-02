#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

/* Project Includes */
#include "stdafx.h"
#include "widget_interface.h"
#include "widget_factory.h"

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
        std::shared_ptr<WidgetInterface_I> AddWidget(std::shared_ptr<AddWidgetInfo_T> widgetInfo, ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue);

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
        std::map<uint16_t, std::shared_ptr<WidgetInterface_I>>  _generalWidgetList;
        uint16_t                                                _generalWidgetCount             = 0;
        std::map<uint16_t, std::shared_ptr<WidgetInterface_I>>  _widgetList;
        uint16_t                                                _widgetCount                    = 0;
        std::map<uint16_t, std::shared_ptr<WidgetInterface_I>>  _menuList;
        uint16_t                                                _menuCount                      = 0;
        uint16_t                                                _windowId;
        WidgetFactory_C                                         _widgetFactory;
};
#endif // GUI_WINDOW_H