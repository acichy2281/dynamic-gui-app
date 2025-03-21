set(GUI_PROTOCOL_LIBRARY_SRC
    "src/gui_client_api.cpp"
    "src/gui_protocol_messages.cpp"
    "src/gui_server_api.cpp"
    "${CMAKE_SOURCE_DIR}/shared/common/src/common_functions.cpp"
)

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
set(GUI_PROTOCOL_LIBRARY_WIN 
    "${CMAKE_SOURCE_DIR}/shared/platform/windows/src/windows_platform_functions.cpp"
    "${CMAKE_SOURCE_DIR}/shared/platform/windows/src/windows_udp_transport.cpp"
)
else() 
set( GUI_PROTOCOL_LIBRARY_WIN )
endif()

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
set(GUI_PROTOCOL_LIBRARY_LINUX 
    "${CMAKE_SOURCE_DIR}/shared/platform/linux/src/linux_platform_functions.cpp"
    "${CMAKE_SOURCE_DIR}/shared/platform/linux/src/linux_udp_transport.cpp"
)
else() 
set( GUI_PROTOCOL_LIBRARY_LINUX )
endif()

set( _GUI_PROTOCOL_LIBRARY_SOURCEFILES
    ${GUI_PROTOCOL_LIBRARY_SRC}
    ${GUI_PROTOCOL_LIBRARY_WIN}
    ${GUI_PROTOCOL_LIBRARY_LINUX}
)

source_group( "" FILES ${GUI_PROTOCOL_LIBRARY_TOPLEVEL})
source_group( gui_protocol_library/src FILES ${GUI_PROTOCOL_LIBRARY_SRC})
source_group( gui_protocol_library/inc FILES ${GUI_PROTOCOL_LIBRARY_INC})
source_group( gui_protocol_library/platform/windows FILES ${GUI_PROTOCOL_LIBRARY_WIN})
source_group( gui_protocol_library/platform/linux FILES ${GUI_PROTOCOL_LIBRARY_LINUX})