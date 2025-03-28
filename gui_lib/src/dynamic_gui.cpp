/* Project includes */
#include "stdafx.h"
#include "dynamic_gui.h"

DynamicGui_C::DynamicGui_C() : _guiServer(std::make_shared<GuiProtocol::GuiServer_C>(
    std::bind(&DynamicGui_C::GuiServer_SendMessage, this, std::placeholders::_1),
    std::bind(&DynamicGui_C::GuiServer_OnWidgetListRequestReceived, this),
    std::bind(&DynamicGui_C::GuiServer_OnWidgetSetValueRequestReceived, this, std::placeholders::_1),
    std::bind(&DynamicGui_C::GuiServer_OnWidgetEventNotificationAckReceived, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))),
    _onWidgetEventOccured(std::bind(&DynamicGui_C::DefaultOnWidgetEvent, this, std::placeholders::_1, std::placeholders::_2))
{

}


DynamicGui_C::~DynamicGui_C()
{
    if (true == _initialized)
    {
        DeInitialize();
    }
}

bool DynamicGui_C::RunGui()
{
    bool retVal = false;
    if (false == _initialized)
    {
        std::cout << "Error: GUI not initialized\n";
    }
    else if (true == _isGuiWindowRunning)
    {
        std::cout << "Error: GUI window already running\n";
    }
    else if (false == ShowGui())
    {
        std::cout << "Failed to show GUI window\n";
    }
    else
    {
        std::cout << "Exiting GUI App\n";
        _isGuiServerRunning = false;
        retVal = true;
    }
    return retVal;
}

bool DynamicGui_C::SetConfigFile(const std::string& configFilePath)
{
    bool retVal = false;
    _configFilePath = configFilePath;
    _configFile.open(_configFilePath);

    if (true == _configFile.is_open())
    {
        _jsonData = nlohmann::json::parse(_configFile);
        ParseJsonData();
        _isConfigFileSet = true;
        retVal = true;
    }
    return retVal;
}

void DynamicGui_C::SetCallbacks(const GuiLibraryCallbacks_T& callBacks)
{
    if (callBacks.onWidgetEventOccured != nullptr)
    {
        _onWidgetEventOccured = callBacks.onWidgetEventOccured;
    }
    if (callBacks.onWindowClose != nullptr)
    {
        _onWindowClose = callBacks.onWindowClose;
    }
}

bool DynamicGui_C::InitializeGui()
{
    bool retVal = false;
    // Setup SDL
    if (true == _initialized)
    {
        std::cout << "Error: SDL already initialized\n";
    }
    else if (false == SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        std::cout << "Error: SDL_Init(): " << SDL_GetError() << "\n";
    }
    else
    {
        // Decide GL+GLSL versions
        #if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100 (WebGL 1.0)
        _glslVersion = "#version 100";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        #elif defined(IMGUI_IMPL_OPENGL_ES3)
        // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
        _glslVersion = "#version 300 es";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        #elif defined(__APPLE__)
        // GL 3.2 Core + GLSL 150
        _glslVersion = "#version 150";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        #else
        // GL 3.0 + GLSL 130
        _glslVersion = "#version 130";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        #endif
        // Create window with graphics context
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        std::cout << "GUI App initialized\n";
        retVal = true;
        _initialized = true;
    }

    return retVal;
}

bool DynamicGui_C::ShowGui()
{
    bool retVal = false;

    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    if (true == _mainWindowName.empty())
    {
        _mainWindowName = "Dynamic GUI App";
    }
    _window = SDL_CreateWindow(_mainWindowName.c_str(), 1280, 720, window_flags);
    if (_window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return false;
    }
    SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    _glContext = SDL_GL_CreateContext(_window);
    if (_glContext == nullptr)
    {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        return false;
    }
    SDL_GL_MakeCurrent(_window, _glContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    SDL_ShowWindow(_window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(_window, _glContext);
    ImGui_ImplOpenGL3_Init(_glslVersion.c_str());

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::string filePath = "";

    // Main loop
    _isGuiWindowRunning = true;
    std::cout << "Running GUI App\n";
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (true == _isGuiWindowRunning)
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
            {
                _isGuiWindowRunning = false;
            }
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(_window))
            {
                _isGuiWindowRunning = false;
            }
        }
        
        if (SDL_GetWindowFlags(_window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        for (auto& window : _windowList)
        {
            std::shared_ptr<WidgetInterface_I> widget;
            /*
            if (true == window.GetWidgetAt(0, widget))
            {
                if (auto textWidget = std::dynamic_pointer_cast<WidgetText_C>(widget))
                {
                    // textWidget->SetWidgetValue("Updating int: %" PRIu16, testPrintInt);
                }
            }
            */
           if (true == window.GetWidgetAt(0, widget))
           {
                if (auto buttonWidget = std::dynamic_pointer_cast<WidgetButton_C>(widget))
                {

                }
           }
            window.ShowWindow();
        }

        // Process the event Queue
        ProcessEventQueue();

        // Show JSON file selector window to load JSON config file 
        if (false == _isConfigFileSet)
        {
            ImGui::Begin("JSON File Selector"); 

            if (ImGui::Button("Choose File"))
            {
                filePath = GetJSONFile(); 
                if (false == filePath.empty())
                {
                    if (true == SetConfigFile(filePath))
                    {
                        SDL_SetWindowTitle(_window, _mainWindowName.c_str());
                        _isConfigFileSet = true;
                        std::cout << "JSON file set to: " << filePath << "\n";
                    }
                }
            }

            ImGui::Text("Select a JSON file to generate a GUI from.");               
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(_window);
    }
    if (nullptr != _onWindowClose)
    {
        _onWindowClose();
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif
    return true;
}

void DynamicGui_C::CloseGui()
{
    _isGuiWindowRunning = false;
}

void DynamicGui_C::ProcessEventQueue()
{
    if (0 != _eventQueue.Size())
    {
        std::cout << "Processing Widget Event\n";
        auto event = _eventQueue.Dequeue();

        std::cout << "Event Handler: window ID: " << event->GetWindowId() << ", widget ID: " << event->GetWidgetId() << "\n";
        if (true == _isGuiServerRunning)
        {
            _guiServerWidgetEventNotificationQueue.Enqueue({ event->GetWindowId(), event->GetWidgetId(), event->GetValue() });
        }
        auto desc = _guiServer->GetWidgetDescriptor((static_cast<uint32_t>(event->GetWindowId()) << 16) | static_cast<uint32_t>(event->GetWidgetId()));
        _onWidgetEventOccured(desc, event->GetValue());
    }
}

void DynamicGui_C::DefaultOnWidgetEvent(WidgetDescriptor_T& widgetId, WidgetValueVariant_T val)
{
    // Default callback for widget events
}

void DynamicGui_C::ParseJsonData()
{
    std::vector<WidgetDescriptor_T> widgetDescList;
    _mainWindowName = _jsonData["Title"];
    uint16_t numWindows = 0;
    // _widgetWindowName = _jsonData["MainWindow"]["Title"];
    for (const auto& window : _jsonData["Windows"])
    {
        GuiWindow_C newWindow(window["Title"], numWindows);
        _windowList.push_back(newWindow);

        for (const auto& widget : window["WidgetList"])
        {
            // WidgetInfo_T widgetInfo;
            std::string widgetTypeStr = widget["Type"];
            std::string widgetName = widget["Name"];
            std::regex textBoxRegex("text", std::regex_constants::icase);
            std::regex buttonRegex("button", std::regex_constants::icase);
            std::regex sliderRegex("slider", std::regex_constants::icase);
            std::regex checkboxRegex("checkbox", std::regex_constants::icase);
            std::regex radiobuttonRegex("radio", std::regex_constants::icase);

            auto staticFieldValue = widget.find("Static");
            bool isStaticField = false;
            if (staticFieldValue != widget.end()) {
                isStaticField = widget["Static"];
            }

            AddWidgetInfo_T addWidgetInfo;
            addWidgetInfo.windowId = numWindows;
            addWidgetInfo.widgetName = widgetName;
            addWidgetInfo.isReadable = true;
            addWidgetInfo.isWritable = true;
            addWidgetInfo.isInteractable = true;
            addWidgetInfo.isStaticField = isStaticField;
            if (true == std::regex_search(widgetTypeStr, textBoxRegex))
            {
                addWidgetInfo.type = WidgetTypes_E::TEXT;
                addWidgetInfo.dataType = GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_STRING;
                addWidgetInfo.defaultValue = widget["Value"].get<std::string>();
                widgetDescList.push_back(AddWidgetToWindow(addWidgetInfo));
                // std::shared_ptr<WidgetText_C> newWidget = std::make_shared<WidgetText_C>(numWindows);
                // newWidget->SetWidgetName(widgetName);
                // newWidget->SetWidgetValue(std::string(widget["Value"]).c_str());
                // newWidget->SetIsStatic(isStaticField);
                // auto widgetId = newWindow.AddWidget(newWidget);
                // auto widgetDes = GuiProtocol::GetTextWidgetDescriptor(numWindows, static_cast<uint16_t>(newWidgetId & 0xFFFF), true, true, widgetName);
                // widgetDescList.push_back(widgetDes);
            }
            else if (true == std::regex_search(widgetTypeStr, buttonRegex))
            {
                addWidgetInfo.type = WidgetTypes_E::BUTTON;
                addWidgetInfo.dataType = GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_BOOL;
                widgetDescList.push_back(AddWidgetToWindow(addWidgetInfo));
                // std::shared_ptr<WidgetButton_C> newWidget = std::make_shared<WidgetButton_C>(_eventQueue, numWindows);
                // newWidget->SetWidgetName(widgetName);
                // newWidget->SetIsStatic(isStaticField);
                // auto widgetId = newWindow.AddWidget(newWidget);
                // auto widgetDes = GuiProtocol::GetButtonWidgetDescriptor(numWindows, static_cast<uint16_t>(newWidgetId & 0xFFFF), widgetName);
                // widgetDescList.push_back(widgetDes);
            }
            else if (true == std::regex_search(widgetTypeStr, sliderRegex))
            {
                addWidgetInfo.type = WidgetTypes_E::SLIDER;
                addWidgetInfo.dataType = GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_FLOAT;
                addWidgetInfo.sliderMin = widget["MinValue"].get<float>();
                addWidgetInfo.sliderMax = widget["MaxValue"].get<float>();
                addWidgetInfo.defaultValue = widget["Value"].get<float>();
                widgetDescList.push_back(AddWidgetToWindow(addWidgetInfo));
                // auto newWidget = std::make_shared<WidgetSlider_C>(_eventQueue, numWindows);
                // float * value = new float(widget["Value"].get<float>());
                
                // newWidget->SetWidgetValue(std::string(widget["Text"]).c_str(), value, widget["MinValue"].get<float>(), widget["MaxValue"].get<float>());
                // newWidget->SetIsStatic(isStaticField);
                // newWindow.AddWidget(newWidget);
                // std::cout << "Adding slider to window\n";
            }
            else if (true == std::regex_search(widgetTypeStr, checkboxRegex)){
                addWidgetInfo.type = WidgetTypes_E::CHECKBOX;
                addWidgetInfo.dataType = GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_BOOL;
                addWidgetInfo.defaultValue = false;
                widgetDescList.push_back(AddWidgetToWindow(addWidgetInfo));
                // auto newWidget = std::make_shared<WidgetCheckbox_C>(_eventQueue, numWindows);
                // newWidget->SetWidgetValue(std::string(widget["Text"]).c_str(), false);
                // newWidget->SetIsStatic(isStaticField);
                // newWindow.AddWidget(newWidget);
                // std::cout << "Adding checkbox to window\n";
            }
            else if (true == std::regex_search(widgetTypeStr, radiobuttonRegex)){
                addWidgetInfo.type = WidgetTypes_E::RADIO;
                addWidgetInfo.dataType = GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_INT;
                std::vector<std::string> options(widget["Options"].begin(), widget["Options"].end());
                addWidgetInfo.radioWidgetOptionsList = options;
                widgetDescList.push_back(AddWidgetToWindow(addWidgetInfo));
                // auto newWidget = std::make_shared<WidgetRadio_C>(_eventQueue, numWindows);
                // std::vector<std::string> options(widget["Options"].begin(), widget["Options"].end());
                
                // newWidget->SetWidgetValue(options, 0);
                // newWindow.AddWidget(newWidget);
                // newWidget->SetIsStatic(isStaticField);
                
                // std::cout << "Adding radio button to window\n";
            }
            else {
                std::cout << "Unfamiliar widget\n";
            }
        }
        numWindows++;
    }

    _guiServer->SetWidgetList(widgetDescList);
}

WidgetDescriptor_T DynamicGui_C::AddWidgetToWindow(AddWidgetInfo_T addWidgetInfo)
{
    WidgetDescriptor_T retVal;
    std::shared_ptr<WidgetInterface_I> newWidget;
    switch (addWidgetInfo.type)
    {
        case WidgetTypes_E::TEXT:
            newWidget = std::make_shared<WidgetText_C>(addWidgetInfo.windowId);
            break;
        case WidgetTypes_E::BUTTON:
            newWidget = std::make_shared<WidgetButton_C>(_eventQueue, addWidgetInfo.windowId);
            break;
        case WidgetTypes_E::SLIDER:
            newWidget = std::make_shared<WidgetSlider_C>(_eventQueue, addWidgetInfo.windowId, addWidgetInfo.sliderMin, addWidgetInfo.sliderMax);
            break;
        case WidgetTypes_E::CHECKBOX:
            newWidget = std::make_shared<WidgetCheckbox_C>(_eventQueue, addWidgetInfo.windowId);
            break;
        case WidgetTypes_E::RADIO:
            newWidget = std::make_shared<WidgetRadio_C>(_eventQueue, addWidgetInfo.windowId, addWidgetInfo.radioWidgetOptionsList);
            break;
        default:
            std::cout << "Error! Unknown widget type\n";
            break;
    }
    if (nullptr != newWidget)
    {
        newWidget->SetWidgetName(addWidgetInfo.widgetName);
        newWidget->SetIsStatic(addWidgetInfo.isStaticField);
        newWidget->SetWidgetValue(addWidgetInfo.defaultValue);
        auto widgetId = _windowList.at(addWidgetInfo.windowId).AddWidget(newWidget);
        retVal = GuiProtocol::GetWidgetDescriptor(addWidgetInfo.windowId, 
                                                  widgetId, 
                                                  addWidgetInfo.isReadable, 
                                                  addWidgetInfo.isWritable, 
                                                  addWidgetInfo.isInteractable,
                                                  addWidgetInfo.isStaticField, 
                                                  addWidgetInfo.type,
                                                  addWidgetInfo.dataType,
                                                  addWidgetInfo.widgetName);
    }
    return retVal;
}

void DynamicGui_C::DeInitialize()
{
    std::cout << "Deinitializing GUI app\n";
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    std::cout << "Shutdown OpenGL3\n";
    ImGui_ImplSDL3_Shutdown();
    std::cout << "Shutdown SDL3\n";
    ImGui::DestroyContext();
    std::cout << "Destroyed Context\n";

    SDL_GL_DestroyContext(_glContext);
    SDL_DestroyWindow(_window);
    SDL_Quit();
    
    std::cout << "GUI app deinitialized\n";
    _initialized = false;
}

WidgetDescriptor_T& DynamicGui_C::GetWidgetDescriptor(uint32_t widgetId)
{
    return _guiServer->GetWidgetDescriptor(widgetId);
}

const std::map<uint32_t, WidgetDescriptor_T>& DynamicGui_C::GetWidgetList() const
{
    return _guiServer->GetWidgetList();
}

bool DynamicGui_C::SetWidgetValue(uint32_t widgetId, WidgetValueVariant_T val)
{
    bool retVal = false;
    uint16_t windowId16 = widgetId >> 16;
    uint16_t widgetId16 = widgetId & 0xFFFF;

    std::shared_ptr<WidgetInterface_I> outWidget;
    if (false == _windowList.at(windowId16).GetWidgetAt(widgetId16, outWidget))
    {
        std::cout << "Unable to find widget " << widgetId << "\n";
    }
    else if (GuiProtocol::WidgetReplyStatus_E::SET_VAL_SUCCESS != SetValueReq_UpdateWidget(outWidget, 
                                                                                           outWidget->GetType(), 
                                                                                           static_cast<GuiProtocol::WidgetDataTypes_E>(val.index()), 
                                                                                           val))
    {
        std::cout << "Failed to set widget " << widgetId << " value\n";
    }
    else
    {
        std::cout << "Successfuly set widget " << widgetId << " value\n";
        retVal = true;
    }
    return retVal;
}

bool DynamicGui_C::RunGuiServer(const GuiServerInitParams_T& initParams)
{
    std::cout << "Running GUI Server\n";
    
    if (false == GuiServer_ValidateInitParams(initParams))
    {
        return false;
    }
    _guiServerTransport = UdpTransportFactory::CreateTransport();
    _guiServerTransport->InitializeSocket(_guiServerPortInfo.destIp, _guiServerPortInfo.destPort);

    _isGuiServerRunning = true;
    while (true == _isGuiServerRunning)
    {
        if (true == _guiServerTransport->PollReceiveSocket())
        {
            std::string senderIp;
            uint16_t senderPort;
            auto msgBuf = std::make_unique<char []>(_guiServerRxBufferSize);
            auto msgSize = _guiServerTransport->ReceiveMessage(msgBuf, _guiServerRxBufferSize, senderIp, senderPort);
            std::cout << "GUI Server received " << msgSize << " bytes UDP msg from " << senderIp << ":" << senderPort << "\n";

            _guiServer->ProcessReceivedMessage(msgBuf, msgSize);
        }
        else if (0 != _guiServerWidgetEventNotificationQueue.Size())
        {
            std::cout << "Sending Widget Event Notification\n";
            auto event = _guiServerWidgetEventNotificationQueue.Dequeue();                   
            GuiProtocol::GuiServerReqStatus_E status = _guiServer->SendWidgetEventNotification(event.windowId, event.widgetId, event.value);
            if (GuiProtocol::GuiServerReqStatus_E::SUCCESS != status)
            {
                std::cout << "Error! Failed to send Widget Event Notification, status " << static_cast<int>(status) << "\n";
            }
            else
            {
                std::cout << "Widget Event Notification sent\n";
            }
        }
        else 
        {
            _guiServer->ProcessTimedActivities();
        }
        if (true == _guiServerSpinSleep) SleepMs(10);
        if (true == IsUserQuit() || _isGuiServerRunning == false) _isGuiServerRunning = false;
    }
    std::cout << "GUI Server stopped\n";
    _isGuiWindowRunning = false;
    return true;
}

bool DynamicGui_C::GuiServer_ValidateInitParams(const GuiServerInitParams_T& initParams)
{
    // Validate Client Port Info
    if (initParams.clientInfo.destIp.empty() || initParams.clientInfo.destPort == 0)
    {
        std::cout << "Error! Invalid GUI client port info\n";
        return false;
    }
    else
    {
        _guiClientPortInfo.destIp = initParams.clientInfo.destIp;
        _guiClientPortInfo.destPort = initParams.clientInfo.destPort;
    }

    // Validate Server Port Info
    if (initParams.serverInfo.destIp.empty() || initParams.serverInfo.destPort == 0)
    {
        std::cout << "Error! Invalid GUI server port info\n";
        return false;
    }
    else
    {
        _guiServerPortInfo.destIp = initParams.serverInfo.destIp;
        _guiServerPortInfo.destPort = initParams.serverInfo.destPort;
    }

    // Validate rxBufferSize 
    if (0 == initParams.rxBufferSize)
    {
        std::cout << "Error! Invalid GUI server rx buffer size\n";
        return false;
    }
    else
    {
        _guiServerRxBufferSize = initParams.rxBufferSize;
    }

    // Configure Spin Lock sleep 
    if (0 >= initParams.spinLockSleepMs)
    {
        _guiServerSpinSleep = false;
    }
    else
    {
        _guiServerSpinSleep = true;
        _guiServerSpinSleepMs = initParams.spinLockSleepMs;
    }
    
    return true;
}

void DynamicGui_C::GuiServer_OnWidgetListRequestReceived()
{
    std::cout << "Widget List Request Received\n";
}

int32_t DynamicGui_C::GuiServer_SendMessage(const std::vector<uint8_t>& message)
{
    return _guiServerTransport->TransportSendMessage(_guiClientPortInfo.destIp, _guiClientPortInfo.destPort, message);
}

GuiProtocol::WidgetReplyStatus_E DynamicGui_C::GuiServer_OnWidgetSetValueRequestReceived(std::vector<GuiProtocol::WidgetSetValueResponseReturn_T>& widgetSetValueList)
{
    auto retVal = GuiProtocol::WidgetReplyStatus_E::SET_VAL_ERROR;
    uint16_t numSetValSuccess = 0;
    for (auto& widgetSetValue : widgetSetValueList)
    {
        std::shared_ptr<WidgetInterface_I> outWidget;
        if (false == _windowList.at(widgetSetValue.windowId).GetWidgetAt(widgetSetValue.widgetId, outWidget))
        {
            widgetSetValue.status = static_cast<uint16_t>(GuiProtocol::WidgetReplyStatus_E::SET_VAL_UNKNOWN_WIDGET);
        }
        else
        {
            if (GuiProtocol::WidgetReplyStatus_E::SET_VAL_SUCCESS == SetValueReq_UpdateWidget(outWidget, 
                                                                                              static_cast<WidgetTypes_E>(widgetSetValue.widgetType), 
                                                                                              static_cast<GuiProtocol::WidgetDataTypes_E>(widgetSetValue.dataType), 
                                                                                              widgetSetValue.val))
            {
                numSetValSuccess++;
                std::cout << "Successfuly set widget " << widgetSetValue.windowId << "." << widgetSetValue.widgetId << " value\n";
                widgetSetValue.status = static_cast<uint16_t>(GuiProtocol::WidgetReplyStatus_E::SET_VAL_SUCCESS);
            }
        }
    }

    if (widgetSetValueList.size() == numSetValSuccess)
    {
        retVal = GuiProtocol::WidgetReplyStatus_E::SET_VAL_SUCCESS;
        std::cout << "All Set Value requests succeeded\n";
    }
    else if (0 < numSetValSuccess)
    {
        retVal = GuiProtocol::WidgetReplyStatus_E::SET_VAL_PARTIAL_SUCCESS;
        std::cout << "Only " << numSetValSuccess << "out of " << widgetSetValueList.size() << " Set Value requests succeeded\n";
    }
    else
    {
        std::cout << "All Set Value requests failed\n";
    }
    return retVal;
}

GuiProtocol::WidgetReplyStatus_E DynamicGui_C::SetValueReq_UpdateWidget(std::shared_ptr<WidgetInterface_I> widget, 
                                                                        WidgetTypes_E type, 
                                                                        GuiProtocol::WidgetDataTypes_E dataType, 
                                                                        WidgetValueVariant_T val)
{
    auto retVal = GuiProtocol::WidgetReplyStatus_E::SET_VAL_UNKNOWN_WIDGET;
    switch (static_cast<WidgetTypes_E>(type))
    {
        case WidgetTypes_E::TEXT:
            if (auto textWidget = std::dynamic_pointer_cast<WidgetText_C>(widget))
            {
                retVal = SetValueReq_UpdateTextWidget(textWidget, dataType, val);
            }
            break;
        
        default:
            break;
    }
    return retVal;
}

GuiProtocol::WidgetReplyStatus_E DynamicGui_C::SetValueReq_UpdateTextWidget(std::shared_ptr<WidgetText_C> textWidget, 
                                                                            GuiProtocol::WidgetDataTypes_E dataType, 
                                                                            WidgetValueVariant_T val)
{
    // bool setRetVal = false;
    // switch (dataType)
    // {
    //     case GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_STRING:
    //         setRetVal = textWidget->SetWidgetValue("%s", std::get<std::string>(val).c_str());
    //         break;

    //     case GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_INT:
    //         setRetVal = textWidget->SetWidgetValue("%d", std::get<int>(val));
    //         break;
        
    //     case GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_FLOAT:
    //         setRetVal = textWidget->SetWidgetValue("%f", std::get<float>(val));
    //         break;

    //     default:
    //         break;
    // }

    GuiProtocol::WidgetReplyStatus_E retVal = GuiProtocol::WidgetReplyStatus_E::SET_VAL_ERROR;
    if (true == textWidget->SetWidgetValue(val))
    {
        retVal = GuiProtocol::WidgetReplyStatus_E::SET_VAL_SUCCESS;
    }
    else
    {
        retVal = GuiProtocol::WidgetReplyStatus_E::SET_VAL_FAILED_TO_SET;
    }
    return retVal;
}

void DynamicGui_C::GuiServer_OnWidgetEventNotificationAckReceived(GuiProtocol::WidgetReplyStatus_E status, uint16_t windowId, uint16_t widgetId)
{
    if (GuiProtocol::WidgetReplyStatus_E::SET_VAL_SUCCESS == status)
    {
        std::cout << "Widget Event Notification Ack Success\n";
    }
    else 
    {
        std::cout << "Error! Widget Event Notification Ack Failed\n";
    }
}