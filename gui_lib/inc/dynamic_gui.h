#ifndef DYNAMIC_GUI_H
#define DYNAMIC_GUI_H

#include "stdafx.h"
#include "gui_window.h"
#include "event_interface.h"
#include "widget_text.h"
#include "widget_text.h"
#include "widget_button.h"
#include "widget_slider.h"
#include "widget_checkbox.h"
#include "widget_radio.h"

struct GuiServerInitParams_T
{
    PortInfo_T serverInfo;
    PortInfo_T clientInfo;
    uint32_t rxBufferSize;
    int64_t spinLockSleepMs;
};

struct WidgetEventNotificationInfo_T
{
    uint16_t windowId;
    uint16_t widgetId;
    WidgetValueVariant_T value;
};

struct GuiLibraryCallbacks_T
{
    std::function<void(uint32_t, WidgetValueVariant_T)> onWidgetEventOccured = [](uint32_t, WidgetValueVariant_T) {};
};

class DynamicGui_C
{
    public: 
        DynamicGui_C();
        ~DynamicGui_C();

        /**
         * @brief Initializes SDL backend for GUI app
         * 
         */
        bool InitializeGui();

        /**
         * @brief Initializes and runs the GUI app blocking function that exits upon window close. 
         * 
         * @return true 
         * @return false 
         */
        bool RunGui();

        /**
         * @brief Set the Config File
         * 
         * @param configFilePath absolute file path to a config file 
         * 
         * @return true 
         * @return false 
         */
        bool SetConfigFile(const std::string& configFilePath);

        /**
         * @brief Set the Callbacks object
         * 
         * @param callBacks Callback struct that contains function pointers for GUI events
         */
        void SetCallbacks(const GuiLibraryCallbacks_T& callBacks);

        /**
         * @brief Run a GUI Server blocking function that initializes and polls a UDP socket to interface with a GUI
         * 
         * @param initParam initialization parameters for the GUI server (server and client port info, rx buffer size)
         * 
         * @return true 
         * @return false 
         */
        bool RunGuiServer(const GuiServerInitParams_T& initParams);

    private: 
        /* Functions */

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
         * @brief Processes the event queue and dispatches events to the appropriate widgets
         * 
         */
        void ProcessEventQueue();

        /**
         * @brief Performs cleanup of GUI app backend resources
         * 
         */
        void DeInitialize();

        /**
         * @brief Empty Callback function for widget events. Called when no callback is set by the user
         * 
         * @param widgetId 
         * @param val 
         */
        void DefaultOnWidgetEvent(uint32_t widgetId, WidgetValueVariant_T val);

        /* Gui Server functions  */
        bool GuiServer_ValidateInitParams(const GuiServerInitParams_T& initParams);
        void GuiServer_OnWidgetListRequestReceived();
        int32_t GuiServer_SendMessage(const std::vector<uint8_t>& message);
        GuiProtocol::WidgetReplyStatus_E GuiServer_OnWidgetSetValueRequestReceived(std::vector<GuiProtocol::WidgetSetValueResponseReturn_T>& widgetSetValueList);
        GuiProtocol::WidgetReplyStatus_E SetValueReq_UpdateWidget(std::shared_ptr<WidgetInterface_I> widget, WidgetTypes_E type, GuiProtocol::WidgetDataTypes_E dataType, WidgetValueVariant_T val);
        GuiProtocol::WidgetReplyStatus_E SetValueReq_UpdateTextWidget(std::shared_ptr<WidgetText_C> textWidget, GuiProtocol::WidgetDataTypes_E dataType, WidgetValueVariant_T val);
        void GuiServer_OnWidgetEventNotificationAckReceived(GuiProtocol::WidgetReplyStatus_E status, uint16_t windowId, uint16_t widgetId);

        /* Variables */

        // Flags
        bool                                                    _isGuiWindowRunning             = false;
        bool                                                    _isGuiServerRunning             = false;
        bool                                                    _initialized                    = false;
        bool                                                    _isConfigFileSet                = false;
        bool                                                    _guiServerSpinSleep             = false;
        
        // Config member variables
        std::ifstream                                           _configFile;
        std::string                                             _configFilePath;
        nlohmann::json                                          _jsonData;

        // Window member variables
        std::string                                             _mainWindowName;
        std::string                                             _glslVersion;
        std::vector<GuiWindow_C>                                _windowList;
        SDL_GLContext                                           _glContext;
        SDL_Window*                                             _window;
        
        // Event member variables
        ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>    _eventQueue;
        std::function<void(uint32_t, WidgetValueVariant_T)>     _onWidgetEventOccured;

        // GuiServer member variables
        std::shared_ptr<GuiProtocol::GuiServer_C>               _guiServer;
        ThreadSafeQueue_C<WidgetEventNotificationInfo_T>          _widgetEventNotificationQueue;
        PortInfo_T                                              _guiServerPortInfo;
        PortInfo_T                                              _guiClientPortInfo;
        std::shared_ptr<TransportInterface>                     _guiServerTransport;
        uint32_t                                                _guiServerRxBufferSize;
        int64_t                                                 _guiServerSpinSleepMs;

};

#endif // DYNAMIC_GUI_H