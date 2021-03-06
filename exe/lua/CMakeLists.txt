# -*- cmake -*-

PROJECT(Lua)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
CMAKE_POLICY(VERSION 2.6)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# If you want to include Lua in a main project,
# you might want to define those variables yourself
IF(NOT Lua_IS_SUBPROJECT)
  INCLUDE(LuaPaths)
ENDIF()

FIND_PACKAGE(Readline)

INCLUDE(CheckLibraryExists)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckFunctionExists)

CHECK_FUNCTION_EXISTS(_longjmp LUA_USE_ULONGJMP)
CHECK_SYMBOL_EXISTS(sigaction signal.h LUA_USE_POSIX_SIGNAL)
CHECK_SYMBOL_EXISTS(isatty unistd.h LUA_USE_ISATTY)
CHECK_SYMBOL_EXISTS(mkstemp stdlib.h LUA_USE_MKSTEMP)
CHECK_SYMBOL_EXISTS(popen stdio.h LUA_USE_POPEN)
CHECK_LIBRARY_EXISTS(m sin "" LUA_USE_LIBM)

IF(READLINE_FOUND)
  SET(CMAKE_REQUIRED_INCLUDES ${READLINE_INCLUDE_DIR})
  SET(CMAKE_REQUIRED_LIBRARIES ${READLINE_LIBRARIES})
  CHECK_LIBRARY_EXISTS(${READLINE_readline_LIBRARY} readline "" LUA_USE_READLINE)
ENDIF(READLINE_FOUND)

FIND_LIBRARY(DL_LIBRARY "dl")
IF(DL_LIBRARY)
  SET(CMAKE_REQUIRED_LIBRARIES ${DL_LIBRARY})
ENDIF(DL_LIBRARY)
CHECK_FUNCTION_EXISTS(dlopen LUA_USE_DLOPEN)
IF(NOT WIN32 AND NOT LUA_USE_DLOPEN)
  MESSAGE(FATAL_ERROR "Cannot compile a useful lua.
Function dlopen() seems not to be supported on your platform.
Apparently you are not on a Windows platform as well.
So lua has no way to deal with shared libraries!")
ENDIF(NOT WIN32 AND NOT LUA_USE_DLOPEN)

IF(WIN32)
  SET(LUA_BUILD_AS_DLL 1)
ENDIF(WIN32)

IF (CMAKE_SHARED_LIBRARY_SUFFIX STREQUAL CMAKE_SHARED_MODULE_SUFFIX)
  SET(LUA_USE_MODULE_AND_LIBRARY 0)
ELSE (CMAKE_SHARED_LIBRARY_SUFFIX STREQUAL CMAKE_SHARED_MODULE_SUFFIX)
  SET(LUA_USE_MODULE_AND_LIBRARY 1)
ENDIF (CMAKE_SHARED_LIBRARY_SUFFIX STREQUAL CMAKE_SHARED_MODULE_SUFFIX)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
CONFIGURE_FILE(src/luaconf.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/luaconf.h)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/luaconf.h src/lua.h src/lauxlib.h src/lualib.h
  DESTINATION "${Lua_INSTALL_INCLUDE_SUBDIR}")

SET(_lua_lib_src
  src/lapi.c src/lcode.c src/ldebug.c src/ldo.c src/ldump.c src/lfunc.c src/lgc.c src/llex.c src/lmem.c
  src/lobject.c src/lopcodes.c src/lparser.c src/lstate.c src/lstring.c src/ltable.c src/ltm.c
  src/lundump.c src/lvm.c src/lzio.c
  src/lauxlib.c src/lbaselib.c src/ldblib.c src/liolib.c
  src/lmathlib.c src/loslib.c src/ltablib.c src/lstrlib.c src/loadlib.c src/linit.c  
  src/luaconf.c ${CMAKE_CURRENT_BINARY_DIR}/luaconf.h)

SET(_lua_src src/lua.c)
SET(_luac_src src/luac.c src/print.c)

# Compile library as c++
SET_SOURCE_FILES_PROPERTIES(${_lua_lib_src} ${_lua_src} ${_luac_src} PROPERTIES
  OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/luaconf.h"
  LANGUAGE CXX)

# Make sure Clang does not complain
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-x c++" CXX_HAS_XCXX)
IF(CXX_HAS_XCXX)
  SET_SOURCE_FILES_PROPERTIES(${_lua_lib_src} ${_lua_src} ${_luac_src} PROPERTIES
    COMPILE_FLAGS "-x c++"
    )
ENDIF(CXX_HAS_XCXX)

# Compile shared library and regular lua executable
ADD_LIBRARY(liblua-shared SHARED ${_lua_lib_src})
SET_TARGET_PROPERTIES(liblua-shared PROPERTIES 
  PREFIX "lib" IMPORT_PREFIX "lib"
  OUTPUT_NAME "lua"
  LINKER_LANGUAGE CXX)

ADD_EXECUTABLE(luac ${_luac_src})
TARGET_LINK_LIBRARIES(luac liblua-shared)
SET_TARGET_PROPERTIES(luac PROPERTIES
  LINKER_LANGUAGE CXX)

ADD_EXECUTABLE(lua ${_lua_src})
TARGET_LINK_LIBRARIES(lua liblua-shared)
SET_TARGET_PROPERTIES(lua PROPERTIES LINKER_LANGUAGE CXX)

# Specify necessary libraries
IF(LUA_USE_LIBM)
  TARGET_LINK_LIBRARIES(liblua-shared m)
ENDIF(LUA_USE_LIBM)

IF(LUA_USE_READLINE)
  INCLUDE_DIRECTORIES(${READLINE_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(lua ${READLINE_LIBRARIES})
  SET(LUA_READLINE_INCLUDE ${READLINE_INCLUDE_DIR})
  SET(LUA_READLINE_LIBRARIES ${READLINE_LIBRARIES})
ELSE(LUA_USE_READLINE)
  SET(LUA_READLINE_INCLUDE)
  SET(LUA_READLINE_LIBRARIES)
ENDIF(LUA_USE_READLINE)

IF(DL_LIBRARY)
  TARGET_LINK_LIBRARIES(liblua-shared ${DL_LIBRARY})
ENDIF(DL_LIBRARY)

# Install files
INSTALL(TARGETS liblua-shared lua luac
  EXPORT torch-exports
  RUNTIME DESTINATION "${Lua_INSTALL_BIN_SUBDIR}"
  LIBRARY DESTINATION "${Lua_INSTALL_LIB_SUBDIR}"
  ARCHIVE DESTINATION "${Lua_INSTALL_LIB_SUBDIR}")

INSTALL(FILES src/lua.h src/lualib.h src/lauxlib.h  
  DESTINATION "${Lua_INSTALL_INCLUDE_SUBDIR}")
