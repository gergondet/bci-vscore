SET(EXEC_NAME bci-vscore)

SET(${EXEC_NAME}_SRC
plugin.cpp
)

ADD_LIBRARY(${EXEC_NAME} SHARED
${${EXEC_NAME}_SRC}
)

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/lib/include"
                 "${CMAKE_CURRENT_BINARY_DIR}"
             ${VISIONSYSTEM_INCLUDE_DIRS} 
             ${DL_INCLUDE_DIR} )

link_directories( "${VISIONSYSTEM_LIBRARY_DIRS}" )

IF(UNIX)
SET_TARGET_PROPERTIES(${EXEC_NAME}
  PROPERTIES
  LINK_FLAGS "-L/usr/local/lib -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system -L${VISIONSYSTEM_LIBRARY_DIRS}"
)
ENDIF(UNIX)

IF(WIN32)
SET_TARGET_PROPERTIES(${EXEC_NAME}
  PROPERTIES
  LINK_FLAGS "sfml-audio.lib sfml-graphics.lib sfml-window.lib sfml-system.lib"
)
ENDIF(WIN32)

# Add bci-interface cflags and link flags
ADD_DEFINITIONS(${BCIINTERFACE_CFLAGS})
TARGET_LINK_LIBRARIES(${EXEC_NAME} ${BCIINTERFACE_LIBS} ${VISIONSYSTEM_LIBRARIES})

set ( INSTALL_BIN_PATH bin )
set ( INSTALL_LIB_PATH lib )
set ( INSTALL_AR_PATH  lib )
set ( INSTALL_HEADERS_PATH include )

# Make relative paths absolute (needed later on)

foreach(p LIB BIN HEADERS AR)
        set(var INSTALL_${p}_PATH)
        if(NOT IS_ABSOLUTE "${${var}}")
                set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
        endif()
endforeach()

INSTALL(TARGETS ${EXEC_NAME}
    DESTINATION "${INSTALL_LIB_PATH}/vs_plugins" COMPONENT main)


