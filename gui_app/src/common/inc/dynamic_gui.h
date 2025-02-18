#ifndef DYNAMIC_GUI_H
#define DYNAMIC_GUI_H

#include "stdafx.h"
#include "gui_window.h"


class DynamicGui_C : GuiProtocol::GuiServer_C
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
        void GuiServer_OnWidgetListRequestReceived() override;
        int32_t GuiServer_SendMessage(const std::vector<uint8_t>& message) override;

        /* Variables */
        bool                                                    _isRunning                      = false;
        bool                                                    _initialized                    = false;
        std::shared_ptr<TransportInterface>                     _transport;
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

};

#endif // DYNAMIC_GUI_H