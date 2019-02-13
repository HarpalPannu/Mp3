cmake_minimum_required(VERSION 3.12)
project(untitled)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
find_package (PkgConfig REQUIRED)
pkg_check_modules (GTK3 REQUIRED gtk+-3.0)
pkg_check_modules (TAGLIB REQUIRED taglib)
pkg_check_modules (ZLIB REQUIRED zlib)

include_directories (${GTK3_INCLUDE_DIRS})
include_directories (${TAGLIB_INCLUDE_DIRS})

link_directories (${GTK3_LIBRARY_DIRS})
link_directories (${TAGLIB_LIBRARY_DIRS})

add_definitions (${GTK3_CFLAGS_OTHER})

set(SOURCES hello-vsc.cpp print.cpp)
add_executable(Hz ${SOURCES})

target_link_libraries (Hz ${GTK3_LIBRARIES})
target_link_libraries (Hz ${TAGLIB_LIBRARIES})
target_link_libraries (Hz ${ZLIB_LIBRARIES})