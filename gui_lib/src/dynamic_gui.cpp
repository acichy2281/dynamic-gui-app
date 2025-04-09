/* Project includes */
#include "stdafx.h"
#include "dynamic_gui.h"

DynamicGui_C::DynamicGui_C() : _guiServer(std::make_shared<GuiProtocol::GuiServer_C>())
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
    _configFile.open(configFilePath);
    if (true == _configFile.is_open())
    {
        _jsonData = nlohmann::json::parse(_configFile);
        try {
            if (true == _jsonSchemaValidationEnabled) { _jsonDataSchemaValidator.validate(_jsonData); }
            ParseJsonData();
            std::cout << "Valid JSON!" << std::endl;
            _isConfigFileSet = true;
            retVal = true;
        }
        catch (const std::exception& e) {
            std::cerr << "Validation Error: " << e.what() << std::endl;
        }

        if (nullptr != _onConfigFileSet) _onConfigFileSet(retVal);
    }
    return retVal;
}

void DynamicGui_C::SetWidgetList(std::vector<WidgetDescriptor_T>& descList)
{
    _widgetMap = std::make_shared<std::map<uint32_t, WidgetDescriptor_T>>();
    for (auto& desc : descList)
    {
        auto it = (*_widgetMap).find(desc.widgetId);
        if (it == (*_widgetMap).end())
        {
            (*_widgetMap)[desc.widgetId] = desc;
            std::cout << "Added widget " << desc.widgetId << ":" << desc.widgetName << " to the Widget Map\n";
        }
        else
        {
            std::cout << "Cannot add duplicate widget " << desc.widgetId << ":" << desc.widgetName << "\n";
        }
    }
    _guiServer->SetWidgetList(_widgetMap);
}

void DynamicGui_C::SetCallbacks(const DynamicGuiCallbacks_T& callBacks)
{
    if (callBacks.onWidgetEventOccured != nullptr)
    {
        _onWidgetEventOccured = callBacks.onWidgetEventOccured;
    }
    if (callBacks.onWindowClose != nullptr)
    {
        _onWindowClose = callBacks.onWindowClose;
    }
    if (callBacks.onConfigFileSet != nullptr)
    {
        _onConfigFileSet = callBacks.onConfigFileSet;
    }
}

bool DynamicGui_C::InitializeGui(DynamicGuiInitParams_T initParams)
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
        SetCallbacks(initParams.callbacks);

        if (false == initParams.jsonSchemaPath.empty())
        {
            _schemaFile.open(initParams.jsonSchemaPath);
            if (true == _configFile.is_open())
            {
                _jsonDataSchemaValidator = nlohmann::json_schema::json_validator();
                _jsonDataSchemaValidator.set_root_schema(nlohmann::json::parse(_schemaFile));
                _jsonSchemaValidationEnabled = true;
            }
            else
            {
                std::cerr << "Error: Failed to open schema file\n";
            }
        }
        
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
        std::shared_ptr<WidgetInterface_I> widget;
        if (true == _windowList.at(event->GetWindowId()).GetWidgetAt(event->GetWidgetId(), widget))
        {
            auto desc = widget->GetDescriptor();
            if (nullptr != _onWidgetEventOccured) _onWidgetEventOccured(desc, event->GetValue());
        }
        else 
        {
            std::throw_with_nested(std::runtime_error("Invalid Widget Event occured"));
        }
    }
}

void DynamicGui_C::ParseJsonData()
{
    std::vector<WidgetDescriptor_T> widgetDescList;
    _mainWindowName = _jsonData["Title"];
    uint16_t numWindows = 0;
    for (const auto& window : _jsonData["Windows"])
    {
        GuiWindow_C newWindow(window["Title"], numWindows);
        for (const auto& widget : window["WidgetList"])
        {
            /* Parse Widget Info */
            auto parsedWidgetInfo = _widgetFactory.ParseWidgetData(widget);
            parsedWidgetInfo->eventQueue = _eventQueue;

            /* Add Widget to Window */
            auto newWidget = newWindow.AddWidget(parsedWidgetInfo);

            /* Populate widget Descriptor List */
            widgetDescList.push_back(newWidget->GetDescriptor());

            std::cout << "Added Widget ID: " << newWidget->GetDescriptor().widgetId << ", Name: " << newWidget->GetDescriptor().widgetName << "\n";
            if (WidgetTypes_E::Menu == newWidget->GetWidgetType())
            {
                /* Loop through menu items to retreive widget descriptors */
                for (const auto& menuItem : std::dynamic_pointer_cast<WidgetMenu_C>(newWidget)->GetMenuItems())
                {
                    widgetDescList.push_back(menuItem->GetDescriptor());
                    std::cout << "Added Widget ID: " << menuItem->GetDescriptor().widgetId << ", Name: " << menuItem->GetDescriptor().widgetName << "\n";
                }
            }
        }
        _windowList.push_back(newWindow);
        numWindows++;
    }
    SetWidgetList(widgetDescList);
}

WidgetDescriptor_T DynamicGui_C::AddWidgetToWindow(std::shared_ptr<AddWidgetInfo_T> addWidgetInfo)
{
    auto newWidget = _windowList.at(addWidgetInfo->windowId).AddWidget(addWidgetInfo);
    _widgetMap->insert({ newWidget->GetDescriptor().widgetId, newWidget->GetDescriptor() });
    return newWidget->GetDescriptor();
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

// WidgetDescriptor_T& DynamicGui_C::GetWidgetDescriptor(uint32_t widgetId)
// {
//     return _guiServer->GetWidgetDescriptor(widgetId);
// }

const std::map<uint32_t, WidgetDescriptor_T>& DynamicGui_C::GetWidgetList() const
{
    return *_widgetMap;
}

const WidgetValueVariant_T DynamicGui_C::GetWidgetValue(uint32_t widgetId)
{
    WidgetValueVariant_T retVal;
    uint16_t windowId16 = widgetId >> 16;
    uint16_t widgetId16 = widgetId & 0xFFFF;

    std::shared_ptr<WidgetInterface_I> outWidget;
    if (false == _windowList.at(windowId16).GetWidgetAt(widgetId16, outWidget))
    {
        std::cout << "Unable to find widget " << widgetId << "\n";
    }
    else
    {
        retVal = outWidget->GetWidgetValue();
        std::cout << "Successfuly got widget " << widgetId << " value\n";
    }
    return retVal;
}

const std::shared_ptr<WidgetInterface_I> DynamicGui_C::GetWidget(uint32_t widgetId)
{
    std::shared_ptr<WidgetInterface_I> outWidget;
    uint16_t windowId16 = widgetId >> 16;
    uint16_t widgetId16 = widgetId & 0xFFFF;

    if (false == _windowList.at(windowId16).GetWidgetAt(widgetId16, outWidget))
    {
        std::cout << "Unable to find widget " << widgetId << "\n";
    }
    return outWidget;
}

const std::shared_ptr<WidgetInterface_I> DynamicGui_C::GetWidget(std::string widgetName)
{
    std::shared_ptr<WidgetInterface_I> outWidget;
    for (auto& window : _windowList)
    {
        for (auto& widget : window.GetWidgetList())
        {
            if (widgetName == widget.second->GetDescriptor().widgetName)
            {
                outWidget = widget.second;
                break;
            }
        }
    }
    return outWidget;
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
    else if (false == outWidget->SetWidgetValue(val))
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
    
    /* Populate callbacks */
    GuiProtocol::GuiServerInitParams_T initServerParams;
    initServerParams.callbacks.sendMessage = std::bind(&DynamicGui_C::GuiServer_SendMessage, this, std::placeholders::_1);
    initServerParams.callbacks.onWidgetListRequestReceived = std::bind(&DynamicGui_C::GuiServer_OnWidgetListRequestReceived, this);
    initServerParams.callbacks.onWidgetSetValueRequestReceived = std::bind(&DynamicGui_C::GuiServer_OnWidgetSetValueRequestReceived, this, std::placeholders::_1);
    initServerParams.callbacks.onWidgetEventNotificationAckReceived = std::bind(&DynamicGui_C::GuiServer_OnWidgetEventNotificationAckReceived, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    initServerParams.callbacks.onWidgetGetValueRequestReceived = std::bind(&DynamicGui_C::GuiServer_OnWidgetGetValueRequestReceived, this, std::placeholders::_1, std::placeholders::_2);

    if (GuiProtocol::GuiServerStatus_E::Success != _guiServer->Initialize(initServerParams))
    {
        std::cout << "Error! Failed to initialize GUI server\n";
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
            GuiProtocol::GuiServerStatus_E status = _guiServer->SendWidgetEventNotification(event.windowId, event.widgetId, event.value);
            if (GuiProtocol::GuiServerStatus_E::Success != status)
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
    auto retVal = GuiProtocol::WidgetReplyStatus_E::Error;
    uint16_t numSetValSuccess = 0;
    for (auto& widgetSetValue : widgetSetValueList)
    {
        std::shared_ptr<WidgetInterface_I> outWidget;
        if (false == _windowList.at(widgetSetValue.windowId).GetWidgetAt(widgetSetValue.widgetId, outWidget))
        {
            widgetSetValue.status = static_cast<uint16_t>(GuiProtocol::WidgetReplyStatus_E::InvalidWidgetId);
        }
        else if (true == outWidget->SetWidgetValue(widgetSetValue.val))
        {
            numSetValSuccess++;
            std::cout << "Successfuly set widget " << widgetSetValue.windowId << "." << widgetSetValue.widgetId << " value\n";
            widgetSetValue.status = static_cast<uint16_t>(GuiProtocol::WidgetReplyStatus_E::Success);
        }
    }

    if (widgetSetValueList.size() == numSetValSuccess)
    {
        retVal = GuiProtocol::WidgetReplyStatus_E::Success;
        std::cout << "All Set Value requests succeeded\n";
    }
    else if (0 < numSetValSuccess)
    {
        retVal = GuiProtocol::WidgetReplyStatus_E::PartialSuccess;
        std::cout << "Only " << numSetValSuccess << "out of " << widgetSetValueList.size() << " Set Value requests succeeded\n";
    }
    else
    {
        std::cout << "All Set Value requests failed\n";
    }
    return retVal;
}

GuiProtocol::WidgetReplyStatus_E DynamicGui_C::GuiServer_OnWidgetGetValueRequestReceived(uint32_t widgetId, WidgetValueVariant_T& val)
{
    auto retVal = GuiProtocol::WidgetReplyStatus_E::Error;
    uint16_t windowId16 = widgetId >> 16;
    uint16_t widgetId16 = widgetId & 0xFFFF;

    std::shared_ptr<WidgetInterface_I> outWidget;
    if (false == _windowList.at(windowId16).GetWidgetAt(widgetId16, outWidget))
    {
        std::cout << "Unable to find widget " << widgetId << "\n";
        retVal = GuiProtocol::WidgetReplyStatus_E::InvalidWidgetId;
    }
    else if (0 == (outWidget->GetDescriptor().flags & WidgetFlags_E::Readable))
    {
        std::cout << "Widget " << widgetId << " is not a readable widget\n";
        retVal = GuiProtocol::WidgetReplyStatus_E::AccessError;
    }
    else
    {
        val = outWidget->GetWidgetValue();
        std::cout << "Successfuly got widget " << widgetId << " value\n";
        retVal = GuiProtocol::WidgetReplyStatus_E::Success;
    }
    return retVal;
}

void DynamicGui_C::GuiServer_OnWidgetEventNotificationAckReceived(GuiProtocol::WidgetReplyStatus_E status, uint16_t windowId, uint16_t widgetId)
{
    if (GuiProtocol::WidgetReplyStatus_E::Success == status)
    {
        std::cout << "Widget Event Notification Ack Success\n";
    }
    else 
    {
        std::cout << "Error! Widget Event Notification Ack Failed\n";
    }
}