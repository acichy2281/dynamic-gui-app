cmake_minimum_required( VERSION 3.29 )
set(CMAKE_CXX_STANDARD 17 )
set(CMAKE_CXX_STANDARD_REQUIRED True )

project(gui_protocol_library_unit_test)

include(files.cmake)

include_directories( ${CMAKE_CURRENT_LIST_DIR}/inc )
add_executable( ${PROJECT_NAME} ${_GUI_PROTOCOL_LIBRARY_UNIT_TESTS_SOURCEFILES})

target_link_libraries( ${PROJECT_NAME} PRIVATE gui_protocol_library)

add_test(NAME ${PROJECT_NAME} COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})