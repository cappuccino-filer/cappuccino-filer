cmake_minimum_required(VERSION 3.0)

project(draft)
include(ExternalProject)
set (draft_VERSION_MAJOR 0)
set (draft_VERSION_MINOR 1)

include_directories(${CMAKE_SOURCE_DIR}/third-party/Simple-Web-Server)
include_directories(${CMAKE_SOURCE_DIR}/backend)
include_directories(${CMAKE_SOURCE_DIR}/backend/core)

# C++11 and Debug
# Note: we focus on gcc/clang right now, so we uses compiler specific solutions.
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(CMAKE_CXX_FLAGS "--std=c++11 -ggdb3 ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
include_directories(${Qt5Core_INCLUDE_DIRS})

# Boost
FIND_PACKAGE(Boost 1.54.0 COMPONENTS system thread coroutine context REQUIRED)
message("Boost include dir: ${Boost_INCLUDE_DIR}")
message("Boost libraries: ${Boost_LIBRARIES}")
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
#MESSAGE("Qt flag: ${Qt5Core_INCLUDE_DIRS}")
#MESSAGE("Qt flag: ${Qt5Widgets_INCLUDE_DIRS}")
#MESSAGE("Flags: ${CMAKE_CXX_FLAGS}")

# Qt debugging messages
ADD_DEFINITIONS(-DQT_MESSAGELOGCONTEXT)

AUX_SOURCE_DIRECTORY(backend/ SRC)

ADD_EXECUTABLE(draft ${SRC})
QT5_USE_MODULES(draft Core)

AUX_SOURCE_DIRECTORY(backend/core CORE_MSRC)
ADD_LIBRARY(core SHARED ${CORE_MSRC})
TARGET_LINK_LIBRARIES(core ${Boost_LIBRARIES} Qt5::Core)

AUX_SOURCE_DIRECTORY(backend/portal PORTAL_MSRC)
ADD_LIBRARY(portal SHARED ${PORTAL_MSRC})
TARGET_LINK_LIBRARIES(portal core ${Boost_LIBRARIES} Qt5::Core)
SET_TARGET_PROPERTIES(portal PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules)

TARGET_LINK_LIBRARIES(draft core)

# vim: tw=0
