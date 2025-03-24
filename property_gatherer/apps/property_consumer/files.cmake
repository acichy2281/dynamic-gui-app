set(PROPERTY_CONSUMER_SRC 
    "src/main.cpp"
    "src/property_consumer.cpp"
    "${CMAKE_SOURCE_DIR}/shared/common/src/common_functions.cpp"
)

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
set(PROPERTY_CONSUMER_WIN 
    "${CMAKE_SOURCE_DIR}/shared/platform/windows/src/windows_platform_functions.cpp"
    "${CMAKE_SOURCE_DIR}/shared/platform/windows/src/windows_udp_transport.cpp"
)
else() 
set( PROPERTY_CONSUMER_WIN )
endif()

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
set(PROPERTY_CONSUMER_LINUX 
    "${CMAKE_SOURCE_DIR}/shared/platform/linux/src/linux_platform_functions.cpp"
    "${CMAKE_SOURCE_DIR}/shared/platform/linux/src/linux_udp_transport.cpp"
)
else() 
set( PROPERTY_CONSUMER_LINUX )
endif()

set( _PROPERTY_CONSUMER_SOURCEFILES
    ${PROPERTY_CONSUMER_SRC}
    ${PROPERTY_CONSUMER_WIN}
    ${PROPERTY_CONSUMER_LINUX}
)

source_group( "" FILES ${PROPERTY_CONSUMER_TOPLEVEL})
source_group( property_consumer/common/src FILES ${PROPERTY_CONSUMER_SRC})
source_group( property_consumer/common/inc FILES ${PROPERTY_CONSUMER_INC})
source_group( property_consumer/platform/windows FILES ${PROPERTY_CONSUMER_WIN})
source_group( property_consumer/platform/linux FILES ${PROPERTY_CONSUMER_LINUX})