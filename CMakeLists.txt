cmake_minimum_required(VERSION 3.0)

project(draft)
include(ExternalProject)
set (draft_VERSION_MAJOR 0)
set (draft_VERSION_MINOR 1)

include_directories(${CMAKE_SOURCE_DIR}/third-party/Simple-Web-Server)

# C++11 and Debug
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(CMAKE_CXX_FLAGS "--std=c++11 -ggdb3 ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
include_directories(${Qt5Core_INCLUDE_DIRS})

#MESSAGE("Qt flag: ${Qt5Core_INCLUDE_DIRS}")
#MESSAGE("Qt flag: ${Qt5Widgets_INCLUDE_DIRS}")
#MESSAGE("Flags: ${CMAKE_CXX_FLAGS}")

AUX_SOURCE_DIRECTORY(backend/ SRC)

add_executable(draft ${SRC})
qt5_use_modules(draft Core)
