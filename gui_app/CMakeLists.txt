cmake_minimum_required( VERSION 3.29 )
set(CMAKE_CXX_STANDARD 17 )
set(CMAKE_CXX_STANDARD_REQUIRED True )

project( gui_app )

find_package(OpenGL REQUIRED)

# Project includes 
include_directories( ${CMAKE_CURRENT_LIST_DIR}/src/common/inc )
include_directories( ${CMAKE_SOURCE_DIR}/shared/common/inc )
if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    include_directories( ${CMAKE_CURRENT_LIST_DIR}/src/platform/windows/inc )
    include_directories( ${CMAKE_SOURCE_DIR}/shared/platform/windows/inc )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
    include_directories( ${CMAKE_CURRENT_LIST_DIR}/src/platform/linux/inc )
    include_directories( ${CMAKE_SOURCE_DIR}/shared/platform/linux/inc )
endif()

include( files.cmake )

add_executable(${PROJECT_NAME} ${_GUI_APP_SOURCEFILES})

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    target_link_libraries(${PROJECT_NAME} PRIVATE imgui OpenGL::GL nlohmann_json::nlohmann_json gui_protocol_library)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    target_link_libraries(${PROJECT_NAME} PRIVATE imgui OpenGL::GL nlohmann_json::nlohmann_json gui_protocol_library dl pthread)
endif()