set(PROPERTY_PRODUCER_SRC 
    "src/main.cpp"
    "src/property_producer.cpp"
    "${CMAKE_SOURCE_DIR}/shared/common/src/common_functions.cpp"
)

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
set(PROPERTY_PRODUCER_WIN 
    "${CMAKE_SOURCE_DIR}/shared/platform/windows/src/windows_platform_functions.cpp"
    "${CMAKE_SOURCE_DIR}/shared/platform/windows/src/windows_udp_transport.cpp"
)
else() 
set( PROPERTY_PRODUCER_WIN )
endif()

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
set(PROPERTY_PRODUCER_LINUX 
    "${CMAKE_SOURCE_DIR}/shared/platform/linux/src/linux_platform_functions.cpp"
    "${CMAKE_SOURCE_DIR}/shared/platform/linux/src/linux_udp_transport.cpp"
)
else() 
set( PROPERTY_PRODUCER_LINUX )
endif()

set( _PROPERTY_PRODUCER_SOURCEFILES
    ${PROPERTY_PRODUCER_SRC}
    ${PROPERTY_PRODUCER_WIN}
    ${PROPERTY_PRODUCER_LINUX}
)

source_group( "" FILES ${PROPERTY_PRODUCER_TOPLEVEL})
source_group( property_producer/common/src FILES ${PROPERTY_PRODUCER_SRC})
source_group( property_producer/common/inc FILES ${PROPERTY_PRODUCER_INC})
source_group( property_producer/platform/windows FILES ${PROPERTY_PRODUCER_WIN})
source_group( property_producer/platform/linux FILES ${PROPERTY_PRODUCER_LINUX})