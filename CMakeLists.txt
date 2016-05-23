cmake_minimum_required(VERSION 3.0)

project(cappuccino_filer)
include(ExternalProject)
set (cappuccino_filer_VERSION_MAJOR 0)
set (cappuccino_filer_VERSION_MINOR 1)

include_directories(${CMAKE_SOURCE_DIR}/third-party/actor-framework/libcaf_core)
include_directories(${CMAKE_SOURCE_DIR}/third-party/actor-framework/libcaf_io)
include_directories(${CMAKE_SOURCE_DIR}/third-party/date/)
include_directories(${CMAKE_SOURCE_DIR}/third-party/soci/include/)
#include_directories(${CMAKE_SOURCE_DIR}/third-party/sqlpp11/include/)
#include_directories(${CMAKE_SOURCE_DIR}/third-party/sqlpp11-connector-mysql/include)
#include_directories(${CMAKE_SOURCE_DIR}/backend)
include_directories(${CMAKE_SOURCE_DIR}/core)

# C++11 and Debug
# Note: we focus on gcc/clang right now, so we uses compiler specific solutions.
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(CMAKE_CXX_FLAGS "--std=c++14 -g -ggdb3 -Wno-deprecated-declarations -Wno-deprecated ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-g")
include_directories(${Qt5Core_INCLUDE_DIRS})

# Boost
FIND_PACKAGE(Boost 1.54.0 COMPONENTS system thread coroutine context filesystem REQUIRED)
message("Boost include dir: ${Boost_INCLUDE_DIR}")
message("Boost libraries: ${Boost_LIBRARIES}")
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
#MESSAGE("Qt flag: ${Qt5Core_INCLUDE_DIRS}")
#MESSAGE("Qt flag: ${Qt5Widgets_INCLUDE_DIRS}")
#MESSAGE("Flags: ${CMAKE_CXX_FLAGS}")

# Qt debugging messages
ADD_DEFINITIONS(-DQT_MESSAGELOGCONTEXT)

# Boost::future and so on
ADD_DEFINITIONS(-DBOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY)
ADD_DEFINITIONS(-DBOOST_THREAD_PROVIDES_FUTURE)
ADD_DEFINITIONS(-DBOOST_THREAD_PROVIDES_FUTURE_CONTINUATION)

# FindMySQL.cmake
include(cmake/FindMySQL.cmake)

#THIRD PARTIES
# CAF
include(ExternalProject)
SET(CAF_BIN "${CMAKE_BINARY_DIR}/caf")
ExternalProject_Add(CAF
	SOURCE_DIR "${CMAKE_SOURCE_DIR}/third-party/actor-framework/"
	GIT_SUBMODULES "${CMAKE_SOURCE_DIR}/third-party/actor-framework/"
	INSTALL_COMMAND ""
	CMAKE_ARGS "-DCAF_NO_OPENCL=1" "-DCAF_NO_UNIT_TESTS=1" "-DCAF_NO_EXAMPLES=1" "-DCAF_NO_BENCHMARKS=1"
	BINARY_DIR "${CAF_BIN}")
SET(CAF_LIBS ${CAF_BIN}/lib/libcaf_core.so ${CAF_BIN}/lib/libcaf_io.so)

# SOCI 
include(ExternalProject)
SET(SOCI_BIN "${CMAKE_BINARY_DIR}/soci")
ExternalProject_Add(SOCI
	SOURCE_DIR "${CMAKE_SOURCE_DIR}/third-party/soci/"
	GIT_SUBMODULES "${CMAKE_SOURCE_DIR}/third-party/soci"
	INSTALL_COMMAND ""
	BINARY_DIR "${SOCI_BIN}")
SET(SOCI_LIBS ${SOCI_BIN}/lib/libsoci_core.so ${SOCI_BIN}/lib/libsoci_mysql.so)
include_directories(${SOCI_BIN}/include/)

# Main program
AUX_SOURCE_DIRECTORY(apps/backend/ APP_BACKEND_SRC)
ADD_EXECUTABLE(cappuccino-filer ${APP_BACKEND_SRC})
QT5_USE_MODULES(cappuccino-filer Core)

# Core library
AUX_SOURCE_DIRECTORY(core CORE_MSRC)
ADD_LIBRARY(core SHARED ${CORE_MSRC})
ADD_DEPENDENCIES(core CAF)
TARGET_LINK_LIBRARIES(core ${Boost_LIBRARIES} Qt5::Core ${CAF_LIBS})

# Modules
## portal (embedded httpd) 
AUX_SOURCE_DIRECTORY(modules/portal PORTAL_MSRC)
ADD_LIBRARY(portal SHARED ${PORTAL_MSRC})
TARGET_LINK_LIBRARIES(portal core ${Boost_LIBRARIES} Qt5::Core)
SET_TARGET_PROPERTIES(portal PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules)

## database
AUX_SOURCE_DIRECTORY(modules/mariadb DATABASE_MSRC)
ADD_LIBRARY(mariadb SHARED ${DATABASE_MSRC})
ADD_DEPENDENCIES(mariadb SOCI)
TARGET_LINK_LIBRARIES(mariadb core ${MYSQL_LIBRARY} ${SOCI_LIBS})
SET_TARGET_PROPERTIES(mariadb PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules)

# storage (interacting with file system directly)
AUX_SOURCE_DIRECTORY(modules/storage STORAGE_MSRC)
ADD_LIBRARY(storage SHARED ${STORAGE_MSRC})
TARGET_LINK_LIBRARIES(storage core ${Boost_LIBRARIES} ${MYSQL_LIBRARY} ${SOCI_LIBS})
SET_TARGET_PROPERTIES(storage PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules)
AUX_SOURCE_DIRECTORY(modules/storage/tools UPDATEDB_SRC)
ADD_EXECUTABLE(updatedb ${UPDATEDB_SRC})

# Demo modules
ADD_LIBRARY(00-stub SHARED modules/demos/00-stub-module.cc)
TARGET_LINK_LIBRARIES(00-stub core ${Boost_LIBRARIES} Qt5::Core)
SET_TARGET_PROPERTIES(00-stub PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules)
ADD_LIBRARY(01-module-api SHARED modules/demos/01-module-api.cc)
TARGET_LINK_LIBRARIES(01-module-api core ${Boost_LIBRARIES} Qt5::Core)
SET_TARGET_PROPERTIES(01-module-api PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules)
ADD_LIBRARY(demo04 SHARED modules/demos/04-actor-basics.cc)
TARGET_LINK_LIBRARIES(demo04 core ${CAF_LIBS})
SET_TARGET_PROPERTIES(demo04 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules)

TARGET_LINK_LIBRARIES(cappuccino-filer core ${MYSQLPP11_LIBS} ${MYSQL_LIBRARY})
TARGET_LINK_LIBRARIES(updatedb core storage ${MYSQLPP11_LIBS} ${MYSQL_LIBRARY})

# vim: tw=0
