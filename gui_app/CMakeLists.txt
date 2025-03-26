cmake_minimum_required( VERSION 3.29 )
set(CMAKE_CXX_STANDARD 17 )
set(CMAKE_CXX_STANDARD_REQUIRED True )

project( gui_app )

# Project includes 
set(GUI_APP_COMMON_SRC 
    "src/main.cpp"
    "src/command_line_parser.cpp"
)
add_executable(${PROJECT_NAME} ${GUI_APP_COMMON_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    target_link_libraries(${PROJECT_NAME} PRIVATE gui_library)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    target_link_libraries(${PROJECT_NAME} PRIVATE gui_library dl pthread)
endif()