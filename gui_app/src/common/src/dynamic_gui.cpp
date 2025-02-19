/* Project includes */
#include "stdafx.h"
#include "dynamic_gui.h"
#include "text_widget.h"

DynamicGui_C::DynamicGui_C()
{
    _rxBufferSize = 2048;
    _transport = UdpTransportFactory::CreateTransport();
    _transport->InitializeSocket("127.0.0.1", 8001);
    _guiClientPortInfo.destIp = "127.0.0.1";
    _guiClientPortInfo.destPort = 8000;
}


DynamicGui_C::~DynamicGui_C()
{
    if (true == _initialized)
    {
        DeInitialize();
    }
}

bool DynamicGui_C::Run()
{
    bool retVal = false;
    if (false == Initialize())
    {
        std::cout << "Failed to initialize SDL\n";
    }
    else if (false == ShowGui())
    {
        std::cout << "Failed to show GUI window\n";
    }
    else
    {
        std::cout << "Exiting GUI App\n";
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
        retVal = true;
    }
    return retVal;
}

bool DynamicGui_C::Initialize()
{
    bool retVal = false;
    // Setup SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
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

    // Main loop
    _isRunning = true;
    std::cout << "Running GUI App\n";
    std::thread guiServerThread(&DynamicGui_C::RunGuiServer, this);
    uint16_t testPrintInt = 0;
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (true == _isRunning)
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
                _isRunning = false;
            }
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(_window))
            {
                _isRunning = false;
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

        // ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
        // ImGui::Begin(_widgetWindowName.c_str());
        // for (const auto& [key, widget] : _widgetMap)
        // {
        //     switch (widget.type)
        //     {
        //         case WidgetTypes_E::TEXT:
        //             ImGui::Text(std::any_cast<std::string>(widget.value).c_str());
        //             break;

        //         default:
        //             break;
        //     }
        // }
        // ImGui::End();

        for (auto& window : _windowList)
        {
            std::shared_ptr<WidgetInterface_I> widget;
            if (true == window.GetWidgetAt(0, widget))
            {
                if (auto textWidget = std::dynamic_pointer_cast<TextWidget_C>(widget))
                {
                    textWidget->SetWidgetValue("Updating int: %" PRIu16, testPrintInt);
                }
            }
            window.ShowWindow();
        }
        testPrintInt++;

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        /*if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);*/

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
           static float f = 0.0f;
           static int counter = 0;

           ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

           ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
           ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
           ImGui::Checkbox("Another Window", &show_another_window);

           ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
           ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

           if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
               counter++;
           ImGui::SameLine();
           ImGui::Text("counter = %d", counter);

           ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
           ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
           ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
           ImGui::Text("Hello from another window!");
           if (ImGui::Button("Close Me"))
               show_another_window = false;
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
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif
    guiServerThread.join();
    return true;
}

void DynamicGui_C::ParseJsonData()
{
    std::vector<GuiProtocol::WidgetDescriptor_T> widgetDescList;
    _mainWindowName = _jsonData["Title"];
    uint16_t numWindows = 0;
    // _widgetWindowName = _jsonData["MainWindow"]["Title"];
    for (const auto& window : _jsonData["Windows"])
    {
        GuiWindow_C newWindow(window["Title"], numWindows);

        for (const auto& widget : window["WidgetList"])
        {
            // WidgetInfo_T widgetInfo;
            std::string widgetTypeStr = widget["Type"];
            std::string widgetName = widget["Name"];
            std::regex textBoxRegex("text", std::regex_constants::icase);

            if (true == std::regex_search(widgetTypeStr, textBoxRegex))
            {
                // widgetInfo.type = WidgetTypes_E::TEXT;
                // widgetInfo.value = std::string(widget["Value"]);

                auto newWidget = std::make_shared<TextWidget_C>();
                newWidget->SetWidgetValue(std::string(widget["Value"]).c_str());
                auto widgetId = newWindow.AddWidget(newWidget);

                // std::cout << "Adding text widget to Main Window " << std::any_cast<std::string>(widgetInfo.value) << "\n";
                std::cout << "Adding text widget to Main Window\n";

                auto widgetDes = GuiServer_GetWidgetDesc(numWindows, widgetId, false, false, WidgetTypes_E::TEXT, GuiProtocol::WidgetDataTypes_E::STRING, widgetName);
                widgetDescList.push_back(widgetDes);
            }
            // _widgetMap[_widgetKeyCount] = widgetInfo;
            // _widgetKeyCount++;
            // std::cout << "Widget Count: " << _widgetKeyCount << "\n";
        }
        _windowList.push_back(newWindow);
        numWindows++;
    }

    GuiServer_SetWidgetList(widgetDescList);
}

void DynamicGui_C::DeInitialize()
{
    std::cout << "Deinitializing GUI app\n";
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(_glContext);
    SDL_DestroyWindow(_window);
    SDL_Quit();

    _initialized = false;
}

void DynamicGui_C::RunGuiServer()
{
    std::cout << "Running GUI Server\n";
    while (true == _isRunning)
    {
        if (true == _transport->PollReceiveSocket())
        {
            std::string senderIp;
            uint16_t senderPort;
            auto msgBuf = std::make_unique<char []>(_rxBufferSize);
            auto msgSize = _transport->ReceiveMessage(msgBuf, _rxBufferSize, senderIp, senderPort);
            std::cout << "GUI Server received " << msgBuf << " bytes UDP msg from " << senderIp << ":" << senderPort << "\n";

            GuiServer_ProcessReceivedMessage(msgBuf, msgSize);
        }
        GuiServer_ProcessTimedActivities();
        SleepMs(10);
    }
}

void DynamicGui_C::GuiServer_OnWidgetListRequestReceived()
{
    std::cout << "Widget List Request Received\n";
}

int32_t DynamicGui_C::GuiServer_SendMessage(const std::vector<uint8_t>& message)
{
    return _transport->TransportSendMessage(_guiClientPortInfo.destIp, _guiClientPortInfo.destPort, message);
}
