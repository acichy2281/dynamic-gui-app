cmake_minimum_required( VERSION 3.29 )
set(CMAKE_CXX_STANDARD 17 )
set(CMAKE_CXX_STANDARD_REQUIRED True )

project( dynamic_gui_app )

# Check for maximum allowed CMake version
if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0)
    message(FATAL_ERROR "CMake version ${CMAKE_VERSION} is too high. Maximum allowed version is 3.31.")
endif()

# Set library / binary output directory 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(BUILD_SHARED_LIBS ON)

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set(FETCHCONTENT_BASE_DIR "3rd_party")  # Set base directory for fetched content
include(FetchContent)

# Fetch nlohmann_json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.1
)

# Fetch SDL
FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.0 
)

# Fetch imgui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.7  
)

# Make both available
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(nlohmann_json SDL imgui)

# Create library for imgui 
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
)

target_include_directories(imgui PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends    
)

target_sources(imgui PUBLIC
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(imgui PUBLIC SDL3::SDL3)

# Add applications 
add_subdirectory(gui_protocol_lib)
add_subdirectory(gui_app)
add_subdirectory(property_gatherer)