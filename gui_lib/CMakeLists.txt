cmake_minimum_required( VERSION 3.29 )
set(CMAKE_CXX_STANDARD 17 )
set(CMAKE_CXX_STANDARD_REQUIRED True )

project( gui_library )
find_package(OpenGL REQUIRED)

include( files.cmake )
add_library(${PROJECT_NAME} SHARED ${_GUI_LIBRARY_SOURCEFILES})

# Project includes 
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/shared/common/inc)

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/shared/platform/windows/inc )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/shared/platform/linux/inc )
endif()

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    target_link_libraries(${PROJECT_NAME} PUBLIC imgui SDL3::SDL3 OpenGL::GL nlohmann_json::nlohmann_json gui_protocol_library nlohmann_json_schema_validator)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    target_link_libraries(${PROJECT_NAME} PUBLIC imgui SDL3::SDL3 OpenGL::GL nlohmann_json::nlohmann_json gui_protocol_library dl pthread nlohmann_json_schema_validator)
endif()

# Ensure static linking on Windows
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_STATIC)
endif()