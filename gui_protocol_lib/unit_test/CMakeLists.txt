cmake_minimum_required( VERSION 3.29 )
set(CMAKE_CXX_STANDARD 17 )
set(CMAKE_CXX_STANDARD_REQUIRED True )

project(gui_protocol_library_unit_test)

set(GUI_PROTOCOL_UNIT_TEST_SRC 
    "src/main.cpp"
    "src/command_line_parser.cpp"
)
add_executable( ${PROJECT_NAME} ${GUI_PROTOCOL_UNIT_TEST_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/inc )

target_link_libraries( ${PROJECT_NAME} PRIVATE gui_protocol_library)

add_test(NAME ${PROJECT_NAME} COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})