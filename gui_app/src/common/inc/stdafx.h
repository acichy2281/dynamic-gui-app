#ifndef STDAFX_H
#define STADFX_H

/* System includes */
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <mutex>
#include <queue>
#include <memory>
#include <fstream>
#include <thread>
#include <unordered_map>
#include <regex>
#include <limits>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <future>
#include <map>
#include <inttypes.h>

/* Project includes */
#include "gui_server_api.h"

/* Shared includes */
#include "common_functions.h"
#include "custom_types.h"
#include "transport_factory.h"

/* 3rd party includes */
#include <nlohmann/json.hpp>
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"
#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#endif // STDAFX_H