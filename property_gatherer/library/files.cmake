file(GLOB PROPERTY_GATHERER_LIBRARY_SRC 
    "src/*"
    "${CMAKE_SOURCE_DIR}/shared/common/src/*"
)

file(GLOB PROPERTY_GATHERER_LIBRARY_INC 
    "inc/*"
    "${CMAKE_SOURCE_DIR}/shared/common/inc/*"
)

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
file(GLOB PROPERTY_GATHERER_LIBRARY_WIN 
    "${CMAKE_SOURCE_DIR}/shared/platform/windows/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/shared/platform/windows/inc/*.h"
)
else() 
set( PROPERTY_GATHERER_LIBRARY_WIN )
endif()

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
file(GLOB PROPERTY_GATHERER_LIBRARY_LINUX 
    "${CMAKE_SOURCE_DIR}/shared/platform/linux/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/shared/platform/linux/inc/*.h"
)
else() 
set( PROPERTY_GATHERER_LIBRARY_LINUX )
endif()

set( _PROPERTY_GATHERER_LIBRARY_SOURCEFILES
    ${PROPERTY_GATHERER_LIBRARY_SRC}
    ${PROPERTY_GATHERER_LIBRARY_INC}
    ${PROPERTY_GATHERER_LIBRARY_WIN}
    ${PROPERTY_GATHERER_LIBRARY_LINUX}
    files.cmake
)

source_group( "" FILES ${PROPERTY_GATHERER_LIBRARY_TOPLEVEL})
source_group( property_gatherer_library/src FILES ${PROPERTY_GATHERER_LIBRARY_SRC})
source_group( property_gatherer_library/inc FILES ${PROPERTY_GATHERER_LIBRARY_INC})
source_group( property_gatherer_library/platform/windows FILES ${PROPERTY_GATHERER_LIBRARY_WIN})
source_group( property_gatherer_library/platform/linux FILES ${PROPERTY_GATHERER_LIBRARY_LINUX})