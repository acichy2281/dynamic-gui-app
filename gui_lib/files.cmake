set(GUI_LIBRARY_COMMON_SRC 
    "src/event_radio_selected.cpp"
    "src/event_slider_set.cpp"
    "src/event_widget_close.cpp"
    "src/event_widget_open.cpp"
    "src/gui_window.cpp"
    "src/widget_button.cpp"
    "src/widget_checkbox.cpp"
    "src/widget_factory.cpp"
    "src/widget_menu_item.cpp"
    "src/widget_menu.cpp"
    "src/widget_radio.cpp"
    "src/widget_slider.cpp"
    "src/widget_text.cpp"
    "src/command_line_parser.cpp"
    "src/dynamic_gui.cpp"
    "src/event_button_press.cpp"
    "src/event_checkbox_toggle.cpp"
    
    "${CMAKE_SOURCE_DIR}/shared/common/src/common_functions.cpp"
)

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
set(GUI_LIBRARY_LIBRARY_WIN 
    "${CMAKE_SOURCE_DIR}/shared/platform/windows/src/windows_platform_functions.cpp"
    "${CMAKE_SOURCE_DIR}/shared/platform/windows/src/windows_udp_transport.cpp"
)
else() 
set( GUI_LIBRARY_LIBRARY_WIN )
endif()

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
set(GUI_LIBRARY_LIBRARY_LINUX 
    "${CMAKE_SOURCE_DIR}/shared/platform/linux/src/linux_platform_functions.cpp"
    "${CMAKE_SOURCE_DIR}/shared/platform/linux/src/linux_udp_transport.cpp"
)
else() 
set( GUI_LIBRARY_LIBRARY_LINUX )
endif()

set( _GUI_LIBRARY_SOURCEFILES
    ${GUI_LIBRARY_COMMON_SRC}
    ${GUI_LIBRARY_WIN}
    ${GUI_LIBRARY_LINUX}
)

source_group( "" FILES ${GUI_LIBRARY_TOPLEVEL})
source_group( gui_library/src FILES ${GUI_LIBRARY_COMMON_SRC})
source_group( gui_library/platform/windows FILES ${GUI_LIBRARY_WIN})
source_group( gui_library/platform/linux FILES ${GUI_LIBRARY_LINUX})