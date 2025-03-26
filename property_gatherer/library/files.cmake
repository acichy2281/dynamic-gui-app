set(PROPERTY_GATHERER_LIBRARY_SRC 
    "src/property_consumer_api.cpp"
    "src/property_gatherer_messages.cpp"
    "src/property_producer_api.cpp"
    "${CMAKE_SOURCE_DIR}/shared/common/src/common_functions.cpp"
)

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
set(PROPERTY_GATHERER_LIBRARY_WIN 
    "${CMAKE_SOURCE_DIR}/shared/platform/windows/src/windows_platform_functions.cpp"
    "${CMAKE_SOURCE_DIR}/shared/platform/windows/src/windows_udp_transport.cpp"
)
else() 
set( PROPERTY_GATHERER_LIBRARY_WIN )
endif()

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
set(PROPERTY_GATHERER_LIBRARY_LINUX 
    "${CMAKE_SOURCE_DIR}/shared/platform/linux/src/linux_platform_functions.cpp"
    "${CMAKE_SOURCE_DIR}/shared/platform/linux/src/linux_udp_transport.cpp"
)
else() 
set( PROPERTY_GATHERER_LIBRARY_LINUX )
endif()

set( _PROPERTY_GATHERER_LIBRARY_SOURCEFILES
    ${PROPERTY_GATHERER_LIBRARY_SRC}
    ${PROPERTY_GATHERER_LIBRARY_WIN}
    ${PROPERTY_GATHERER_LIBRARY_LINUX}
)

source_group( "" FILES ${PROPERTY_GATHERER_LIBRARY_TOPLEVEL})
source_group( property_gatherer_library/src FILES ${PROPERTY_GATHERER_LIBRARY_SRC})
source_group( property_gatherer_library/platform/windows FILES ${PROPERTY_GATHERER_LIBRARY_WIN})
source_group( property_gatherer_library/platform/linux FILES ${PROPERTY_GATHERER_LIBRARY_LINUX})