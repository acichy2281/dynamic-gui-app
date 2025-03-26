cmake_minimum_required( VERSION 3.29 )
set(CMAKE_CXX_STANDARD 17 )
set(CMAKE_CXX_STANDARD_REQUIRED True )

project( property_gatherer_library )

# Define the library
include(files.cmake)
add_library(${PROJECT_NAME} SHARED ${_PROPERTY_GATHERER_LIBRARY_SOURCEFILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/shared/common/inc)
if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/shared/platform/windows/inc )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/shared/platform/linux/inc )
endif()