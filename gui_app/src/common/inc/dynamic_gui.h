#ifndef DYNAMIC_GUI_H
#define DYNAMIC_GUI_H

#include "stdafx.h"
#include "gui_window.h"
#include "event_interface.h"
#include "widget_text.h"
#include "widget_text.h"
#include "widget_button.h"
#include "widget_slider.h"


class DynamicGui_C
{
    public: 
        DynamicGui_C();
        ~DynamicGui_C();

        /**
         * @brief Initializes and runs the GUI app blocking function that exits upon window close. 
         * 
         * @return true 
         * @return false 
         */
        bool Run();

        /**
         * @brief Set the Config File
         * 
         * @param configFilePath absolute file path to a config file 
         * 
         * @return true 
         * @return false 
         */
        bool SetConfigFile(const std::string& configFilePath);

    private: 
        /* Functions */

        /**
         * @brief Initializes SDL backend for GUI app
         * 
         */
        bool Initialize();

        /**
         * @brief Displays the GUI window
         * 
         * @return true 
         * @return false 
         */
        bool ShowGui();

        /**
         * @brief Parses json data from the member variable _jsonData
         * 
         */
        void ParseJsonData();

        /**
         * @brief Performs cleanup of GUI app backend resources
         * 
         */
        void DeInitialize();


        /* Gui Server functions  */
        void RunGuiServer();
        void GuiServer_OnWidgetListRequestReceived();
        int32_t GuiServer_SendMessage(const std::vector<uint8_t>& message);
        GuiProtocol::WidgetReplyStatus_E GuiServer_OnWidgetSetValueRequestReceived(std::vector<GuiProtocol::WidgetSetValueResponseReturn_T>& widgetSetValueList);
        GuiProtocol::WidgetReplyStatus_E SetValueReq_UpdateWidget(std::shared_ptr<WidgetInterface_I> widget, WidgetTypes_E type, GuiProtocol::WidgetDataTypes_E dataType, GuiProtocol::WidgetValueVariant_T val);
        GuiProtocol::WidgetReplyStatus_E SetValueReq_UpdateTextWidget(std::shared_ptr<WidgetText_C> textWidget, GuiProtocol::WidgetDataTypes_E dataType, GuiProtocol::WidgetValueVariant_T val);
        void GuiServer_OnWidgetEventNotificationAckReceived(GuiProtocol::WidgetReplyStatus_E status, uint16_t windowId, uint16_t widgetId);

        /* Variables */
        bool                                                    _isRunning                      = false;
        bool                                                    _initialized                    = false;
        std::shared_ptr<TransportInterface>                     _transport;
        std::shared_ptr<GuiProtocol::GuiServer_C>               _guiServer;
        // uint16_t                                                _widgetKeyCount                 = 0;
        // std::map<uint16_t, WidgetInfo_T>                        _widgetMap;
        std::vector<GuiWindow_C>                                _windowList;
        std::ifstream                                           _configFile;
        std::string                                             _configFilePath;
        nlohmann::json                                          _jsonData;
        std::string                                             _glslVersion;
        std::string                                             _mainWindowName;
        // std::string                                             _widgetWindowName;
        SDL_GLContext                                           _glContext;
        SDL_Window*                                             _window;
        PortInfo_T                                              _guiClientPortInfo;
        uint32_t                                                _rxBufferSize;
        bool                                                    _testEventNotification        = false;
        ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>    _eventQueue;

};

#endif // DYNAMIC_GUI_H