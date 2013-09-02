#
#  Copyright
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(PROJECT_NAME bci-vscore)
project(${PROJECT_NAME})

SET(PROJECT_VERSION 0.1)

IF(WIN32)
  SET(LIBDIR_KW "/LIBPATH:")
  SET(LIBINCL_KW "")
  SET(LIB_EXT ".lib")
ENDIF(WIN32)

IF(UNIX)
  SET(LIBDIR_KW "-L")
  SET(LIBINCL_KW "-l")
  SET(LIB_EXT "")
ENDIF(UNIX)

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

#
#  Look for dependencies
#

# Load Pkgconfig macros
INCLUDE(FindPkgConfig)
SET(PACKAGE_REQUIREMENTS "")

#
# Dependency on bci-interface
#
pkg_check_modules(BCIINTERFACE bci-interface)

IF(BCIINTERFACE_FOUND)
ADD_DEFINITIONS(-DWITH_COSHELL)
EXEC_PROGRAM(${PKG_CONFIG_EXECUTABLE} ARGS bci-interface --cflags
  OUTPUT_VARIABLE BCIINTERFACE_CFLAGS)
MESSAGE(STATUS "BCIINTERFACE_CFLAGS: ${BCIINTERFACE_CFLAGS}")
EXEC_PROGRAM(${PKG_CONFIG_EXECUTABLE} ARGS bci-interface --libs
  OUTPUT_VARIABLE BCIINTERFACE_LIBS)
MESSAGE(STATUS "BCIINTERFACE_LIBS: ${BCIINTERFACE_LIBS}")
ENDIF(BCIINTERFACE_FOUND)


IF(WIN32)
  ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS /wd4251 /wd4275 /wd4101 /wd4355")
ENDIF(WIN32)

### Search for Lib/Headers

find_package( visionsystem )

ADD_SUBDIRECTORY(src)

SET(BCI_DATA
data/DOWN_HL.png
data/DOWN.png
data/LEFT_HL.png
data/LEFT.png
data/RIGHT_HL.png
data/RIGHT.png
data/STOP_HL.png
data/STOP.png
data/SWITCH_HL.png
data/SWITCH.png
data/UP_HL.png
data/UP.png
)

INSTALL( FILES ${BCI_DATA}
         DESTINATION $ENV{HOME}/.vs_core/plugins/${PROJECT_NAME}/data )
