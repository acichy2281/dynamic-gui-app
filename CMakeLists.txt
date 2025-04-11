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

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Force shared libraries" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(FETCHCONTENT_BASE_DIR "3rd_party")  # Set base directory for fetched content
include(FetchContent)

# Fetch nlohmann_json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.1
)

# Fetch imgui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.7  
)

# Fetch json-schema-validator
FetchContent_Declare(
    json-schema-validator
    GIT_REPOSITORY https://github.com/pboettch/json-schema-validator.git
    GIT_TAG 2.3.0
)

# Fetch SDL3
FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.6
)

# Fetch SDL3_Image
FetchContent_Declare(
    SDL_Image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_Image.git
    GIT_TAG release-3.2.4 
)

# Set 3rd party library options
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE) # Requires Perl and Nasm for libaom (AVIF image format)

# Make both available
FetchContent_MakeAvailable(nlohmann_json SDL SDL_Image imgui json-schema-validator)

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

target_link_libraries(imgui PRIVATE SDL3::SDL3)

# Add applications 
add_subdirectory(gui_protocol_lib)
add_subdirectory(gui_lib)
add_subdirectory(gui_app)
add_subdirectory(property_gatherer)