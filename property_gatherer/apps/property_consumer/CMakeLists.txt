cmake_minimum_required( VERSION 3.29 )
set(CMAKE_CXX_STANDARD 17 )
set(CMAKE_CXX_STANDARD_REQUIRED True )

project( property_consumer )

include( files.cmake )

add_executable(${PROJECT_NAME} ${_PROPERTY_CONSUMER_SOURCEFILES})

# Project includes 
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/shared/common/inc)
if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/shared/platform/windows/inc )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/shared/platform/linux/inc )
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC property_gatherer_library gui_protocol_library gui_library)